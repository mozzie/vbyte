#include <random>
#include <iostream>
#include <fstream>
using namespace std;

int main(int argc, char *argv[]) {
  if(argc<=2) {
    cout  << "Usage: generate <uint64_t count> <name>" << endl;
    exit(1);
  }
  int amount = atoi(argv[1]);
  if(amount == 0) {
    exit(1);
  }

  random_device rd;
  mt19937_64 gen(rd());
  uniform_int_distribution<unsigned long long> dis;

  string filename = "numbers_";
  filename.append(argv[1]);
  filename.append("_");
  filename.append(argv[2]);
   int maxvalues [8] = {1<<7,1<<8,1<<15,1<<16,1<<23,1<<24,1<<30,1<<30}; // <- all
  // int maxvalues [8] = {1<<7,1<<7,1<<7,1<<8,1<<8,1<<8,1<<16,1<<31}; // <- twolarge
  //int maxvalues [8] = {1<<2,1<<2,1<<3,1<<3,1<<3,1<<4,1<<4,1<<15}; // <- onelarge
  //int maxvalues [8] = {1<<2,1<<2,1<<3,1<<3,1<<3,1<<4,1<<4,1<<4}; // <- onlysmall
  ofstream file(filename, ios::out | ios::binary);
  for (int n=0; n<amount; ++n) {
    int rand = dis(gen)%8;
    uint64_t i = dis(gen)%maxvalues[rand];
    file.write(reinterpret_cast<const char *>(&i), sizeof(uint64_t));
  }
  file.close();
  return 0;
}
