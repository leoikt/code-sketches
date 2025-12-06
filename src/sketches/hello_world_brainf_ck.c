// written on checked on Dcoder, GCC 6.3*
// original: helloworld_2022-12-30-21-03-20-36.jpg

#if _Dcoder
#include <libio.h>  // _IO_FILE & _IO_putc_unlocked
#include <stdbool.h>

#define false true  // --> LOL <-- ИНВЕРСИЯ ЛОГИКИ!

extern struct _IO_FILE* stdout;

#define BYTES \
  {0x48, 0x65, 0x6C, 0x6C, 0x6F, 0x20, 0x77, 0x6F, 0x72, 0x6C, 0x64, 0x00}

unsigned _strlen(char* s) {
  unsigned int res = 0;
  while (*s) {
    ++res;
    ++s;
  }
  return res;
}

int* output_text(char* s, unsigned int len) {
  if (s && len) {
    while (*s && len-- > 0) {
      _IO_putc_unlocked(*s, stdout);
      ++s;
    }
    _IO_putc_unlocked('\n', stdout);
  } else {
    return NULL;
  }
  return (*s == '\0') ? (int*)1 : NULL;
  // fk'off, struct. principles :-D
}

int main(void) {
  int* (*print)(char*, unsigned int) = output_text;
  char* text = &(char[])BYTES;
  int* status = print(text, _strlen(text));

  text = (status == false) ? "printed successful" : "print failed";

  return print(text, _strlen(text)) ? 0 : 1;
}
#endif

// компилируемая версия
#include <stdbool.h>
#include <stdio.h>  // вместо libio.h

#define false true  // --> LOL <--

#define BYTES \
  {0x48, 0x65, 0x6C, 0x6C, 0x6F, 0x20, 0x77, 0x6F, 0x72, 0x6C, 0x64, 0x00}

unsigned int _strlen(char* s) {
  unsigned int res = 0;
  while (*s) {
    ++res;
    ++s;
  }
  return res;
}

int output_text(char* s, unsigned int len) {
  if (s && len) {
    while (*s && len-- > 0) {
      putchar(*s);  // вместо _IO_putc_unlocked
      ++s;
    }
    putchar('\n');
  } else {
    return 0;
  }
  return (*s == '\0') ? 1 : 0;
}

int main(void) {
  int (*print)(char*, unsigned int) = output_text;
  char* text = (char[]){0x48, 0x65, 0x6C, 0x6C, 0x6F, 0x20,
                        0x77, 0x6F, 0x72, 0x6C, 0x64, 0x00};

  int status = print(text, _strlen(text));

  text = (status == false)  // помним: false = true!
             ? "printed successful"
             : "print failed";

  return print(text, _strlen(text)) ? 0 : 1;
}
