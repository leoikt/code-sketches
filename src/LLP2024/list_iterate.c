// src: https://stepik.org/lesson/1443633/step/8?unit=1462434
/*
struct list {
    int64_t value;
    struct list* next;
};
*/
void print_int64(int64_t i);

struct list* node_create(int64_t value);
void list_destroy(struct list* list);

/*  Generate list of length sz using init value and f function
 The result: init, f(init), f(f(init)), ... */
struct list* list_iterate(int64_t init, size_t sz, int64_t(f)(int64_t)) {
  if (sz == 0) return NULL;

  struct list *l = node_create(init), *p = l;

  if (l)
    for (size_t i = 1; i < sz; i++) {
      p->next = node_create(init = f(init));
      if (p->next) {
        p = p->next;
      } else {
        list_destroy(l);
        return NULL;
      }
    }
  return l;
}
