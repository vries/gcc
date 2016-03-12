/* { dg-additional-options "-O2" } */
/* { dg-additional-options "-fdump-tree-ealias-all" } */

#define N 2

void __attribute__((noinline, noclone))
foo (unsigned int *a, unsigned int *b)
{

#pragma acc data copyout (a[0:N]) copyout (b[0:N])
  {
#pragma acc kernels present (a[0:N]) present (b[0:N])
    {
      a[0] = 0;
      b[0] = 0;
    }
  }
}

int
main (void)
{
  unsigned int a[2];
  unsigned int b[2];
  foo (&a[0], &b[0]);
  return 0;
}
