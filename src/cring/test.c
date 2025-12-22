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

/* Вспомогательные функции для тестов */
static int test_sum = 0;
void sum_node_data(Node* node) { test_sum += node->data; }

void reset_sum() { test_sum = 0; }

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

  // Добавляем первый элемент - он становится текущим
  addElement(ring, 10);
  TEST_ASSERT(getSize(ring) == 1, "Размер не 1");
  TEST_ASSERT(getCurrent(ring) != NULL, "Текущий элемент NULL");
  TEST_ASSERT(getCurrent(ring)->data == 10, "Неверные данные элемента");

  // Добавляем второй элемент - вставляется ПОСЛЕ текущего, current не меняется
  addElement(ring, 20);
  TEST_ASSERT(getSize(ring) == 2, "Размер не 2");
  TEST_ASSERT(getCurrent(ring)->data == 10,
              "Текущий элемент должен остаться 10");

  // Добавляем третий элемент - вставляется ПОСЛЕ текущего, current не меняется
  addElement(ring, 30);
  TEST_ASSERT(getSize(ring) == 3, "Размер не 3");
  TEST_ASSERT(getCurrent(ring)->data == 10,
              "Текущий элемент должен остаться 10");

  destroyRing(ring);
  TEST_PASS();
  return 1;
}

int test_remove_current() {
  TEST_START("Удаление текущего элемента");

  Ring* ring = createRing();
  addElement(ring, 10);  // current = 10
  addElement(ring, 20);  // вставка после 10, current остаётся 10
  addElement(ring, 30);  // вставка после 10, current остаётся 10

  // Порядок: 10(current) -> 30 -> 20 -> 10

  // Удаляем текущий элемент (10), current переходит к следующему (30)
  int val = removeCurrent(ring);
  TEST_ASSERT(val == 10, "Удалено неверное значение (ожидалось 10)");
  TEST_ASSERT(getSize(ring) == 2, "Размер не уменьшился");
  TEST_ASSERT(getCurrent(ring)->data == 30,
              "Текущий элемент должен перейти к 30");

  // Удаляем текущий элемент (30), current переходит к следующему (20)
  val = removeCurrent(ring);
  TEST_ASSERT(val == 30, "Удалено неверное значение (ожидалось 30)");
  TEST_ASSERT(getSize(ring) == 1, "Размер не уменьшился");
  TEST_ASSERT(getCurrent(ring)->data == 20,
              "Текущий элемент должен перейти к 20");

  // Удаляем текущий элемент (20), кольцо становится пустым
  val = removeCurrent(ring);
  TEST_ASSERT(val == 20, "Удалено неверное значение (ожидалось 20)");
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
  addElement(ring, 10);  // current = 10
  addElement(ring, 20);  // вставка после 10
  addElement(ring, 30);  // вставка после 10

  // Порядок: 10(current) -> 30 -> 20 -> 10
  TEST_ASSERT(getCurrent(ring)->data == 10,
              "Текущий элемент должен быть первым добавленным (10)");

  // Переходим вперед (по next)
  next(ring);  // 10 -> 30 (следующий после 10)
  TEST_ASSERT(getCurrent(ring)->data == 30,
              "Неверный переход вперед (10 -> 30)");

  next(ring);  // 30 -> 20
  TEST_ASSERT(getCurrent(ring)->data == 20,
              "Неверный переход вперед (30 -> 20)");

  next(ring);  // 20 -> 10
  TEST_ASSERT(getCurrent(ring)->data == 10, "Кольцо не замкнулось (20 -> 10)");

  // Переходим назад (по prev)
  prev(ring);  // 10 -> 20 (предыдущий перед 10)
  TEST_ASSERT(getCurrent(ring)->data == 20,
              "Неверный переход назад (10 -> 20)");

  prev(ring);  // 20 -> 30
  TEST_ASSERT(getCurrent(ring)->data == 30,
              "Неверный переход назад (20 -> 30)");

  prev(ring);  // 30 -> 10
  TEST_ASSERT(getCurrent(ring)->data == 10,
              "Кольцо не замкнулось при обратном обходе (30 -> 10)");

  destroyRing(ring);
  TEST_PASS();
  return 1;
}

int test_reverse() {
  TEST_START("Реверс кольца");

  Ring* ring = createRing();
  addElement(ring, 10);  // current = 10
  addElement(ring, 20);  // вставка после 10
  addElement(ring, 30);  // вставка после 10

  // До реверса: 10(current) -> 30 -> 20 -> 10
  TEST_ASSERT(getCurrent(ring)->data == 10,
              "Начальная позиция: current должен быть 10");

  reverse(ring);

  // После реверса: 10(current) -> 20 -> 30 -> 10
  TEST_ASSERT(getCurrent(ring)->data == 10,
              "Текущий элемент после реверса должен остаться 10");

  // Проверяем новый порядок next
  next(ring);  // 10 -> 20
  TEST_ASSERT(getCurrent(ring)->data == 20,
              "После реверса next(10) должен быть 20");

  next(ring);  // 20 -> 30
  TEST_ASSERT(getCurrent(ring)->data == 30,
              "После реверса next(20) должен быть 30");

  next(ring);  // 30 -> 10
  TEST_ASSERT(getCurrent(ring)->data == 10,
              "После реверса кольцо должно замкнуться");

  // Реверс пустого кольца
  Ring* empty = createRing();
  reverse(empty);  // Не должно падать
  destroyRing(empty);

  // Реверс кольца из одного элемента
  Ring* single = createRing();
  addElement(single, 100);
  reverse(single);
  TEST_ASSERT(getCurrent(single)->data == 100,
              "Реверс одного элемента не должен менять current");
  TEST_ASSERT(getSize(single) == 1, "Размер не должен измениться");
  destroyRing(single);

  destroyRing(ring);
  TEST_PASS();
  return 1;
}

int test_remove_element() {
  TEST_START(
      "Удаление элемента по значению (removeElement - первое вхождение)");

  Ring* ring = createRing();
  addElement(ring, 10);  // current = 10
  addElement(ring, 20);
  addElement(ring, 30);
  addElement(ring, 20);  // Дубликат

  // Порядок: 10(current) -> 20(новый) -> 30 -> 20(старый) -> 10

  // Удаляем существующий элемент (первое вхождение 20)
  int result = removeElement(ring, 20);
  TEST_ASSERT(result == 1, "Элемент должен быть найден и удален");
  TEST_ASSERT(getSize(ring) == 3, "Размер должен уменьшиться до 3");

  // Удаляем несуществующий элемент
  result = removeElement(ring, 99);
  TEST_ASSERT(result == 0, "Несуществующий элемент не должен быть найден");
  TEST_ASSERT(getSize(ring) == 3, "Размер не должен измениться");

  // Удаляем из пустого кольца
  Ring* empty = createRing();
  result = removeElement(empty, 10);
  TEST_ASSERT(result == 0, "Удаление из пустого кольца должно вернуть 0");
  destroyRing(empty);

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
  // Порядок: 10(current) -> 50 -> 40 -> 30 -> 20 -> 10
  TEST_ASSERT(getCurrent(ring)->data == 10,
              "Текущий элемент должен быть первым добавленным (10)");

  // Вращаем вперед на 2 шага: 10 -> 30 (через 50 и 40)
  rotate(ring, 2);
  TEST_ASSERT(getCurrent(ring)->data == 30,
              "Неверное вращение вперед (10 -> 30)");

  // Вращаем назад на 1 шаг: 30 -> 40
  rotate(ring, -1);
  TEST_ASSERT(getCurrent(ring)->data == 40,
              "Неверное вращение назад (30 -> 40)");

  // Вращение на 0 шагов
  rotate(ring, 0);
  TEST_ASSERT(getCurrent(ring)->data == 40,
              "Вращение на 0 шагов изменило позицию");

  destroyRing(ring);
  TEST_PASS();
  return 1;
}

int test_remove_each_element() {
  TEST_START("Удаление всех элементов по значению (removeEachElement)");

  Ring* ring = createRing();

  // Тест 1: Удаление всех вхождений значения
  addElement(ring, 10);  // current = 10
  addElement(ring, 20);
  addElement(ring, 10);  // дубликат
  addElement(ring, 30);
  addElement(ring, 10);  // дубликат
  addElement(ring, 40);

  // Порядок: 10(current) -> 40 -> 10(3й) -> 30 -> 10(2й) -> 20 -> 10
  TEST_ASSERT(getSize(ring) == 6, "Неверный начальный размер");

  int removed = removeEachElement(ring, 10);
  TEST_ASSERT(removed == 3, "Должно быть удалено 3 элемента");
  TEST_ASSERT(getSize(ring) == 3, "Размер должен уменьшиться до 3");

  // После удаления current должен стать 40 (следующий после первого удаленного
  // 10)
  TEST_ASSERT(getCurrent(ring)->data == 40, "Current должен быть 40");

  // Тест 5: Удаление, когда current - один из удаляемых элементов
  destroyRing(ring);
  ring = createRing();
  addElement(ring, 1);
  addElement(ring, 2);
  addElement(ring, 1);  // current останется на первом 1
  addElement(ring, 3);

  // Сначала проверяем, что current = 1
  TEST_ASSERT(getCurrent(ring)->data == 1, "Current должен быть 1");

  removed = removeEachElement(ring, 1);
  TEST_ASSERT(removed == 2, "Должно быть удалено 2 элемента");
  TEST_ASSERT(getSize(ring) == 2, "Размер должен быть 2");
  // После удаления current должен перейти к следующему элементу (2)
  TEST_ASSERT(getCurrent(ring)->data == 2, "Current должен перейти к 2");

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
  // Порядок: 1(current) -> 5 -> 4 -> 3 -> 2 -> 1

  // Тест суммирования
  reset_sum();
  applyFunction(ring, sum_node_data);
  TEST_ASSERT(test_sum == 15, "Сумма элементов неверна (1+2+3+4+5=15)");

  // Тест применения к пустому кольцу
  Ring* empty = createRing();
  reset_sum();
  applyFunction(empty, sum_node_data);  // Не должно падать
  TEST_ASSERT(test_sum == 0,
              "Сумма должна остаться 0 при обходе пустого кольца");

  destroyRing(empty);
  destroyRing(ring);
  TEST_PASS();
  return 1;
}

int test_edge_cases() {
  TEST_START("Граничные случаи");

  // Работа с NULL
  Ring* ring = NULL;
  addElement(ring, 10);                // Не должно падать
  removeCurrent(ring);                 // Не должно падать
  next(ring);                          // Не должно падать
  prev(ring);                          // Не должно падать
  getSize(ring);                       // Не должно падать
  isEmpty(ring);                       // Не должно падать
  rotate(ring, 5);                     // Не должно падать
  reverse(ring);                       // Не должно падать
  removeElement(ring, 10);             // Не должно падать
  removeEachElement(ring, 10);         // Не должно падать
  applyFunction(ring, sum_node_data);  // Не должно падать

  // Создание и немедленное удаление
  ring = createRing();
  destroyRing(ring);

  TEST_PASS();
  return 1;
}

int test_add_element_logic() {
  TEST_START("Проверка логики добавления элементов");

  Ring* ring = createRing();

  // Добавляем первый элемент
  addElement(ring, 100);
  TEST_ASSERT(getCurrent(ring)->data == 100,
              "Первый элемент должен быть текущим");
  TEST_ASSERT(ring->current->next->data == 100,
              "Next должен указывать на себя");
  TEST_ASSERT(ring->current->prev->data == 100,
              "Prev должен указывать на себя");

  // Добавляем второй элемент
  addElement(ring, 200);
  // Порядок: 100(current) -> 200 -> 100
  TEST_ASSERT(getCurrent(ring)->data == 100,
              "Текущий элемент должен остаться 100");
  TEST_ASSERT(ring->current->next->data == 200,
              "Next от 100 должен указывать на 200");
  TEST_ASSERT(ring->current->prev->data == 200,
              "Prev от 100 должен указывать на 200");

  // Добавляем третий элемент
  addElement(ring, 300);
  // Порядок: 100(current) -> 300 -> 200 -> 100
  TEST_ASSERT(getCurrent(ring)->data == 100,
              "Текущий элемент должен остаться 100");
  TEST_ASSERT(ring->current->next->data == 300,
              "Next от 100 должен указывать на 300");
  TEST_ASSERT(ring->current->prev->data == 200,
              "Prev от 100 должен указывать на 200");

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
  test_remove_each_element();
  test_apply_function();
  test_edge_cases();
  test_add_element_logic();

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
