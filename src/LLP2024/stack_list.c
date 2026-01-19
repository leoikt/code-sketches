// src: https://stepik.org/lesson/1443635/step/8?unit=1462436

struct list {
  item value;
  struct list* next;
};

struct stack_int {
  struct list* items;
};

static struct list* c(item i, struct list* next) {
  struct list* result = malloc(sizeof(struct list));
  *result = (struct list){i, next};
  return result;
}

static struct stack_int stack_int_default() {
  return (struct stack_int){.items = NULL};
}

struct stack_int* stack_int_create() {
  struct stack_int* const result = malloc(sizeof(struct stack_int));
  *result = stack_int_default();
  return result;
}

static void list_free(struct list* l) {
  while (l) {
    struct list* cur = l;
    l = l->next;
    free(cur);
  }
}

void stack_int_destroy(struct stack_int* s) {
  if (s) {
    list_free(s->items);
    free(s);
  }
}

bool stack_int_empty(struct stack_int const* s) {
  return (s == NULL || s->items == NULL);
}

bool stack_int_full(struct stack_int const* s) { return false; }

bool stack_int_push(struct stack_int* s, item i) {
  if (!s) return false;
  struct list* new_node = c(i, s->items);
  if (!new_node) return false;
  s->items = new_node;
  return true;
}

struct maybe_item stack_int_pop(struct stack_int* s) {
  if (s == NULL || s->items == NULL) {
    return none_int;
  }

  struct list* cur = s->items;
  item val = cur->value;
  s->items = cur->next;
  free(cur);

  // Вызываем ту самую "неиспользуемую" функцию, чтобы ублажить компилятор
  return some_int(val.value);
}

static void stack_int_foreach(struct stack_int const* s, void(f)(item)) {
  for (struct list* l = s->items; l; l = l->next) f(l->value);
}

static void print_int64_cr(item i) { printf("%" STACK_ITEM_PRI "\n", i.value); }

// Note: here as well, we were able to leave the function "as is"
// simply by rewriting the foreach through which it is implemented
// This shows that traversing different data structures, folds
// and other popular higher-order functions actually isolate a piece of
// logic that distinguishes working with the same data, but differently
// structured. A strong justification for this is provided by category theory
// and algebraic data types.

void stack_int_print(struct stack_int const* s) {
  stack_int_foreach(s, print_int64_cr);
}
