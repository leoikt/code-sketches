// src: https://stepik.org/lesson/1443633/step/4?unit=1462434

/*
struct list {
    int64_t value;
    struct list* next;
};
*/
/*You can use the following functions */
void print_int64(int64_t i);
struct list* node_create(int64_t value);
void list_destroy(struct list* list);

/*  Create a new list where each item is derived from the corresponding item of
 * l list with the f function */
struct list* list_map(const struct list* l, int64_t(f)(int64_t)) {
  if (l == NULL) return NULL;

  struct list* new = node_create(f(l->value));
  struct list* cur = new;
  l = l->next;

  while (l && cur) {
    cur->next = node_create(f(l->value));
    cur = cur->next;
    l = l->next;
  }
  return new;
}

static int64_t identity(int64_t a) { return a; }

struct list* list_copy(const struct list* l) { return list_map(l, identity); }

static int64_t absolute(int64_t a) { return a < 0 ? -a : a; }

struct list* list_abs(const struct list* l) { return list_map(l, absolute); }
