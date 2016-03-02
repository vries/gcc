#include <stdlib.h>

int z = 1;

int
main (void)
{
  unsigned int a[1000];
  for (unsigned i = 0; i < 1000; ++i)
    a[i] = 0;

#pragma acc kernels copyout (a)
  {
    z = 2;
#pragma acc loop independent gang
    for (unsigned i = 0; i < 1000; ++i)
      a[i] = 1;
  }

  for (unsigned i = 0; i < 1000; ++i)
    if (a[i] != 1)
      abort ();
}
