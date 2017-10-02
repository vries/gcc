/* { dg-do compile } */
/* { dg-additional-options "-march=core-avx2" { target i?86-*-* x86_64-*-* } } */

typedef struct {
    float a,b,c;
} S;

S * arr[100];

void bar (float *in[], int n)
{
  int i;
  for (i=0; i<n; i++)
    (*in)[i] = -arr[i]->b;
}
