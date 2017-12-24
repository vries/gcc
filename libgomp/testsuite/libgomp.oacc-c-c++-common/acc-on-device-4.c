/* { dg-do compile } */
/* { dg-options "-O0" } */
/* { dg-additional-options "-fdump-tree-oaccdevlow" } */

#include <openacc.h>

extern void bar ();

void
foo (void)
{
  if (!acc_on_device (acc_device_host))
    bar ();
}

/* { dg-final { scan-tree-dump-not "acc_on_device" "oaccdevlow" } } */
/* { dg-final { scan-tree-dump-not "bar" "oaccdevlow" } } */
/* { dg-final { scan-tree-dump "_\[0-9\] = 1" "oaccdevlow" } } */
