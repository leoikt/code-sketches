// src: https://stepik.org/lesson/1443621/step/9?thread=solutions&unit=1462422
/* Rewrite the code in the assignment so that it compiles. In tests, two
 * non-negative numbers are entered: the first is checked using is_single_digit,
 * the second using is_triple_digit.
 */

/*
// Returns 1 if the number contains one digit, otherwise 0
int is_single_digit(int n) {
 if (n < 0) {
   return 0;
 }
 if (n > 9) {
   return 0;
 }
 if (n <= 9 && n >= 0) {
   return 1;
 }
}

// Returns 1 if the number contains exactly three digits, otherwise 0
int is_triple_digit(int n) {
 if (n < 10) {
   return 0;
 } else {
   if (n > 999) {
     return 0;
   } else {
     if (n <= 999 && n >= 100) {
       return 1;
     }
   }
 }
}
 */

/* Не знаю, зачем автор курса учит множественным точкам выхода и спагетти-коду,
 * но сделаем лучше */

// Returns 1 if the number contains one digit, otherwise 0
int is_single_digit(int n) { return (unsigned)(n + 9) <= 18; }

// Returns 1 if the number contains exactly three digits, otherwise 0
int is_triple_digit(int n) {
  unsigned u = n < 0 ? -n : n;  // abs(n)
  return (u - 100) < 900;
}
