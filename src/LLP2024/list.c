
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

// src: https://stepik.org/lesson/1443630/step/5?unit=1462431
size_t list_length(const struct list* list) {
  size_t len = 0;
  struct list* pl = (struct list*)list;
  if (list) {
    len = 1;
    while (pl->next) {
      len++;
      pl = pl->next;
    }
  }
  return len;
}

// src: https://stepik.org/lesson/1443630/step/6?unit=1462431
void list_destroy(struct list* list) {
  while (list) {
    struct list* next = list->next;
    free(list);
    list = next;
  }
}

//  src: https://stepik.org/lesson/1443630/step/7?unit=1462431
struct list* list_last(struct list* list) {
  if (!list) return NULL;

  struct list* last = list;
  while (last->next) last = last->next;
  return last;
}

// src: https://stepik.org/lesson/1443630/step/8?unit=1462431
void list_add_back(struct list** old, int64_t value) {
  struct list* new = node_create(value);
  if (new) {
    if (old && *old) {
      struct list* last = list_last(*old);
      last->next = new;
    } else if (old) {
      *old = new;
    }
  }
}

// src: https://stepik.org/lesson/1443630/step/9?unit=1462431
int64_t list_sum(const struct list* list) {
  int64_t sum = 0;
  if (!list) return sum;
  struct list* pl = (struct list*)list;
  do {
    sum += pl->value;
    pl = pl->next;
  } while (pl);
  return sum;
}

// src: https://stepik.org/lesson/1443630/step/10?unit=1462431
struct maybe_int64 list_at(const struct list* list, size_t idx) {
  struct maybe_int64 res = {.valid = false};
  if (list) {
    struct list* pl = (struct list*)list;
    for (size_t i = 0; i < idx; i++) {
      pl = pl->next;
    }
    if (pl) {
      res.value = pl->value;
      res.valid = true;
    }
  }
  return res;
}

// src: https://stepik.org/lesson/1443630/step/11?unit=1462431
void list_add_front(struct list** old, int64_t value);

// create a flipped copy of the list
struct list* list_reverse(const struct list* list) {
  struct list* rlist = NULL;
  if (list) {
    struct list* pl = (struct list*)list;
    while (pl) {
      list_add_front(&rlist, pl->value);
      pl = pl->next;
    }
  }
  return rlist;
}
