/* { dg-additional-options "-O2" } */
/* { dg-additional-options "-fdump-tree-ealias-all" } */

#define N 2

void
foo (unsigned int *a, unsigned int *b, unsigned int *c, unsigned int *d)
{

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
/* { dg-final { scan-tree-dump-times "clique 1 base 6" 1 "ealias" } } */
/* { dg-final { scan-tree-dump-times "clique 1 base 7" 1 "ealias" } } */
/* { dg-final { scan-tree-dump-times "clique 1 base 8" 1 "ealias" } } */
/* { dg-final { scan-tree-dump-times "clique 1 base 9" 1 "ealias" } } */
/* { dg-final { scan-tree-dump-times "(?n)clique .* base .*" 12 "ealias" } } */

