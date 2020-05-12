#include <sdsl/bit_vectors.hpp>
#include <sdsl/int_vector.hpp>
#include <string>
#include <vector>
#include <ctime>
#include <chrono>
#include <iterator>
#include <iostream>
#include "vbyte_helpers.hpp"
#include <valgrind/callgrind.h>

using namespace std;
using namespace sdsl;

uint32_t bit_length = 4;
unsigned int random_accesses = 1000000;
uint32_t cap = 1<<bit_length;
int main(int argc, char *argv[]) {
  if(argc<=1) {
    cerr << "Give uint64_t file as parameter" << endl;
    return 1;
  }

  vector<uint64_t> original = read_data_from_file(argv[1]);

  std::array<std::vector<uint32_t>, 16> data;
  for(int i = 0; i < 8; i++) {
    vector<uint32_t> vec;
    data[i] = vec;
  }
  for(vector<uint64_t>::const_iterator i = original.begin(); i != original.end(); i++) {
    vector<uint32_t> v = vb_encode_number(*i, cap);
    v.front() += cap;
    int level = 0;
    for(auto j = v.rbegin(); j != v.rend(); j++) {
      data[level].push_back(*j);
      level++;
    }
  }
  bit_vector b[16];
  std::array<std::vector<uint8_t>, 16> iv;
  size_t index = 0;
  for(int i = 0; i < 16; i++) {
    vector<uint8_t> vec(data[i].size()/2 + 1,0);
    bit_vector bv(data[i].size(), 0);
    index = 0;
    for (vector<uint32_t>::const_iterator j = data[i].begin(); j != data[i].end(); j++, index++) {
      uint8_t value = *j;
      if(index%2 == 0) {
        vec[index/2] = (value%cap)<<4;
      }
      else {
        vec[index/2] += value%cap;
      }
      bv[index] = (*j>>bit_length) & 1;
    }
    iv[i] = vec;
    b[i] = bv;
  }


  rank_support_v5<0> rb[16];
  for(int i =0; i < 16; i++) {
    rank_support_v5<0> r(&b[i]);
    rb[i] = r;
  }

  srand((unsigned) time(0));

  vector<unsigned int> indices(random_accesses, 0);
  for(vector<uint64_t>::size_type i = 0; i < indices.size(); i++) {
    indices[i] = rand()%original.size();
  }

  chrono::steady_clock::time_point time_begin = chrono::steady_clock::now();
  uint64_t z = 0;
  uint64_t val = 0;
  for (vector<unsigned int>::const_iterator i = indices.begin(); i != indices.end(); i++) {
    index = *i;
    int current_level = 0;
    val = 0;
    while(b[current_level][index] == 0) {
      uint8_t d = iv[current_level][index/2];
      if(index%2==1) {
        d=d&15;
      }
      else {
        d=d>>4;
      }
      val = (val<<bit_length) + d;
      index = rb[current_level](index);
      current_level++;
    }
    uint8_t e = iv[current_level][index/2];
    if(index%2==1) {
      e=e&15;
    }
    else {
      e=e>>4;
    }

    val = (val<<bit_length) + e;

    // the following is for accessing the value so it's not optimized out
    z = z^val;
    // sanity check for debugging the values
    if(0 && val != original[*i]) {
      cout << "Did not match: " << val << " vs " << original[*i] << endl;
    }
  }

  chrono::steady_clock::time_point time_end = chrono::steady_clock::now();
  cout << "checksum: " << z << endl;
  cout << "Time taken: " << chrono::duration_cast<chrono::milliseconds> (time_end - time_begin).count() << "[ms]" << endl;
  return 0;
}
