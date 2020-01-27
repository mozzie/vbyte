#include <vector>
#include <sdsl/util.hpp>

#include <cstdint> // include this header for uint64_t
using namespace std;

vector<uint32_t> vb_encode_number(uint64_t number, uint32_t cap = 128) {
  vector<uint32_t> v;
  while(1) {
    v.push_back(number%cap);
    if(number < cap) {
      break;
    }
    number/=cap;
  }
  return v;
}

vector<uint64_t> read_data_from_file(char* filename) {
  size_t fsize = sdsl::util::file_size(filename);
  if(fsize%8) {
    cout << "Wrong sized file" << endl;
    exit(1);
  }
  cout << "Original data size: " << fsize << endl;

  vector<uint64_t> original(fsize/8, 0);
  ifstream idt;
  idt.open(filename, ios::in|ios::binary);
  size_t index = 0;
  while( idt ) {
      uint32_t val[2] = {0};
      if (idt.read((char*)val, sizeof(val))) {
          original[index] = (uint64_t)(val[1]) << 32 | (uint64_t)(val[0]);
          index++;

      }
  }
  idt.close();
  return original;
}
