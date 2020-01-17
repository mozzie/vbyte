#include <sdsl/bit_vectors.hpp>
#include <sdsl/util.hpp>
#include <string>
#include <vector>
using namespace std;
using namespace sdsl;

const uint8_t cap = (uint8_t) 128;

vector<uint8_t> vb_encode_number(uint64_t number) {
  vector<uint8_t> v;
  while(1) {
    cout << number << std::endl;
    v.push_back(number%cap);
//    cout << (int)*v.end() << ": ";
    if(number < cap) {
      break;
    }
    number/=cap;
  }
  //cout << (int)*v.end();
  *v.begin() += cap;
  //cout << " " << (int)*v.end() << std::endl;
  return v;
}


int main(int argc, char *argv[]) {
  if(argc<=1) {
    cerr << "Give uint64_t file as parameter" << endl;
    return 1;
  }
//  int_vector_buffer<> ivb(argv[1]);
//  if (!ivb.is_open()) {
//    std::cerr << "ERROR: ivb could not be opend with file " << argv[1] << std::endl;
//    return 1;
//  }
  size_t fsize = util::file_size(argv[1]);
  if(fsize%8) {
    cout << "Wrong sized file" << endl;
    return 1;
  }
  vector<uint8_t> data;
  ifstream idt;
  idt.open(argv[1], ios::in|ios::binary);
  uint64_t cur;
  while( idt ) {
      uint32_t val[2] = {0};
      if (idt.read((char*)val, sizeof(val))) {
          cur = (uint64_t)(val[0]) << 32 | (uint64_t)(val[1]);
          vector<uint8_t> v = vb_encode_number(cur);
          data.insert(data.end(), v.rbegin(), v.rend());
        //  cout << cur << ' ' << endl;
      }
  }
  idt.close();
  bit_vector b(data.size(), 0);
  b[3] = 1;
  int index = 0;
  for (vector<uint8_t>::const_iterator i = data.begin(); i != data.end(); ++i) {
    b[4] = 1;
    b[index] = 1;
    index++;
    std::cout << index << ' ';
    std::cout << (int)*i << ' ';
    std::cout << ((*i>>7) & 1) << "==?" << b[index] << std::endl;
  }
  return 0;
}
