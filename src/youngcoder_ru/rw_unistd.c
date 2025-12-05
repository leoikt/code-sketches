/* src: https://stepik.org/lesson/53872/step/10?unit=32085 */

#include <unistd.h>

#define STDIN 0
#define STDOUT 1
#define STDERR 2

int main() {
  char byte = 0;
  read(STDIN, &byte, sizeof(byte));  // читаем 1 байт из stdin в адрес byte;
  byte ^= 1;  // Инвертируем: '0'(ascii=48) -> '1'(ascii=49), '1'(49) -> '0'(48)
  write(STDOUT, &byte, sizeof(byte));  // пишем в stdout наш byte

  return 0;
}
