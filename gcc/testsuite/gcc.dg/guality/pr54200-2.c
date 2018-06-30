/* { dg-do run } */
/* { dg-options "-g -fdebug-nops -fkeep-vars-live -DMAIN" } */

#include "prevent-optimization.h"

int o ATTRIBUTE_USED;

void
bar (void)
{
  o = 2;
}

int __attribute__((noinline,noclone))
foo (int z, int x, int b)
{
  if (x == 1)
    {
      bar ();
      return z;
    }
  else
    {
      int a = (x + z) + b;
      /* Add cast to prevent unsupported.  */
      return a; /* { dg-final { gdb-test . "(int)a" "6" } } */
    }
}

#ifdef MAIN
int main ()
{
  foo (3, 2, 1);
  return 0;
}
#endif
