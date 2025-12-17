size_t read_size();
void array_int_fill(int64_t* array, size_t size);
int64_t* _array_int_read(size_t* size);

int64_t** marray_read(size_t* rows, size_t** sizes) {
  *rows = read_size();
  int64_t** a = malloc(*rows * sizeof(int64_t*));
  *sizes = malloc(*rows * sizeof(size_t));

  for (size_t i = 0; i < *rows; i++) {
    a[i] = _array_int_read(&((*sizes)[i]));
  }
  return a;
}

void marray_print(int64_t** marray, size_t* sizes, size_t rows) {
  for (size_t i = 0; i < rows; i = i + 1) {
    array_int_print(marray[i], sizes[i]);
    print_newline();
  }

  /*for (size_t i = 0; i < rows; i++) {
      free(marray[i]);
  }
  free(marray);
  free(sizes);*/
}
