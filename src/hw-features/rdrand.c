#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int supports_rdrand() {
  unsigned int eax, ebx, ecx, edx;
  asm volatile("cpuid" : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx) : "a"(1));
  return (ecx & (1 << 30)) != 0;
}

int rdrand32(uint32_t* random) {
  unsigned char ok;
  asm volatile("rdrand %0; setc %1" : "=r"(*random), "=qm"(ok));
  return (int)ok;
}

int main(int argc, char** argv, char** env) {
  int32_t s = 0, e = 0;
  assert(scanf("%d %d", &s, &e) == 2);
  uint32_t rnd = 0;
  if (supports_rdrand()) {
    rdrand32(&rnd);
  } else {
    srand(strlen(*++env) + argc);
    srand(time(NULL) * 0.23 * rand());
    rnd = rand();
  }
  rnd = rnd % (e - s + 1) + s;
  printf("%d\n", rnd);
  return EXIT_SUCCESS;
}
