#include <inttypes.h>
#include <time.h>

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

typedef struct u_lists {
  struct u_nodes* first;
  struct u_nodes* last;
} u_list;

typedef struct u_nodes {
    uint64_t val;
    struct u_nodes* next;
} u_node;

u_list *new_u_list() {
    u_list* head = (u_list*) malloc(sizeof(u_list));
    head->first = NULL;
    head->last = NULL;
    return head;
}

void u_prepend(u_list* head, unsigned char value) {
    u_node* new = (u_node*) malloc(sizeof(u_node));
    new->val = value;
    new->next = head->first;
    head->first = new;
    if(head->last == NULL) {
      head->last = new;
    }
}

void u_append(u_list* head, uint64_t value) {
    u_node* new = (u_node*) malloc(sizeof(u_node));
    new->val = value;
    new->next = NULL;
    if(head->last == NULL) {
      head->last = new;
    }
    else {
      head->last->next = new;
      head->last = new;
    }

    if(head->first == NULL) {
      head->first = new;
    }
}

void u_extend(u_list* orig, u_list* add) {
    if(add->first != NULL) {
      if(orig->first == NULL) {
        orig->first = add->first;
        orig->last = add->last;
      }
      else {
        orig->last->next = add->first;
        orig->last = add->last;
      }
    }
}

void print_u_list(u_list* head) {
    u_node* current = head->first;

    while (current != NULL) {
        printf("%" PRIu64 "\n", current->val);
        current = current->next;
    }
}

u_list *generate_random_u_list(int len) {
  srand ( time(NULL) );
  u_list* ulist = new_u_list();
  int i;
  for (i = 0; i < len; i++) {
    u_append(ulist, rand_uint64());
  }
  return ulist;
}
