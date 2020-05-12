#include <sdsl/bit_vectors.hpp>
#include <sdsl/int_vector.hpp>
#include <string>
#include <vector>
#include <ctime>
#include <chrono>
#include <iterator>
#include <iostream>
#include <bitset>
#include "vbyte_helpers.hpp"
#include <valgrind/callgrind.h>

using namespace std;
using namespace sdsl;

uint32_t bit_length = 4;
unsigned int random_accesses = 1000000;
uint32_t cap = 1<<bit_length;
int bsize = 64;

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
  uint8_t *iv  = new uint8_t[data.size()/2+4];
  size_t index = 0;
  size_t num_index = -1;
  size_t num_p = 0;
  uint8_t overflow = 0;
  for (vector<uint32_t>::const_iterator i = data.begin(); i != data.end(); i++, index++) {
    b[index] = (*i>>bit_length) & 1;
    if(b[index] == 1) {
        uint64_t orig = original[num_p];
        int diff = index-num_index;
        int shiftsize = bsize-(diff*bit_length);
        orig = (orig<<shiftsize)>>shiftsize;
        num_index++;
        if(num_index%2 == 1) {
          if(diff == 16) {
            overflow = orig>>60;
          }
          orig = orig<<4;
        }
        uint64_t *x = (uint64_t *)&iv[num_index/2];
        *x = *x | orig;
        if(num_index%2 == 1 && diff == 16) {
            iv[8+(num_index/2)] = overflow;
        }
        num_index=index;
        num_p++;
    }
  }

  bit_vector::select_1_type sls(&b);

  srand((unsigned) time(0));

  vector<unsigned int> indices(random_accesses, 0);
  for(vector<uint64_t>::size_type i = 0; i < indices.size(); i++) {
    indices[i] = rand()%original.size();
  }

uint64_t z = 0;
uint64_t *test;
uint8_t diff;
uint64_t val;
int begin;
cout << "bsize" << bsize << endl;
uint64_t bitmasks[16];
val = 0;
for(int i = 0;i < 16; i++) {
  val = val << 4;
  val = val | 0xF;
  bitmasks[i] = val;
}
chrono::steady_clock::time_point time_begin = chrono::steady_clock::now();
  for (vector<unsigned int>::const_iterator i = indices.begin(); i != indices.end(); i++) {
    index = *i;

    begin = index == 0 ? 0 : sls(index)+1;

    int offset = begin%bsize;
    int block = begin/bsize;
    uint64_t blokki = *(b.data()+block);
    val = blokki >> offset;
    if(!val) {
      uint64_t blokki2 = *(b.data()+block+1);
      val = val | (blokki2 <<(64-offset));
    }
      uint8_t diff = bits::lo(val);


    test = (uint64_t *)&iv[begin/2];

    val = *test>>((begin%2)*bit_length);

    val = val & bitmasks[diff];

    // the following is for accessing the value so it's not optimized out
    z = z^val;
    // sanity check for debugging the values
    if(0 && val != original[index]) {
      cout << "Did not match: " << val << " vs "  << original[index]   << endl;
    }
  }

  chrono::steady_clock::time_point time_end = chrono::steady_clock::now();
  cout << "checksum: " << z << endl;
  cout << "Time taken: " << chrono::duration_cast<chrono::milliseconds> (time_end - time_begin).count() << "[ms]" << endl;
  return 0;
}
