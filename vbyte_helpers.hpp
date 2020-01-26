#include <vector>
using namespace std;

#ifndef VBYTE_H
#define VBYTE_H

vector<uint32_t> vb_encode_number(uint64_t number, uint32_t cap);

vector<uint64_t> read_data_from_file(char* filename);

#endif
