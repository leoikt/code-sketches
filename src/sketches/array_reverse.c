/* src: https://stepik.org/lesson/1443625/step/11?unit=1462426 */
#define SWAP(a, b)            \
  do {                        \
    __typeof__(a) temp = (a); \
    (a) = (b);                \
    (b) = temp;               \
  } while (0)

// fast solution without AVX
void array_reverse(int* array, int size) {
  if (!array || size <= 1) return;

  int* start = array;
  int* end = array + size - 1;

  while (start < end) {
    SWAP(*start, *end);
    start++;
    end--;
  }
}

void array_reverse_ptr(int* array, int* limit) {
  if (array < limit) {
    int size = limit - array;
    array_reverse(array, size);
  }
}
