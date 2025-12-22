#include "ring.h"

#include <stdio.h>

int main() {
  Ring* ring = createRing();

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
  applyFunction(ring, printNode);
  printf("\n");

  destroyRing(ring);
  return 0;
}
