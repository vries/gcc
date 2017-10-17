! { dg-no-torture-options }
      program condinc1
      logical l
      l = .false.
!$    include 'condinc1.inc'
      stop 2
      end
