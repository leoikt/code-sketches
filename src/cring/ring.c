#include "ring.h"

#include <stdio.h>
#include <stdlib.h>

Ring* createRing() {
  Ring* tmp = (Ring*)malloc(sizeof(Ring));
  if (!tmp) return NULL;

  tmp->size = 0;
  tmp->current = NULL;
  return tmp;
}

void destroyRing(Ring* ring) {
  if (!ring) return;

  while (!isEmpty(ring)) {
    removeCurrent(ring);
  }
  free(ring);
}

void addElement(Ring* ring, int value) {
  if (!ring) return;

  Node* newNode = (Node*)malloc(sizeof(Node));
  if (!newNode) return;

  newNode->data = value;

  if (ring->current == NULL) {
    ring->current = newNode;
    newNode->next = newNode->prev = newNode;
  } else {
    Node* prevNode = ring->current->prev;

    newNode->next = ring->current;
    newNode->prev = prevNode;
    prevNode->next = newNode;
    ring->current->prev = newNode;
  }

  ++ring->size;
}

int removeCurrent(Ring* ring) {
  if (!ring || ring->current == NULL) {
    fprintf(stderr, "Ring is empty\n");
    return -1;
  }

  Node* target = ring->current;
  int retVal = target->data;

  if (ring->size == 1) {
    ring->current = NULL;
  } else {
    target->prev->next = target->next;
    target->next->prev = target->prev;
    ring->current = target->next;
  }

  free(target);
  --ring->size;

  if (ring->size == 0) {
    ring->current = NULL;
  }

  return retVal;
}

int removeElement(Ring* ring, int value) {
  if (!ring || isEmpty(ring)) return 0;

  Node* start = ring->current;
  Node* current = start;
  int removed = 0;

  do {
    if (current->data == value) {
      Node* toRemove = current;
      current = current->next;

      if (toRemove == ring->current) {
        removeCurrent(ring);
      } else {
        toRemove->prev->next = toRemove->next;
        toRemove->next->prev = toRemove->prev;
        free(toRemove);
        --ring->size;
      }

      removed = 1;
      break;
    } else {
      current = current->next;
    }
  } while (current != start);

  return removed;
}

Node* next(Ring* ring) {
  if (!ring || !ring->current) return NULL;

  ring->current = ring->current->next;
  return ring->current;
}

Node* prev(Ring* ring) {
  if (!ring || !ring->current) return NULL;

  ring->current = ring->current->prev;
  return ring->current;
}

Node* getCurrent(Ring* ring) { return ring ? ring->current : NULL; }

size_t getSize(Ring* ring) { return ring ? ring->size : 0; }

int isEmpty(Ring* ring) { return !ring || ring->size == 0; }

void rotate(Ring* ring, int steps) {
  if (!ring || isEmpty(ring)) return;

  if (steps > 0) {
    for (int i = 0; i < steps; i++) {
      ring->current = ring->current->next;
    }
  } else if (steps < 0) {
    for (int i = 0; i < -steps; i++) {
      ring->current = ring->current->prev;
    }
  }
}

void reverse(Ring* ring) {
  if (!ring || ring->size < 2) return;

  Node* current = ring->current;
  Node* temp = NULL;

  do {
    temp = current->prev;
    current->prev = current->next;
    current->next = temp;
    current = current->prev;
  } while (current != ring->current);

  ring->current = ring->current->next;
}

void printRing(Ring* ring) {
  if (!ring || isEmpty(ring)) {
    printf("Ring is empty\n");
    return;
  }

  Node* start = ring->current;
  Node* current = start;

  printf("Ring (size=%zu): ", ring->size);
  do {
    printf("%d ", current->data);
    current = current->next;
  } while (current != start);
  printf("\n");
}

void applyFunction(Ring* ring, void (*f)(Node*)) {
  if (!ring || isEmpty(ring) || !f) return;

  Node* start = ring->current;
  Node* current = start;

  do {
    f(current);
    current = current->next;
  } while (current != start);
}

/* Вспомогательная функция для примера */
void printNode(Node* node) {
  if (node) {
    printf("%d ", node->data);
  }
}
