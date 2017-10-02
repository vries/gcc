/* { dg-do compile } */
/* { dg-additional-options "-ffat-lto-objects" } */

int
f (void)
{
  register unsigned int r asm ("no-such-register"); /* { dg-error "invalid register name" } */
  return r;
}
