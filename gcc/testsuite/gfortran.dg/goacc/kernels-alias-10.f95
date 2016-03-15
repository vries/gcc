! { dg-additional-options "-O2" }
! { dg-additional-options "-fdump-tree-ealias-all" }

program main
  implicit none
  integer, parameter :: n = 2
  integer, dimension (0:n-1) :: a, b, c, d

  !$acc kernels copyin (a(0:n-1)) create (b(0:n-1)) copyout (c(0:n-1)) copy (d(0:n-1))
  a(0) = 0
  b(0) = 0
  c(0) = 0
  d(0) = 0
  !$acc end kernels

end program main

! { dg-final { scan-tree-dump-times "clique 1 base 1" 4 "ealias" } }
! { dg-final { scan-tree-dump-times "clique 1 base 2" 1 "ealias" } }
! { dg-final { scan-tree-dump-times "clique 1 base 3" 1 "ealias" } }
! { dg-final { scan-tree-dump-times "clique 1 base 4" 1 "ealias" } }
! { dg-final { scan-tree-dump-times "clique 1 base 5" 1 "ealias" } }
! { dg-final { scan-tree-dump-times "clique 1 base 6" 1 "ealias" } }
! { dg-final { scan-tree-dump-times "clique 1 base 7" 1 "ealias" } }
! { dg-final { scan-tree-dump-times "clique 1 base 8" 1 "ealias" } }
! { dg-final { scan-tree-dump-times "clique 1 base 9" 1 "ealias" } }
! { dg-final { scan-tree-dump-times "(?n)clique .* base .*" 12 "ealias" } }
