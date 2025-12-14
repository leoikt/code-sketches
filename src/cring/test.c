#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "ring.h"

/* Вспомогательные функции для тестов */
static int test_count = 0;
static int passed_count = 0;

#define TEST_START(name)         \
  printf("🧪 Тест: %s\n", name); \
  test_count++
#define TEST_ASSERT(cond, msg)                                        \
  do {                                                                \
    if (!(cond)) {                                                    \
      printf("  ❌ Ошибка: %s (файл %s, строка %d)\n", msg, __FILE__, \
             __LINE__);                                               \
      return 0;                                                       \
    }                                                                 \
  } while (0)
#define TEST_PASS()       \
  printf("  ✅ Успех\n"); \
  passed_count++

void print_int_node(Node* node) { printf("%d ", node->data); }

int sum_nodes = 0;
void sum_node_data(Node* node) { sum_nodes += node->data; }

/* Тесты */
int test_create_destroy() {
  TEST_START("Создание и удаление кольца");

  Ring* ring = createRing();
  TEST_ASSERT(ring != NULL, "Кольцо не создано");
  TEST_ASSERT(getSize(ring) == 0, "Размер не 0");
  TEST_ASSERT(isEmpty(ring) == 1, "Кольцо не пустое");
  TEST_ASSERT(getCurrent(ring) == NULL, "Текущий элемент не NULL");

  destroyRing(ring);
  TEST_PASS();
  return 1;
}

int test_add_elements() {
  TEST_START("Добавление элементов");

  Ring* ring = createRing();

  addElement(ring, 10);
  TEST_ASSERT(getSize(ring) == 1, "Размер не 1");
  TEST_ASSERT(getCurrent(ring) != NULL, "Текущий элемент NULL");
  TEST_ASSERT(getCurrent(ring)->data == 10, "Неверные данные элемента");

  addElement(ring, 20);
  TEST_ASSERT(getSize(ring) == 2, "Размер не 2");

  addElement(ring, 30);
  TEST_ASSERT(getSize(ring) == 3, "Размер не 3");

  destroyRing(ring);
  TEST_PASS();
  return 1;
}

int test_remove_current() {
  TEST_START("Удаление текущего элемента");

  Ring* ring = createRing();
  addElement(ring, 10);
  addElement(ring, 20);
  addElement(ring, 30);

  int val = removeCurrent(ring);
  TEST_ASSERT(val == 30, "Удалено неверное значение");
  TEST_ASSERT(getSize(ring) == 2, "Размер не уменьшился");

  val = removeCurrent(ring);
  TEST_ASSERT(val == 20, "Удалено неверное значение");
  TEST_ASSERT(getSize(ring) == 1, "Размер не уменьшился");

  val = removeCurrent(ring);
  TEST_ASSERT(val == 10, "Удалено неверное значение");
  TEST_ASSERT(getSize(ring) == 0, "Размер не 0");
  TEST_ASSERT(isEmpty(ring) == 1, "Кольцо не пустое");

  // Тест удаления из пустого кольца
  val = removeCurrent(ring);
  TEST_ASSERT(val == -1, "Ожидалась ошибка при удалении из пустого кольца");

  destroyRing(ring);
  TEST_PASS();
  return 1;
}

int test_navigation() {
  TEST_START("Навигация по кольцу");

  Ring* ring = createRing();
  addElement(ring, 10);
  addElement(ring, 20);
  addElement(ring, 30);

  // Проверяем начальное положение
  TEST_ASSERT(getCurrent(ring)->data == 30,
              "Текущий элемент не последний добавленный");

  // Переходим вперед
  next(ring);
  TEST_ASSERT(getCurrent(ring)->data == 10, "Неверный переход вперед");

  next(ring);
  TEST_ASSERT(getCurrent(ring)->data == 20, "Неверный переход вперед");

  next(ring);
  TEST_ASSERT(getCurrent(ring)->data == 30, "Кольцо не замкнулось");

  // Переходим назад
  prev(ring);
  TEST_ASSERT(getCurrent(ring)->data == 20, "Неверный переход назад");

  prev(ring);
  TEST_ASSERT(getCurrent(ring)->data == 10, "Неверный переход назад");

  prev(ring);
  TEST_ASSERT(getCurrent(ring)->data == 30,
              "Кольцо не замкнулось при обратном обходе");

  destroyRing(ring);
  TEST_PASS();
  return 1;
}

int test_rotate() {
  TEST_START("Вращение кольца");

  Ring* ring = createRing();
  for (int i = 1; i <= 5; i++) {
    addElement(ring, i * 10);
  }

  // Текущий элемент должен быть 50
  TEST_ASSERT(getCurrent(ring)->data == 50, "Начальная позиция неверна");

  // Вращаем вперед
  rotate(ring, 2);
  TEST_ASSERT(getCurrent(ring)->data == 20, "Неверное вращение вперед");

  // Вращаем назад
  rotate(ring, -1);
  TEST_ASSERT(getCurrent(ring)->data == 10, "Неверное вращение назад");

  // Вращение на 0 шагов
  rotate(ring, 0);
  TEST_ASSERT(getCurrent(ring)->data == 10,
              "Вращение на 0 шагов изменило позицию");

  // Вращение по кругу
  rotate(ring, 5);
  TEST_ASSERT(getCurrent(ring)->data == 10, "Вращение на полный круг неверно");

  destroyRing(ring);
  TEST_PASS();
  return 1;
}

int test_reverse() {
  TEST_START("Реверс кольца");

  Ring* ring = createRing();
  addElement(ring, 10);
  addElement(ring, 20);
  addElement(ring, 30);

  // Порядок до реверса: 30 <-current, 10, 20
  TEST_ASSERT(getCurrent(ring)->data == 30, "Начальная позиция неверна");

  reverse(ring);

  // После реверса порядок должен быть: 30, 20, 10 <-current
  TEST_ASSERT(getCurrent(ring)->data == 10,
              "Текущий элемент после реверса неверен");

  next(ring);
  TEST_ASSERT(getCurrent(ring)->data == 20, "Порядок после реверса неверен");

  next(ring);
  TEST_ASSERT(getCurrent(ring)->data == 30, "Порядок после реверса неверен");

  // Реверс пустого кольца
  Ring* empty = createRing();
  reverse(empty);  // Не должно падать
  destroyRing(empty);

  // Реверс кольца из одного элемента
  Ring* single = createRing();
  addElement(single, 100);
  reverse(single);
  TEST_ASSERT(getCurrent(single)->data == 100,
              "Реверс одного элемента сломал кольцо");
  destroyRing(single);

  destroyRing(ring);
  TEST_PASS();
  return 1;
}

int test_remove_element() {
  TEST_START("Удаление элемента по значению");

  Ring* ring = createRing();
  addElement(ring, 10);
  addElement(ring, 20);
  addElement(ring, 30);
  addElement(ring, 20);  // Дубликат

  // Удаляем существующий элемент
  int result = removeElement(ring, 20);
  TEST_ASSERT(result == 1, "Элемент не найден");
  TEST_ASSERT(getSize(ring) == 3, "Размер не уменьшился");

  // Удаляем несуществующий элемент
  result = removeElement(ring, 99);
  TEST_ASSERT(result == 0, "Несуществующий элемент якобы удален");

  // Удаляем из пустого кольца
  Ring* empty = createRing();
  result = removeElement(empty, 10);
  TEST_ASSERT(result == 0, "Удаление из пустого кольца вернуло не 0");
  destroyRing(empty);

  destroyRing(ring);
  TEST_PASS();
  return 1;
}

int test_apply_function() {
  TEST_START("Применение функции к элементам");

  Ring* ring = createRing();
  for (int i = 1; i <= 5; i++) {
    addElement(ring, i);
  }

  // Тест суммирования
  sum_nodes = 0;
  applyFunction(ring, sum_node_data);
  TEST_ASSERT(sum_nodes == 15, "Сумма элементов неверна (1+2+3+4+5=15)");

  // Тест применения к пустому кольцу
  Ring* empty = createRing();
  applyFunction(empty, sum_node_data);  // Не должно падать
  TEST_ASSERT(sum_nodes == 15, "Сумма изменилась при обходе пустого кольца");

  destroyRing(empty);
  destroyRing(ring);
  TEST_PASS();
  return 1;
}

int test_edge_cases() {
  TEST_START("Граничные случаи");

  // Работа с NULL
  Ring* ring = NULL;
  addElement(ring, 10);  // Не должно падать
  removeCurrent(ring);
  next(ring);
  prev(ring);
  getSize(ring);
  isEmpty(ring);
  rotate(ring, 5);
  reverse(ring);
  removeElement(ring, 10);
  applyFunction(ring, print_int_node);

  // Создание и немедленное удаление
  ring = createRing();
  destroyRing(ring);

  TEST_PASS();
  return 1;
}

int main() {
  printf("🚀 Запуск тестов библиотеки Ring\n");
  printf("================================\n");

  test_create_destroy();
  test_add_elements();
  test_remove_current();
  test_navigation();
  test_rotate();
  test_reverse();
  test_remove_element();
  test_apply_function();
  test_edge_cases();

  printf("\n📊 Итоги тестирования:\n");
  printf("   Всего тестов: %d\n", test_count);
  printf("   Пройдено: %d\n", passed_count);
  printf("   Провалено: %d\n", test_count - passed_count);
  printf("   Успешность: %.1f%%\n", (float)passed_count / test_count * 100);

  if (passed_count == test_count) {
    printf("\n🎉 Все тесты пройдены успешно!\n");
    return 0;
  } else {
    printf("\n💥 Обнаружены ошибки!\n");
    return 1;
  }
}
