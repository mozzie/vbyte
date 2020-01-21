#include <sdsl/bit_vectors.hpp>
#include <sdsl/int_vector.hpp>
#include <sdsl/util.hpp>
#include <string>
#include <vector>
#include <ctime>
using namespace std;
using namespace sdsl;

const uint8_t cap = (uint8_t) 128;

vector<uint8_t> vb_encode_number(uint64_t number) {
  vector<uint8_t> v;
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
  vector<uint8_t> data;
  vector<uint64_t> original(fsize/8, 0);
  ifstream idt;
  idt.open(argv[1], ios::in|ios::binary);
  size_t index = 0;
  while( idt ) {
      uint32_t val[2] = {0};
      if (idt.read((char*)val, sizeof(val))) {
        cout << val[0] << " " << val[1];
          original[index] = (uint64_t)(val[1]) << 32 | (uint64_t)(val[0]);
          cout << " " << original[index] << endl;
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
    vector<uint8_t> v = vb_encode_number(*i);
    data.insert(data.end(), v.rbegin(), v.rend());
  }

  cout << "Original data size: " << fsize << endl;
  cout << "VByte data vector size: " << sizeof(std::vector<uint8_t>) + sizeof(uint8_t)*data.size() << endl;

  bit_vector b(data.size(), 0);
  int_vector<> iv(data.size(), 0, 7);
  index = 0;

  for (vector<uint8_t>::const_iterator i = data.begin(); i != data.end(); i++, index++) {
    b[index] = (*i>>7) & 1;
    iv[index] = *i;
  }

  select_support_mcl<> sls(&b);

  cout << "continue-stop vector size: " << size_in_bytes(b) + sizeof(bit_vector) << endl;
  cout << "7-bit vector size: " << size_in_bytes(iv) + sizeof(int_vector<>) << endl;
  cout << "select support vector size: " << size_in_bytes(sls) + sizeof(select_support_mcl<>) << endl;

  srand((unsigned) time(0));

  cout << "number of numbers: " << original.size() << endl;

  for (int i = 1; i <= 20; i++) {
    index = rand()%original.size();
    cout << "original index " << index << " has " << original[index];

    int begin = index == 0 ? 0 : sls(index)+1;
    int end = sls(index+1);
//    cout << "Compressed data is between indices " << begin << "-" << end << endl;
    uint64_t val = 0;
    for(int j = begin; j <= end; j++) {
      val = (val << 7) + iv[j];
    }

    cout << ", decompressed value: " << val << endl;

    // DEBUG if fails
    if(val != original[index]) {
      val = 0;
      for(int j = begin; j <= end; j++) {
        cout << "data: " << val << " + " << iv[j];
        val = val << 7;
        val = val +iv[j];
        cout << " => " << val << endl;
      }
      vector<uint8_t> v = vb_encode_number(original[index]);
      for (vector<uint8_t>::const_iterator j = v.begin(); j != v.end(); ++j) {
        cout << (int)*j << endl;
      }
    }
    //END DEBUG
  }
  return 0;
}
