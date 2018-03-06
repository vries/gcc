/* { dg-additional-options "-ftree-switch-conversion" } */

#include <stdlib.h>

int
main (void)
{
  int n[1];

  n[0] = 4;

#pragma omp target
  {
    int a = n[0];

    switch (a & 3)
      {
      case 0: a = 4; break;
      case 1: a = 3; break;
      case 2: a = 2; break;
      default:
	a = 1; break;
      }

    n[0] = a;
  }

  if (n[0] != 4)
    abort ();

  return 0;
}
