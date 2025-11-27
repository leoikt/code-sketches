#include <stdio.h>
/* calcultaing of water weights  */

#define W_CUP 249.5f
// weigth of water in cup, g
#define W_DROP 0.05f
// weight of 1 water drop, g
#define W_MOL 3e-23F
// weight of 1 water molecule

#define drops_in_cup (W_CUP / W_DROP)
#define mols_in_drop (W_DROP / W_MOL)

typedef unsigned long long int ui64;
typedef unsigned int ui32;
typedef long double fp128;

int main() {
  ui32 n = 0;
  if (scanf("%u", &n) == 1) {
    ui64 totaldrops = n * drops_in_cup;
    fp128 totalmols = totaldrops * mols_in_drop;

    printf("%llu %.3Le", totaldrops, totalmols);

  } else {
    fputs("input failure", stderr);
  }
  return 0;
}
