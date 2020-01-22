#include <sdsl/bit_vectors.hpp>
#include <sdsl/int_vector.hpp>
#include <sdsl/util.hpp>
#include <string>
#include <vector>
#include <ctime>
#include <chrono>

using namespace std;
using namespace sdsl;

const uint32_t cap = (uint32_t) 256;


vector<uint32_t> vb_encode_number(uint64_t number) {
  vector<uint32_t> v;
  while(1) {
    v.push_back(number%cap);
    if(number < cap) {
      break;
    }
    number/=cap;
  }
  *v.begin() += cap;
  return v;
}


int main(int argc, char *argv[]) {
  if(argc<=1) {
    cerr << "Give uint64_t file as parameter" << endl;
    return 1;
  }

  size_t fsize = util::file_size(argv[1]);
  if(fsize%8) {
    cout << "Wrong sized file" << endl;
    return 1;
  }
  vector<uint32_t> data;
  vector<uint64_t> original(fsize/8, 0);
  ifstream idt;
  idt.open(argv[1], ios::in|ios::binary);
  size_t index = 0;
  while( idt ) {
      uint32_t val[2] = {0};
      if (idt.read((char*)val, sizeof(val))) {
          original[index] = (uint64_t)(val[1]) << 32 | (uint64_t)(val[0]);
          index++;

      }
  }
  idt.close();

  //following for trimming out gaps
//  sort(original.begin(), original.end());
//  for(std::vector<uint64_t>::size_type i = original.size()-1; i > 1; i--) {
//    original[i] -= original[i-1];
//  }
  //

  for(vector<uint64_t>::const_iterator i = original.begin(); i != original.end(); i++) {
    vector<uint32_t> v = vb_encode_number(*i);
    data.insert(data.end(), v.rbegin(), v.rend());
  }

  cout << "Original data size: " << fsize << endl;
  cout << "VByte data vector size: " << sizeof(std::vector<uint32_t>) + sizeof(uint32_t)*data.size() << endl;

  bit_vector b(data.size(), 0);
  int_vector<> iv(data.size(), 0, 8);
  index = 0;

  for (vector<uint32_t>::const_iterator i = data.begin(); i != data.end(); i++, index++) {
    b[index] = (*i>>8) & 1;
    iv[index] = *i;
  }

  select_support_mcl<> sls(&b);

  cout << "continue-stop vector memory size: " << size_in_bytes(b) + sizeof(bit_vector) << "bytes" <<endl;
  cout << "data vector memory size: " << size_in_bytes(iv) + sizeof(int_vector<>) << "bytes" << endl;
  cout << "select support vector memory size: " << size_in_bytes(sls) + sizeof(select_support_mcl<>) << "bytes" << endl;

  srand((unsigned) time(0));

  cout << "number of numbers: " << original.size() << endl;
  int random_accesses = 1000000;

  vector<unsigned int> indices(random_accesses, 0);
  for(vector<uint64_t>::size_type i = 0; i < indices.size(); i++) {
    indices[i] = rand()%original.size();
  }
  std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
  uint64_t z = 0;
  for (vector<unsigned int>::const_iterator i = indices.begin(); i != indices.end(); i++) {
    index = *i;
//    cout << "original index " << index << " has " << original[index];

    int begin = index == 0 ? 0 : sls(index)+1;
    int end = sls(index+1);
//    cout << "Compressed data is between indices " << begin << "-" << end << endl;
    uint64_t val = 0;
    for(int j = begin; j <= end; j++) {
      val = (val << 8) + iv[j];
    }
    z = z^val;
  }
  cout << "making sure optimizer doesn't steal our code: " << z << endl;
  std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
  std::cout << "Time taken: " << std::chrono::duration_cast<std::chrono::milliseconds> (end - begin).count() << "[ms]" << std::endl;
  return 0;
}
