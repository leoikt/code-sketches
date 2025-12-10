/* src: https://stepik.org/lesson/1443625/step/12?unit=1462426 */

#include <limits.h>

void array_fib(int* a, int* l) {
  if (!a || !l || a >= l) return;  // Защита от ошибок
  if (a < l) *a++ = 1;             // Первый элемент
  if (a < l) *a++ = 1;             // Второй элемент
  while (a < l) {                  // Остальные
    int x = a[-1], y = a[-2];      // Два предыдущих
    *a++ = (x <= INT_MAX - y - 1) ? x + y + 1 : INT_MAX;
  }
}
