// src: https://stepik.org/lesson/1443628/step/12?unit=1462429

#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

// The following functions are available to you:
size_t read_size();
int64_t* array_int_max(int64_t* array, size_t size);
int64_t** marray_read(size_t* rows, size_t* sizes[]);
void marray_free(int64_t** marray, size_t rows);
void marray_print(int64_t** marray, size_t* sizes, size_t rows);

// Pointer to the maximum of two numbers.
// If at least one number is NULL, then return the second.
// If both numbers are NULL, the result is NULL
int64_t* int64_ptr_max(int64_t* x, int64_t* y) {
  if (!x) return y;
  if (!y) return x;
  return *x > *y ? x : y;
}

// Return the address of the maximum element of an array of arrays
int64_t* marray_int_max(int64_t** marray, size_t* sizes, size_t rows) {
  if (!marray || !sizes || rows == 0) return NULL;

  int64_t* max = *marray;
  for (size_t i = 0; i < rows; i++) {
    if (sizes[i] == 0 || !marray[i]) continue;
    for (size_t j = 0; j < sizes[i]; j++) {
      max = int64_ptr_max(max, &marray[i][j]);
    }
  }
  return max;
}

// Subtract m from all the array elements
void marray_normalize(int64_t** marray, size_t sizes[], size_t rows,
                      int64_t m) {
  if (!marray || !sizes) return;

  for (size_t i = 0; i < rows; i++) {
    if (!marray[i] || sizes[i] == 0) continue;
    for (size_t j = 0; j < sizes[i]; j++) {
      marray[i][j] -= m;
    }
  }
}

// Read, find the maximum and normalize the array, print the result
void perform() {
  size_t rows = 0, *szs = NULL;
  int64_t** a = marray_read(&rows, &szs);

  if (!a && rows > 0) {
    fprintf(stderr, "memory allocation failed\n");
    if (szs) free(szs);
    return;
  }

  int64_t* p_max = marray_int_max(a, szs, rows);

  if (p_max) {
    int64_t m = *p_max;
    marray_normalize(a, szs, rows, m);
  }

  marray_print(a, szs, rows);
  marray_free(a, rows);
  free(szs);
}
