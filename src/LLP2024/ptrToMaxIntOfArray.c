// src: https://stepik.org/lesson/1443628/step/8?unit=1462429

int64_t* array_int_max(int64_t* array, size_t size) {
  if (!array || !size) return NULL;
  int64_t* max = array;
  for (size_t i = 1; i < size; i++) {
    max = *max < array[i] ? array + i : max;
  }
  return max;
}
