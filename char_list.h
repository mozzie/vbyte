typedef struct c_lists {
  struct c_nodes* first;
  struct c_nodes* last;
} c_list;

typedef struct c_nodes {
    unsigned char val;
    struct c_nodes* next;
} c_node;

c_list *new_c_list() {
    c_list* head = (c_list*) malloc(sizeof(c_list));
    head->first = NULL;
    head->last = NULL;
    return head;
}

void c_prepend(c_list* head, unsigned char value) {
    c_node* new = (c_node*) malloc(sizeof(c_node));
    new->val = value;
    new->next = head->first;
    head->first = new;
    if(head->last == NULL) {
      head->last = new;
    }
}

void c_append(c_list* head, unsigned char value) {
    c_node* new = (c_node*) malloc(sizeof(c_node));
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

void c_extend(c_list* orig, c_list* add) {
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

void print_c_list(c_list* head) {
    c_node* current = head->first;

    while (current != NULL) {
        printf("%c\n", current->val);
        current = current->next;
    }
}
