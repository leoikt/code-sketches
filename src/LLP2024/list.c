
// src: https://stepik.org/lesson/1443630/step/3?thread=solutions&unit=1462431
struct list {
  int64_t value;
  struct list* next;
};

struct list* node_create(int64_t value) {
  struct list* node = (struct list*)malloc(sizeof(struct list));
  if (node) {
    node->value = value;
    node->next = NULL;
  }
  return node;
}

// src: https://stepik.org/lesson/1443630/step/4?unit=1462431
void list_add_front(struct list** old, int64_t value) {
  struct list* new = node_create(value);
  new->next = *old;
  *old = new;
}
