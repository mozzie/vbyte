#include <random>
#include <iostream>
#include <fstream>
using namespace std;

int main(int argc, char *argv[]) {
  if(argc<=2) {
    cout  << "Usage: generate <uint64_t count> <number of bits in number>" << endl;
    exit(1);
  }
  int amount = atoi(argv[1]);
  if(amount == 0) {
    exit(1);
  }
  int max = atoi(argv[2]);
  random_device rd;
  mt19937_64 gen(rd());
  uniform_int_distribution<unsigned long long> dis;

  string filename = "numbers_";
  filename.append(argv[1]);
  filename.append("_");
  filename.append(argv[2]);

  ofstream file(filename, ios::out | ios::binary);
  for (int n=0; n<amount; ++n) {
    uint64_t i = dis(gen)%max;
    file.write(reinterpret_cast<const char *>(&i), sizeof(uint64_t));
  }
  file.close();
  return 0;
}
