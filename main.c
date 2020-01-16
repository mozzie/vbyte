#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "vbyte.h"

int main() {
  u_list* ulist = generate_random_u_list(10);
  print_u_list(ulist);
//  print_c_list(test);
  printf("<encode+decode>\n");
  c_list* test2 = vb_encode(ulist);
//  print_c_list(test2);
  u_list* test3 = vb_decode(test2);
  print_u_list(test3);
  return 0;
}
