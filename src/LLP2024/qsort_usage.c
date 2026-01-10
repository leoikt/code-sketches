// We want the user structure to contain references only to strings from the
// heap.
typedef struct {
  char* addr;
} string_heap;

/*  The type for user IDs
    and its input and output specifiers for printf */
typedef uint64_t uid;
#define PRI_uid PRIu64
#define SCN_uid SCNu64

enum city { C_SARATOV, C_MOSCOW, C_PARIS, C_LOS_ANGELES, C_OTHER };

/*  An array where enumeration elements are mapped to their textual
 * representations */
const char* city_string[] = {[C_SARATOV] = "Saratov",
                             [C_MOSCOW] = "Moscow",
                             [C_PARIS] = "Paris",
                             [C_LOS_ANGELES] = "Los Angeles",
                             [C_OTHER] = "Other"};

struct user {
  const uid id;
  const string_heap name;
  enum city city;
};

int id_comparer(const struct user* x, const struct user* y) {
  if (x->id > y->id) return 1;
  if (x->id < y->id) return -1;
  return 0;
}

int suser_comparer(const void* _x, const void* _y) {
  return id_comparer((const struct user*)_x, (const struct user*)_y);
}

void users_sort_uid(struct user users[], size_t sz) {
  if (sz < 2) return;
  qsort(users, sz, sizeof(struct user), suser_comparer);
}

int suser_byname_comparer(const void* _x, const void* _y) {
  return strcmp(((const struct user*)_x)->name.addr,
                ((const struct user*)_y)->name.addr);
}

/* Sort an array of users by the name field */
/* The order of the lines is lexicographic; you can use
    a string comparator - standard strcmp function */
void users_sort_name(struct user users[], size_t sz) {
  if (sz < 2) return;
  qsort(users, sz, sizeof(struct user), suser_byname_comparer);
}

/* Sort the array by _text representation_ of the city */

#define CITY_COUNT (sizeof(city_string) / sizeof(city_string[0]))

// Безопасное получение строки города
static const char* get_city_name(enum city c) {
  /* Проверяем, что индекс не отрицательный и не выходит за пределы массива
     (Приведение к size_t обрабатывает отрицательные значения, если enum
     знаковый)*/
  if ((size_t)c < CITY_COUNT && city_string[c] != NULL) {
    return city_string[c];
  }
  /* Если город неизвестен или индекс поврежден, возвращаем
     строку для "Other" или пустую строку, чтобы strcmp не упал */
  return city_string[C_OTHER] ? city_string[C_OTHER] : "";
}

int suser_city_comparer(const void* _x, const void* _y) {
  const struct user* user_x = (const struct user*)_x;
  const struct user* user_y = (const struct user*)_y;

  const char* city_x_str = get_city_name(user_x->city);
  const char* city_y_str = get_city_name(user_y->city);

  return strcmp(city_x_str, city_y_str);
}

void users_sort_city(struct user users[], size_t sz) {
  if (users && sz > 1)
    qsort(users, sz, sizeof(struct user), suser_city_comparer);
}
