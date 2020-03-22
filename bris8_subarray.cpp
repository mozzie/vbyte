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

uint32_t bit_length = 8;
unsigned int random_accesses = 1000000;
uint32_t cap = 1<<bit_length;
int main(int argc, char *argv[]) {
  if(argc<=1) {
    cerr << "Give uint64_t file as parameter" << endl;
    return 1;
  }

  vector<uint64_t> original = read_data_from_file(argv[1]);

  std::array<std::vector<uint32_t>, 8> data;
  for(int i = 0; i < 8; i++) {
    vector<uint32_t> vec;
    data[i] = vec;
  }
  //uint8_t *iv  = new uint8_t[8][original.size()];
  int data_size = 0;
  for(vector<uint64_t>::const_iterator i = original.begin(); i != original.end(); i++) {
    vector<uint32_t> v = vb_encode_number(*i, cap);
    v.front() += cap;
    int level = 0;
    for(auto j = v.rbegin(); j != v.rend(); j++) {
      data[level].push_back(*j);
      level++;
      data_size++;
    }
  //  data.insert(data.end(), v.begin(), v.end());
  }
  bit_vector b[8];
  std::array<std::vector<uint8_t>, 8> iv;
  size_t index = 0;

  for(int i = 0; i < 8; i++) {
    vector<uint8_t> vec(data[i].size(),0);
    bit_vector bv(data[i].size(), 0);
    index = 0;
    for (vector<uint32_t>::const_iterator j = data[i].begin(); j != data[i].end(); j++, index++) {
        bv[index] = (*j>>bit_length) & 1;
        vec[index] = *j%(cap);
    }

    iv[i] = vec;
    b[i] = bv;
  }


  rank_support_v<0> rb[8];
  for(int i =0; i < 8; i++) {
    rank_support_v<0> r(&b[i]);
    rb[i] = r;
  }
  //select_support_mcl<> sls(&b);

//  cout << "continue-stop vector memory size: " << size_in_bytes(b) + sizeof(bit_vector) << "bytes" <<endl;
//  cout << "select support vector memory size: " << size_in_bytes(sls) + sizeof(select_support_mcl<>) << "bytes" << endl;

  srand((unsigned) time(0));

  cout << "number of numbers: " << original.size() << endl;

  vector<unsigned int> indices(random_accesses, 0);
  for(vector<uint64_t>::size_type i = 0; i < indices.size(); i++) {
    indices[i] = rand()%(original.size()-50);
  }

  chrono::steady_clock::time_point time_begin = chrono::steady_clock::now();
  uint64_t z = 0;
  int level = 0;
  uint64_t val = 0;
  uint8_t loopcount = 50;
  uint32_t level_indices[8];
  int max_level=-1;
  for (vector<unsigned int>::const_iterator i = indices.begin(); i != indices.end(); i++) {
    index = *i;
    max_level = -1;
    for(int counter=0;counter<loopcount;counter++) {
      index = *i + counter;
      level = 0;
      val = 0;
      while(b[level][index] == 0) {
        uint8_t d = iv[level][index];
        val = (val<<bit_length) + d;
        if(max_level < level) {
          level_indices[level] = rb[level](index);
          max_level = level;
        }
        index = level_indices[level];
        level_indices[level]++;
        level++;
      }
      val = (val<<bit_length) + iv[level][index];
      z = z^val;

      if(0 && val != original[*i+counter]) {
        cout << "Did not match: " << val << " vs " << original[*i+counter] << endl;
        cout << original[*i+counter]%cap << " " << original[*i+counter]/cap << endl;
      }
    }
  }

  chrono::steady_clock::time_point time_end = chrono::steady_clock::now();
  cout << "checksum: " << z << endl;
  cout << "Time taken: " << chrono::duration_cast<chrono::milliseconds> (time_end - time_begin).count() << "[ms]" << endl;
  int sum = 0;
  int bit_vec_size = 0;
  for(int i = 0; i < 8; i++) {
    sum += size_in_bytes(rb[i]);
    bit_vec_size += size_in_bytes(b[i]);
  }
  cout << "rank support size: " << sum << endl;
  cout << "bit vector size: " << bit_vec_size << endl;
  cout << "Data size: " << sizeof(uint8_t)*data_size << endl;
  cout << "total data size:" << bit_vec_size + sizeof(uint8_t)*data_size << endl;
  return 0;
}
