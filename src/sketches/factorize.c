/* src: https://stepik.org/lesson/1443625/step/7?unit=1462426 */

unsigned isPrime_MND(unsigned n) {
  unsigned div = 2, mnd = 0;
  while (div * div <= n && !mnd) {
    if (n % div == 0) mnd = div;
    div++;
  }
  return mnd;  // Для простых вернет 0, для составных - минимальный нетривиалный
               // делитель
}

void factorize(int n, int* a, int* b) {
  if (n == 1) {
    *a = 0;
    *b = 0;
    return;
  }

  unsigned mnd = isPrime_MND(n);

  if (mnd == 0) {  // Число простое
    *a = 0;
    *b = n - 1;
  } else {  // Число составное
    *a = mnd - 1;
    *b = (n / mnd) - 1;
  }
}
