#include <inttypes.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

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

uint64_t rand_uint64(void) {
  uint64_t r = 0;
  for (int i=LOOP_COUNT; i > 0; i--) {
    r = r*(RAND_MAX + (uint64_t)1) + rand();
  }
  return r;
}

int main(int argc, char *argv[]) {
  if(argc<=1) {
    printf("Give uint64_t count as argument");
    exit(1);
 }
  int arg1 = atoi(argv[1]);
  srand ( time(NULL) );
  int i;
  for (i = 0; i < arg1; i++) {
      printf("%" PRIu64 "\n", rand_uint64());
  }
  return 0;
}
