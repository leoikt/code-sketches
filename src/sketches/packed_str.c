// вдохновлено этой задачей:
// https://stepik.org/lesson/1443631/step/13?unit=1462432
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Структура для строки в куче
struct heap_string {
  char* addr;
};

// Тип упакованного указателя
typedef uintptr_t packed_ptr;

#define TAG_STR 1
#define TAG_MASK 1

// 1. Копирование строки в кучу
struct heap_string halloc(const char* s) {
  struct heap_string h;
  h.addr = malloc(strlen(s) + 1);
  if (h.addr) {
    strcpy(h.addr, s);
  }
  return h;
}

// 2. Освобождение памяти
void heap_string_free(struct heap_string h) { free(h.addr); }

// 3. Упаковка структуры heap_string в packed_ptr
// Мы устанавливаем младший бит в 1, чтобы пометить, что это указатель
packed_ptr pack_heap_string(struct heap_string h) {
  return (uintptr_t)h.addr | TAG_STR;
}

// 4. Распаковка обратно в структуру
// Мы должны очистить младший бит, чтобы получить корректный адрес
struct heap_string unpack_heap_string(packed_ptr p) {
  struct heap_string h;
  h.addr = (char*)(p & ~TAG_MASK);
  return h;
}

// Демонстрация использования
int main() {
  // Создаем строку в куче
  struct heap_string original = halloc("Hello 2025");

  // Упаковываем её в одно 64-битное число
  packed_ptr packed = pack_heap_string(original);
  printf("Packed value (hex): %p\n", (void*)packed);

  // Проверяем тег и распаковываем
  if (packed & TAG_STR) {
    struct heap_string unpacked = unpack_heap_string(packed);
    printf("Unpacked string: %s\n", unpacked.addr);

    // Освобождаем память через распакованную структуру
    heap_string_free(unpacked);
  }

  return 0;
}
