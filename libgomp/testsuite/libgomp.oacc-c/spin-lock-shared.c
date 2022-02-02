#include <stdio.h>
#include <openacc.h>
#include <gomp-constants.h>
#include <assert.h>

enum memmodel
  {
    MEMMODEL_RELAXED = 0,
    MEMMODEL_ACQUIRE = 2,
    MEMMODEL_RELEASE = 3,
    MEMMODEL_SEQ_CST = 5,
  };

#define TYPE unsigned int
#define TESTS tests_32
#include "spin-lock-shared.h"
#undef TYPE
#undef TESTS

#define TYPE unsigned long long int
#define TESTS tests_64
#include "spin-lock-shared.h"
#undef TYPE
#undef TESTS

#define N (50 * 1000)

int
main (void)
{
  tests_32 (N);
  tests_64 (N);

  return 0;
}
