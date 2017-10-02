/* { dg-do compile } */
/* { dg-additional-options "-fdump-tree-optimized" } */
/* { dg-skip-if "" { *-*-* } { "-fno-fat-lto-objects" } { "" } } */

static const unsigned int foo = 1;
unsigned int test( void )
{
  const volatile unsigned int *bar = &foo;
  return ( *bar );
}

/* { dg-final { scan-tree-dump-not "return 1" "optimized" } } */
