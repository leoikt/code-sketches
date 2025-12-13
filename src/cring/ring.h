#ifndef RING_H
#define RING_H

#include <stddef.h>

typedef struct _Node {
  int data;
  struct _Node* next;
  struct _Node* prev;
} Node;

typedef struct _Ring {
  size_t size;
  Node* current;
} Ring;

/* Создание и уничтожение */
Ring* createRing();
void destroyRing(Ring* ring);

/* Вставка и удаление */
void addElement(Ring* ring, int value);
int removeCurrent(Ring* ring);
int removeElement(Ring* ring, int value);

/* Навигация */
Node* next(Ring* ring);
Node* prev(Ring* ring);
Node* getCurrent(Ring* ring);

/* Информация */
size_t getSize(Ring* ring);
int isEmpty(Ring* ring);

/* Операции с кольцом */
void rotate(Ring* ring, int steps);
void reverse(Ring* ring);
void printRing(Ring* ring);
void applyFunction(Ring* ring, void (*f)(Node*));

#endif
