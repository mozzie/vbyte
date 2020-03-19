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
  cout << "Data vector memory size: " << sizeof(uint8_t) * data.size() << "bytes" << endl;
  srand((unsigned) time(0));

  cout << "number of numbers: " << original.size() << endl;

  vector<unsigned int> indices(random_accesses, 0);
  for(vector<uint64_t>::size_type i = 0; i < indices.size(); i++) {
    // easy way to avoid index out of bounds
    indices[i] = rand()%(original.size()-50);
  }

  uint64_t z = 0;
//  CALLGRIND_START_INSTRUMENTATION;
uint64_t *test;
int end;
uint8_t diff;
uint64_t val;
uint64_t bit_offset = 0;
int total_diff = 0;
int begin;
int bsize = 64;
cout << "bsize" << bsize << endl;
int shiftamt;
uint64_t maxuint = 0-1;
cout << maxuint << endl;
uint64_t bitmasks[8];
val = 0;
for(int i = 0;i < 8; i++) {
  val = val << 8;
  val = val | 0xFF;
  bitmasks[i] = val;
}
uint8_t loopcount = 50;
uint64_t blockcounter = 0;
chrono::steady_clock::time_point time_begin = chrono::steady_clock::now();
  for (vector<unsigned int>::const_iterator i = indices.begin(); i != indices.end(); i++) {
    index = *i;
    blockcounter = 0;
//TODO CALLGRIND THIS
    begin = index == 0 ? 0 : sls(index)+1;
//TODO see if ternary above can be substituted with just sls call
    int offset = (begin)%bsize;
    int block = begin>>6;
    uint64_t blokki = *(b.data()+block);
    bit_offset = blokki >> offset;
    total_diff = offset;
    if(!bit_offset) {
      blockcounter++;
      uint64_t blokki2 = *(b.data()+block+blockcounter);
      bit_offset = blokki2;

      diff = bits::lo(bit_offset);
      bit_offset = bit_offset >> (diff+1);
      total_diff = diff+1;
      diff = diff  + (64-offset);

    }
    else {
      diff = bits::lo(bit_offset);
      bit_offset = bit_offset >> (diff+1);
      total_diff = total_diff + diff+1;
    }
    for(int counter=0;counter<loopcount;counter++) {
//TODO see if this can be read on byte level instead of 64bit level - would save the extra calculation

      test = (uint64_t *)&iv[begin];
      val = *test & bitmasks[diff];
      z = z^val;
//      if((int)diff != sls(index+counter+1) - sls(index+counter) -1) {
//        cout << "Diff:" << (int)diff << " sls diff:" << (sls(index+counter+1) - sls(index+counter) -1) << endl;
//      }
      if(1 && val != original[index+counter]) {
        cout << "Did not match: " << val << " vs "  << original[index+counter]   << endl;
        cout << "index " << index+counter << ", counter: " << counter << endl;
        cout << "bit_offset " << bit_offset << ", total_diff " << (int)total_diff << endl;
      }
      begin = begin+diff+1;
      if(!bit_offset) {
        blockcounter++;
        uint64_t blokki2 = *(b.data()+block+blockcounter);
        bit_offset = blokki2;

        diff = bits::lo(bit_offset);
        int tempdiff = diff+1;
        bit_offset = bit_offset >> tempdiff;
        diff = diff  + (64-total_diff);
//          cout << "NEW BLOCK, total_diff:" << (int)total_diff << endl;
        total_diff = tempdiff;
      }
      else {
        diff = bits::lo(bit_offset);
        bit_offset = bit_offset >> (diff+1);
        total_diff = total_diff + diff + 1;

      }
    }
  }

  chrono::steady_clock::time_point time_end = chrono::steady_clock::now();
  cout << "checksum: " << z << endl;
  cout << "Time taken: " << chrono::duration_cast<chrono::milliseconds> (time_end - time_begin).count() << "[ms]" << endl;
  return 0;
}
