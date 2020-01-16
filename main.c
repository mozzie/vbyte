#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "vbyte.h"

int main() {
  u_list* ulist = generate_random_u_list(100);
  print_u_list(ulist);
  c_list* test = vb_encode_number(ulist->first->val);
  print_c_list(test);
  c_list* test2 = vb_encode(ulist);
  print_c_list(test2);
  return 0;
}
