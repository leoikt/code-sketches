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
