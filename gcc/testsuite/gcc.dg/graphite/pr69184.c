/* { dg-do compile } */
/* { dg-options "-O3 -floop-interchange -w" } */

int a, b, c, e, f, g;
int d[1];
static int *h = &c;
long i;

int
fn1 (short p1)
{
  return p1 + a;
}

void
fn2 ()
{
  for (; f; f++)
    {
      int *j = &g;
      e = 1;
      for (; e; e++)
        {
          i = b ?: b;
          *j ^= fn1 (d[e]);
          if (*h)
            break;
        }
    }
}
