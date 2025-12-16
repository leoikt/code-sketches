// src: https://stepik.org/lesson/1443628/step/9?unit=1462429

int64_t read_int64() {
  int64_t res = 0;
  if (scanf("%" SCNd64, &res) != 1) fprintf(stderr, "input failure\n");
  return res;
}

size_t read_size() {
  size_t res = 0;
  if (scanf("%zu", &res) != 1)
    fprintf(stderr, "input failure for read_size()\n");
  return res;
}

// fill an already allocated array of size 'size' with numbers
void array_int_fill(int64_t* array, size_t size) {
  if (array && size)
    for (size_t i = 0; i < size; i++) {
      array[i] = read_int64();
    }
}

// Read the size of the array to * size, allocate memory for the array and fill
// it with numbers.
int64_t* _array_int_read(size_t* size) {
  *size = read_size();
  int64_t* res = NULL;

  if (*size && *size < SIZE_MAX / sizeof(int64_t)) {
    res = (int64_t*)malloc(*size * sizeof(int64_t));
    array_int_fill(res, *size);
  }

  return res;
}

int64_t* array_int_max(int64_t* array, size_t size) {
  if (!array || !size) return NULL;
  int64_t* max = array;
  for (size_t i = 1; i < size; i++) {
    max = *max < array[i] ? array + i : max;
  }
  return max;
}

void intptr_print(int64_t* x) {
  if (x == NULL) {
    printf("None");
  } else {
    printf("%" PRId64, *x);
  }
}

void perform() {
  size_t sz = 0;
  int64_t* a = _array_int_read(&sz);
  intptr_print(array_int_max(a, sz));
  free(a);
}
