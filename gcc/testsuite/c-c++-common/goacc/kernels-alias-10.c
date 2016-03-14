/* { dg-additional-options "-O2" } */
/* { dg-additional-options "-fdump-tree-ealias-all" } */

#define N 2

void
foo (void)
{
  unsigned int a[N];
  unsigned int b[N];
  unsigned int c[N];
  unsigned int d[N];

#pragma acc kernels copyin (a[0:N]) create (b[0:N]) copyout (c[0:N]) copy (d[0:N])
  {
    a[0] = 0;
    b[0] = 0;
    c[0] = 0;
    d[0] = 0;
  }
}

/* { dg-final { scan-tree-dump-times "clique 1 base 1" 4 "ealias" } } */
/* { dg-final { scan-tree-dump-times "clique 1 base 2" 1 "ealias" } } */
/* { dg-final { scan-tree-dump-times "clique 1 base 3" 1 "ealias" } } */
/* { dg-final { scan-tree-dump-times "clique 1 base 4" 1 "ealias" } } */
/* { dg-final { scan-tree-dump-times "clique 1 base 5" 1 "ealias" } } */
/* { dg-final { scan-tree-dump-times "(?n)clique .* base .*" 8 "ealias" } } */

