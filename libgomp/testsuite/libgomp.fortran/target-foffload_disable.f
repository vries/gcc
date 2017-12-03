!     { dg-additional-options "-foffload=disable" }

      PROGRAM MAIN
      IMPLICIT NONE

      INCLUDE "omp_lib.h"

      IF (.NOT. OMP_IS_INITIAL_DEVICE()) CALL ABORT
!$OMP TARGET
      IF (.NOT. OMP_IS_INITIAL_DEVICE()) CALL ABORT
!$OMP END TARGET
      IF (.NOT. OMP_IS_INITIAL_DEVICE()) CALL ABORT

      END
