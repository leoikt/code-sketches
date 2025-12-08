/* src: https://stepik.org/lesson/1443625/step/16?unit=1462426 */

#include <assert.h>
#include <stdio.h>
#include <string.h>

int is_whitespace(char c) { return c == ' ' || c == '\t' || c == '\n'; }

int string_count(char* str) {
  int res = 0;
  if (str)
    while (str[res]) res++;
  return res;
}

int isWord(char y, char z) { return !is_whitespace(y) && !is_whitespace(z); }

int string_words(char* str) {
  int res = 0, pos = string_count(str);
  if (pos > 1) {
    int i = 2;
    while (i < pos) {
      while (str[i] && !is_whitespace(str[i])) i++;
      res += isWord(str[i - 1], str[i - 2]);
      i++;
    }
    if (!res) res = isWord(str[1], str[0]);
  }
  return res;
}

// Эталонная функция для сравнения
int correct_string_words(char* str) {
  int words = 0;
  int length = 0;
  int i = 0;

  while (str[i]) {
    if (is_whitespace(str[i])) {
      if (length > 1) words++;
      length = 0;
    } else {
      length++;
    }
    i++;
  }

  if (length > 1) words++;
  return words;
}

// Функция для запуска тестов
void run_test(char* str, char* test_name) {
  int result = string_words(str);
  int expected = correct_string_words(str);

  printf("Тест: %-30s | Строка: \"%s\"\n", test_name, str);
  printf("  Ожидается: %d, Получено: %d - ", expected, result);

  if (result == expected) {
    printf("✓ ПРОЙДЕН\n");
  } else {
    printf("✗ ПРОВАЛЕН\n");
  }
}

int main() {
  printf("=== ТЕСТИРОВАНИЕ string_words ===\n\n");

  // 1. БАЗОВЫЕ ТЕСТЫ
  printf("1. Базовые тесты:\n");
  run_test("hello world", "Два обычных слова");
  run_test("a b c", "Три коротких слова");
  run_test("ab cd ef", "Три слова по 2 буквы");
  run_test("abc", "Одно длинное слово");
  run_test("ab", "Одно слово из 2 букв");
  run_test("a", "Одна буква");
  run_test("", "Пустая строка");
  run_test("   ", "Только пробелы");

  // 2. ПРОБЕЛЫ В НАЧАЛЕ И КОНЦЕ
  printf("\n2. Пробелы в начале и конце:\n");
  run_test("  hello world  ", "Пробелы вокруг");
  run_test("  ab  ", "Пробелы вокруг слова из 2 букв");
  run_test("  a  ", "Пробелы вокруг одной буквы");
  run_test("   abc   def   ", "Несколько пробелов");
  run_test(" \t \n  test  \n\t ", "Разные пробельные символы вокруг");

  // 3. МНОЖЕСТВЕННЫЕ ПРОБЕЛЫ
  printf("\n3. Множественные пробелы между словами:\n");
  run_test("hello     world", "Много пробелов между");
  run_test("a    b    c", "Много пробелов между короткими");
  run_test("ab    cd    ef", "Много пробелов между словами из 2 букв");
  run_test("word1  word2   word3    word4", "Разное количество пробелов");

  // 4. РАЗНЫЕ ПРОБЕЛЬНЫЕ СИМВОЛЫ
  printf("\n4. Разные пробельные символы:\n");
  run_test("hello\tworld", "Таб между словами");
  run_test("hello\nworld", "Новая строка между словами");
  run_test("hello \t \n world", "Смесь пробельных символов");
  run_test("a\tb\nc d", "Разные разделители");
  run_test("ab\tcd\nef", "Табы и переводы строк");

  // 5. ГРАНИЧНЫЕ СЛУЧАИ С ДЛИНОЙ СЛОВ
  printf("\n5. Граничные случаи с длиной слов:\n");
  run_test("aa bb cc", "Слова ровно по 2 буквы");
  run_test("aaa bbb ccc", "Слова ровно по 3 буквы");
  run_test("a aa aaa", "Слова разной длины");
  run_test("ab abc abcd abcde", "Возрастающая длина");
  run_test("verylongword short", "Очень длинное + короткое");

  // 6. СЛОЖНЫЕ КОМБИНАЦИИ
  printf("\n6. Сложные комбинации:\n");
  run_test("  a b  cd   efg    hij    ", "Смесь разных длин и пробелов");
  run_test("\t\n \t\nhello\t\n \tworld\t\n ", "Пробелы и слова вперемешку");
  run_test("word", "Одно слово без пробелов");
  run_test("w", "Одна буква без пробелов");
  run_test("  w  ", "Одна буква с пробелами");

  // 7. СПЕЦИАЛЬНЫЕ КЕЙСЫ (где твой алгоритм может ошибаться)
  printf("\n7. Специальные кейсы (потенциальные проблемы):\n");
  run_test("ab  ", "Слово + пробелы в конце");
  run_test("  ab", "Пробелы + слово в начале");
  run_test("  ab  ", "Пробелы с обеих сторон");
  run_test("a  b", "Короткие слова с несколькими пробелами");
  run_test("abcde", "Очень длинное одно слово");
  run_test("abc def  ghi   jkl", "Разное количество пробелов");
  run_test("", "Ещё раз пустая строка");
  run_test(" ", "Один пробел");
  run_test("  ", "Два пробела");
  run_test("\t", "Один таб");
  run_test("\n", "Одна новая строка");

  // 8. СТРОКИ С РАЗНЫМИ СИМВОЛАМИ
  printf("\n8. Строки с разными символами:\n");
  run_test("123 4567 89", "Цифры");
  run_test("!! ?? ...", "Знаки препинания");
  run_test("c++ python java", "Слова с символами");
  run_test("test-case example.com", "Дефисы и точки");

  // 9. ТЕСТЫ НА ПРОИЗВОДИТЕЛЬНОСТЬ (длинные строки)
  printf("\n9. Длинные строки:\n");
  char long_str1[1000] = "word";
  for (int i = 0; i < 50; i++) strcat(long_str1, " longword");
  run_test(long_str1, "Длинная строка с длинными словами");

  char long_str2[1000] = "";
  for (int i = 0; i < 100; i++) strcat(long_str2, "a ");
  run_test(long_str2, "Много коротких слов (все по 1 букве)");

  char long_str3[1000] = "ab";
  for (int i = 0; i < 50; i++) strcat(long_str3, " cd");
  run_test(long_str3, "Много слов по 2 буквы");

  // 10. ТЕСТ С НУЛЕВЫМ УКАЗАТЕЛЕМ
  printf("\n10. Крайние случаи:\n");
  // Внимание: твоя функция проверяет str в string_count, но не в string_words
  // Добавим проверку в main
  if (string_words(NULL) == 0) {
    printf("Тест: NULL указатель                     | Строка: (null)\n");
    printf("  Ожидается: 0, Получено: %d - ✓ ПРОЙДЕН\n", string_words(NULL));
  } else {
    printf("Тест: NULL указатель                     | Строка: (null)\n");
    printf("  Ожидается: 0, Получено: %d - ✗ ПРОВАЛЕН\n", string_words(NULL));
  }

  // Автоматическая проверка всех тестов
  printf("\n=== ИТОГИ ===\n");

  // Создадим массив всех тестовых строк
  char* all_tests[] = {
      // Базовые
      "hello world", "a b c", "ab cd ef", "abc", "ab", "a", "", "   ",
      // Пробелы вокруг
      "  hello world  ", "  ab  ", "  a  ", "   abc   def   ",
      " \t \n  test  \n\t ",
      // Множественные пробелы
      "hello     world", "a    b    c", "ab    cd    ef",
      "word1  word2   word3    word4",
      // Разные пробельные символы
      "hello\tworld", "hello\nworld", "hello \t \n world", "a\tb\nc d",
      "ab\tcd\nef",
      // Граничные случаи
      "aa bb cc", "aaa bbb ccc", "a aa aaa", "ab abc abcd abcde",
      "verylongword short",
      // Сложные комбинации
      "  a b  cd   efg    hij    ", "\t\n \t\nhello\t\n \tworld\t\n ", "word",
      "w", "  w  ",
      // Специальные кейсы
      "ab  ", "  ab", "  ab  ", "a  b", "abcde", "abc def  ghi   jkl", "", " ",
      "  ", "\t", "\n",
      // Разные символы
      "123 4567 89", "!! ?? ...", "c++ python java", "test-case example.com"};

  int total_tests = sizeof(all_tests) / sizeof(all_tests[0]);
  int passed = 0;
  int failed = 0;

  for (int i = 0; i < total_tests; i++) {
    int result = string_words(all_tests[i]);
    int expected = correct_string_words(all_tests[i]);

    if (result == expected) {
      passed++;
    } else {
      failed++;
      printf("✗ Провален тест: \"%s\"\n", all_tests[i]);
      printf("  Ожидалось: %d, Получено: %d\n", expected, result);
    }
  }

  // Проверка NULL
  if (string_words(NULL) == 0)
    passed++;
  else
    failed++;

  printf("\nВсего тестов: %d\n", total_tests + 1);
  printf("Пройдено: %d\n", passed);
  printf("Провалено: %d\n", failed);

  if (failed == 0) {
    printf("\n✅ ВСЕ ТЕСТЫ ПРОЙДЕНЫ УСПЕШНО!\n");
  } else {
    printf("\n❌ ЕСТЬ ПРОБЛЕМЫ!\n");
  }

  return 0;
}
