/* usage:
 * 1. $ make
 * 2. $ gcc -Wall -Wextra -Werror -std=c17 -o example example.c -L. -lring
 * 3. $ ./example
 */
#include <stdio.h>

#include "ring.h"

/* Собственная функция для печати узла */
void printElement(Node* node) {
  if (node) {
    printf("%d ", node->data);
  }
}

int main() {
  Ring* ring = createRing();

  // Добавляем элементы
  for (int i = 1; i <= 5; i++) {
    addElement(ring, i * 10);
  }

  printf("Initial ring:\n");
  printRing(ring);

  printf("\nRotating forward 2 steps:\n");
  rotate(ring, 2);
  printRing(ring);

  printf("\nRemoving current element (%d):\n", getCurrent(ring)->data);
  removeCurrent(ring);
  printRing(ring);

  printf("\nReversing ring:\n");
  reverse(ring);
  printRing(ring);

  printf("\nApplying function to each element:\n");
  applyFunction(ring, printElement);  // Используем нашу функцию
  printf("\n");

  // Тестируем removeEachElement
  printf("\nAdding duplicate elements:\n");
  addElement(ring, 20);
  addElement(ring, 20);
  addElement(ring, 30);
  printRing(ring);

  printf("\nRemoving all elements with value 20:\n");
  int removed = removeEachElement(ring, 20);
  printf("Removed %d elements\n", removed);
  printRing(ring);

  printf("\nTesting navigation:\n");
  printf("Current: %d\n", getCurrent(ring)->data);
  next(ring);
  printf("After next: %d\n", getCurrent(ring)->data);
  prev(ring);
  printf("After prev: %d\n", getCurrent(ring)->data);

  destroyRing(ring);
  printf("\nRing destroyed successfully.\n");
  return 0;
}
