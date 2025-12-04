/* src: https://stepik.org/lesson/1443624/step/4?unit=1462425 */

typedef unsigned long long ull_t;

int is_cube(ull_t n_ref) {
  int result = 0;

  if (n_ref < 2) {
    result = (n_ref == 0 || n_ref == 1);
  } else {
    ull_t cube = 0, l = 1, r = 2642245;

    // Продолжаем поиск, пока есть область поиска и не уверены, что нашли
    while (l <= r && result == 0) {
      ull_t m = l + (r - l) / 2;  // позиция поиска в диапазоне значений
      cube = m * m * m;

      if (cube == n_ref) {
        result = 1;  // Нашли точное совпадение, выходим из цикла
      } else if (cube < n_ref) {
        l = m + 1;
      } else {
        r = m - 1;
      }  // границы поиска
    }  // цикл
  }  // поиск куба

  return result;
}

int linear_is_cube(ull_t n_ref) {
  int result = 0;
  ull_t i = 0;

  while (i <= 2642245 && result == 0) {
    ull_t cube = i * i * i;

    if (cube == n_ref) {
      result = 1;
    } else if (cube > n_ref) {
      i = 2642246;  // Устанавливаем i за пределы, чтобы выйти из цикла
    } else {
      i++;
    }
  }

  return result;
}
