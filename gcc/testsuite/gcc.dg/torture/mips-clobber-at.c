/* "$1" used to be mapped to the internal frame pointer.  */
/* { dg-do compile { target mips*-*-* } } */
int foo () { asm volatile ("#" ::: "$1"); }
