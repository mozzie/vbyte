#include <sdsl/bit_vectors.hpp>
#include <sdsl/int_vector.hpp>
#include <string>
#include <vector>
#include <ctime>
#include <chrono>
#include <iterator>
#include <iostream>
#include "vbyte_helpers.hpp"

using namespace std;
using namespace sdsl;

uint32_t bit_length = 8;
unsigned int random_accesses = 1000000;
uint32_t cap = 1<<bit_length;
int main(int argc, char *argv[]) {
  if(argc<=1) {
    cerr << "Give uint64_t file as parameter" << endl;
    return 1;
  }

  vector<uint64_t> original = read_data_from_file(argv[1]);

  vector<uint32_t> data;

  for(vector<uint64_t>::const_iterator i = original.begin(); i != original.end(); i++) {
    vector<uint32_t> v = vb_encode_number(*i, cap);
    v.back() += cap;
    data.insert(data.end(), v.begin(), v.end());
  }

  bit_vector b(data.size(), 0);
  uint8_t *iv  = new uint8_t[data.size()];
  size_t index = 0;

  for (vector<uint32_t>::const_iterator i = data.begin(); i != data.end(); i++, index++) {
    b[index] = (*i>>bit_length) & 1;
    iv[index] = *i;
  }

  bit_vector::select_1_type sls(&b);

  //select_support_mcl<> sls(&b);

  cout << "continue-stop vector memory size: " << size_in_bytes(b) + sizeof(bit_vector) << "bytes" <<endl;
  cout << "select support vector memory size: " << size_in_bytes(sls) + sizeof(select_support_mcl<>) << "bytes" << endl;

  srand((unsigned) time(0));

  cout << "number of numbers: " << original.size() << endl;

  vector<unsigned int> indices(random_accesses, 0);
  for(vector<uint64_t>::size_type i = 0; i < indices.size(); i++) {
    indices[i] = rand()%original.size();
  }

  chrono::steady_clock::time_point time_begin = chrono::steady_clock::now();
  uint64_t z = 0;
  for (vector<unsigned int>::const_iterator i = indices.begin(); i != indices.end(); i++) {
    index = *i;


    int begin = index == 0 ? 0 : sls(index)+1;
//    bit_vector::iterator iter = b.begin()+begin;
//    while(*iter == 0) {
//      iter++;
//    }

    uint64_t *test = (uint64_t *)&iv[begin];

//    uint8_t diff = std::distance(b.begin()+begin, iter);
    int end = sls(index+1);
    uint8_t diff = end-begin;
    uint64_t val = *test%(256<<(8*diff));

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
