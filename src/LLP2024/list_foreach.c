// src: https://stepik.org/lesson/1443633/step/2?unit=1462434
/*
struct list {
  int64_t value;
  struct list* next;
};
*/
void print_int64(int64_t i);

void print_int64_with_space(int64_t i) {
  print_int64(i);
  putchar(' ');
}

/* Run function f on each element of the list  */
void list_foreach(const struct list* l, void(f)(int64_t)) {
  while (l) {
    f(l->value);
    l = l->next;
  }
}

/* Print the list with foreach and additional function */
void list_print(const struct list* l) {
  list_foreach(l, print_int64_with_space);
}

// src: https://stepik.org/lesson/1443633/step/3?unit=1462434

/* Change each element of the list with the f function */
void list_map_mut(struct list* l, int64_t(f)(int64_t)) {
  while (l) {
    l->value = f(l->value);
    l = l->next;
  }
}

static int64_t triple(int64_t x) { return x * 3; }

/* Using list_map_mut multiply all the elements of the list by 3 */
void list_triple(struct list* l) { list_map_mut(l, triple); }
