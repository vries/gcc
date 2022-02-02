/* As in spin-lock-global-2.c.  */
/* { dg-do run { target openacc_nvidia_accel_selected } } */

/* Also test without JIT optimization.  */
/* { dg-set-target-env-var GOMP_NVPTX_JIT "-O0" } */

#include "spin-lock-shared-2.c"
