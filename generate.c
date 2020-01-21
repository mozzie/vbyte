#include <inttypes.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if RAND_MAX/256 >= 0xFFFFFFFFFFFFFF
  #define LOOP_COUNT 1
#elif RAND_MAX/256 >= 0xFFFFFF
  #define LOOP_COUNT 2
#elif RAND_MAX/256 >= 0x3FFFF
  #define LOOP_COUNT 3
#elif RAND_MAX/256 >= 0x1FF
  #define LOOP_COUNT 4
#else
  #define LOOP_COUNT 5
#endif

uint64_t rand_uint64(int max_value) {
  uint64_t r = 0;
  for (int i=LOOP_COUNT; i > 0; i--) {
    r = r*(max_value + (uint64_t)1) + rand();
  }
  return r;
}

int main(int argc, char *argv[]) {
  if(argc<=2) {
    printf("Usage: generate <uint64_t count> <number max value>\n");
    printf("Give uint64_t count as argument\n");
    exit(1);
 }
  int arg1 = atoi(argv[1]);
  if(arg1 == 0) {
    exit(1);
  }
  char filename[30];
  strcpy(filename, "numbers_");
  strcat(filename, argv[1]);
  strcat(filename, "_");
  strcat(filename, argv[2]);
  srand ( time(NULL) );
  FILE* fout;
  fout = fopen(filename, "wb");
  int i;
  int max = atoi(argv[2]);
  uint64_t u64;
  for (i = 0; i < arg1; i++) {
      u64 = rand_uint64(max);
      fwrite(&u64, sizeof(u64), 1, fout);
  }
  fclose(fout);
  return 0;
}
