/* { dg-additional-options "-foffload=disable" } */

#include <omp.h>

int main()
{
  if (!omp_is_initial_device())
    __builtin_abort();
#pragma omp target
  {
    if (!omp_is_initial_device())
      __builtin_abort();
  }
  if (!omp_is_initial_device())
    __builtin_abort();

  return 0;
}
