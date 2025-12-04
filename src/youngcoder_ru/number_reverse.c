/* https://stepik.org/lesson/53872/step/8?unit=32085 */
/*

Реверс
Определить число, полученное выписыванием в обратном порядке цифр заданного
целого трехзначного числа. Вывод начинается с первой ненулевой цифры.
stdin: Натуральное трёхзначное число KK
Выходные данные:
Число KK, записанное в обратном порядке без нулей в начале
*/

// then
#include <stdio.h>
#define L 8
#define _isDig(a) ((a) >= '0' && (a) <= '9' ? (a) : (0))

unsigned getEndianZeros(const char* s) {
  unsigned res = 0, break_f = 0;
  for (char i = L - 1; i >= 0 && !break_f; --i)
    if (_isDig(s[i]) == '0' || _isDig(s[i]) == 0)
      ++res;
    else
      break_f = 1;
  return res;
}

int main() {
  char s[L] = {0};
  if (scanf("%Ls", &s) == 1) {
    // %{N}s - {N} must be < L
    unsigned z = getEndianZeros(&s);
    for (int i = (L - z - 1); i > -1; --i)
      if (_isDig(s[i])) putchar(s[i]);
  }
}

// now
/*
#include <stdio.h>
#include <string.h>

int main() {
  char s[32];

  if (scanf("%31s", s) == 1) {
    int end = strlen(s) - 1;

    while (end > 0 && s[end] == '0') end--;

    for (int i = end; i >= 0; i--) {
      putchar(s[i]);
    }
  }

  return 0;
}
*/
