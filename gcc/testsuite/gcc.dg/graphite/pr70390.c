/* { dg-do compile } */
/* { dg-options "-O3 -floop-interchange" } */

int a, c;
int *b;
short d, f;
static int e;
long g;
void
fn1 ()
{
  a = 4;
  for (; a >= 0; a--)
    ;
  for (; c;)
    for (; f <= 4; f++)
      {
        g &= a;
        d = 0;
        for (; d <= 4; d++)
          e |= b[d];
      }
}
