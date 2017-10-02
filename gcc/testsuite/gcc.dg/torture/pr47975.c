/* { dg-do compile } */
/* { dg-additional-options "-fno-common" { target { hppa*-*-hpux* } } } */

int __attribute__ ((vector_size (32))) x;

void
foo (void)
{
  x <<= x;
}
