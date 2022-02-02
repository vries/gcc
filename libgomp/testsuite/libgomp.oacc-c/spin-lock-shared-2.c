/* { dg-do run { target openacc_nvidia_accel_selected } } */

/* Define to 0 to have a regular spinlock.
   Makes the test-case invalid OpenACC: there's nothing that guarantees that
   the program will terminate.  So, we only do this for nvptx.  */
#define SPIN_CNT_MAX 0

#include "spin-lock-shared.c"
