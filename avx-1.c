typedef double v2df __attribute__ ((__vector_size__ (16)));

v2df
foo (unsigned char u, v2df a, v2df b)
{
  v2df zero = __extension__ (typeof (zero)){ 0.0f, 0.0f };
  return __builtin_ia32_rcp14sd_mask (a, b, zero, u);
}
