#include <stdlib.h>

int
main (void)
{
  int n[1];

  n[0] = 3;

#pragma omp target
  {
    static int test[4] = { 1, 2, 3, 4 };
    n[0] += test[n[0]];
  }

  if (n[0] != 7)
    abort ();

  return 0;
}
