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
#define LOCKVAR1 lock_32_1
#define LOCKVAR2 lock_32_2
#define TESTS tests_32
#include "spin-lock-global.h"
#undef TYPE
#undef LOCKVAR1
#undef LOCKVAR2
#undef TESTS

#define TYPE unsigned long long int
#define LOCKVAR1 lock_64_1
#define LOCKVAR2 lock_64_2
#define TESTS tests_64
#include "spin-lock-global.h"
#undef TYPE
#undef LOCKVAR1
#undef LOCKVAR2
#undef TESTS

#define N (7 * 1000)

int
main (void)
{
  tests_32 (N);
  tests_64 (N);

  return 0;
}
