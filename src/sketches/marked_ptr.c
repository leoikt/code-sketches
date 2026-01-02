#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

// Используем uintptr_t, так как это целое число,
// размер которого совпадает с указателем
typedef uintptr_t packed_ptr;

#define TAG_INT 0x0
#define TAG_STR 0x1
#define TAG_MASK 0x1

// Упаковка целого числа: сдвигаем влево на 1 бит, последний бит оставляем 0
packed_ptr pack_int(int64_t i) {
  return (packed_ptr)((uint64_t)i << 1) | TAG_INT;
}

// Упаковка указателя: устанавливаем последний бит в 1
// (Предполагаем, что malloc вернул адрес, кратный 2)
packed_ptr pack_string(char* s) { return (uintptr_t)s | TAG_STR; }

// Проверка: строка
int is_string(packed_ptr p) { return (p & TAG_MASK) == TAG_STR; }

// Распаковка числа
int64_t unpack_int(packed_ptr p) { return (int64_t)p >> 1; }

// Распаковка указателя: ОБЯЗАТЕЛЬНО зануляем бит тега перед использованием
char* unpack_string(packed_ptr p) { return (char*)(p & ~TAG_MASK); }

// Освобождение памяти (только для строк)
void packed_destroy(packed_ptr p) {
  if (is_string(p)) {
    free(unpack_string(p));
  }
}

void print_packed(packed_ptr p) {
  if (is_string(p)) {
    printf("String: %s\n", unpack_string(p));
  } else {
    printf("Int: %" PRId64 "\n", unpack_int(p));
  }
}

int main() {
  // Пример с числом
  packed_ptr p1 = pack_int(42);
  print_packed(p1);

  // Пример со строкой (выделенной в куче)
  char* my_str = malloc(20);
  sprintf(my_str, "Hello 2025");
  packed_ptr p2 = pack_string(my_str);
  print_packed(p2);

  // Очистка
  packed_destroy(p1);  // ничего не сделает
  packed_destroy(p2);  // вызовет free()

  return 0;
}
