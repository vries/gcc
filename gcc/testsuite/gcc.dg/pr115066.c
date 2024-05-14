/* { dg-do compile } */
/* { dg-skip-if "split DWARF unsupported" { hppa*-*-hpux* powerpc*-ibm-aix* *-*-darwin* } } */
/* { dg-options "-gsplit-dwarf -g3 -dA -gdwarf-4" } */
/* { dg-final { scan-assembler-times {\.section\t"?\.debug_macro} 1 } } */
/* { dg-final { scan-assembler-not {\.byte\t0x5\t.* Define macro strp} } } */
/* { dg-final { scan-assembler {\.byte\t0xb\t.* Define macro strx} } } */

#define foo 1
