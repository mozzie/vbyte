#include "char_list.h"
#include "uint_list.h"

const uint64_t cap = (uint64_t) 128;

c_list *vb_encode_number(uint64_t number) {
  c_list* bytes = new_c_list();
  while(number >= cap) {
    c_prepend(bytes, number%cap);
    number/=cap;
  }
  bytes->last->val += 128;
  return bytes;
}

c_list *vb_encode(u_list* ulist) {
  c_list* bytestream = new_c_list();
  u_node* current = ulist->first;
  while(current != NULL) {
    c_extend(bytestream, vb_encode_number(current->val));
    current = current->next;
  }
  return bytestream;
}

u_list *vb_decode(c_list clist) {
  u_list *numbers = new_u_list();
}
