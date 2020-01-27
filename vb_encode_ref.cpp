#include <sdsl/bit_vectors.hpp>
#include <sdsl/int_vector.hpp>
#include <string>
#include <vector>
#include <ctime>
#include <chrono>
#include "vbyte_helpers.hpp"
#include <cmath>
using namespace std;
using namespace sdsl;

uint32_t bit_length = 7;
unsigned int random_accesses = 1000000;

int main(int argc, char *argv[]) {
  if(argc<=1) {
    cerr << "Give uint64_t file as parameter" << endl;
    return 1;
  }

  vector<uint64_t> original = read_data_from_file(argv[1]);

  vector<uint32_t> data;

  for(vector<uint64_t>::const_iterator i = original.begin(); i != original.end(); i++) {
    vector<uint32_t> v = vb_encode_number(*i, 1<<bit_length);
    data.insert(data.end(), v.rbegin(), v.rend());
  }

  bit_vector b(data.size(), 0);
  int_vector<> iv(data.size(), 0, bit_length);
  size_t index = 0;
  //unsigned int block_len = ((int)sqrt(original.size()-1))+1;
  unsigned int block_len = 1024;
  unsigned int block_amount = original.size()/block_len +1;
  cout << "block_len" << block_len << endl;
  int* blocks = new int[block_amount];

  for (vector<uint32_t>::const_iterator i = data.begin(); i != data.end(); i++, index++) {
    b[index] = (*i>>bit_length) & 1;
    iv[index] = *i;
  }

  select_support_mcl<> sls(&b);
  blocks[0] = 0; // because first data begins at 0
  for(int b_i = 1; b_i < block_amount; b_i++) {
    blocks[b_i] = sls(b_i*block_len)+1;
  }
  cout << "index vector size: " << sizeof(int)*block_amount << "bytes" << endl;
  cout << "continue-stop vector memory size: " << size_in_bytes(b) + sizeof(bit_vector) << "bytes" <<endl;
  cout << "data vector memory size: " << size_in_bytes(iv) + sizeof(int_vector<>) << "bytes" << endl;
  cout << "select support vector memory size: " << size_in_bytes(sls) + sizeof(select_support_mcl<>) << "bytes" << endl;

  srand((unsigned) time(0));
  uint32_t bitmask = ~((~0)<<bit_length);
  cout << bitmask << endl;
  cout << "number of numbers: " << original.size() << endl;
  vector<unsigned int> indices(random_accesses, 0);
  for(vector<uint64_t>::size_type i = 0; i < indices.size(); i++) {
    indices[i] = rand()%original.size();
  }

  chrono::steady_clock::time_point time_begin = chrono::steady_clock::now();
  uint64_t z = 0;
  uint32_t v;
  for (vector<unsigned int>::const_iterator i = indices.begin(); i != indices.end(); i++) {
    index = *i;
    int block_begin_index = blocks[index/block_len];
    int begin = index-index%block_len;
    vector<uint32_t>::const_iterator iter = data.begin() + block_begin_index;
    while(begin < index) {
      if(((*iter>>bit_length)&1) == 1) {
        begin++;
      }
      iter++;
    }
    uint64_t val = 0;
    do {
      v = *iter;
      val = (val << bit_length) + (v&bitmask);
      iter++;
    } while (((v>>bit_length) & 1) == 0);
    z = z^val;
    if(val != original[index]) {
      cout << "Did not match: " << val << " vs " << original[index] << endl;
      cout << "index " << index << endl;
    }
  }

  chrono::steady_clock::time_point time_end = chrono::steady_clock::now();
  cout << "checksum: " << z << endl;
  cout << "Time taken: " << chrono::duration_cast<chrono::milliseconds> (time_end - time_begin).count() << "[ms]" << endl;
  return 0;
}
