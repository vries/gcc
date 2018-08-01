/* Plugin for NVPTX execution -- cuda library interface.

   Copyright (C) 2018 Free Software Foundation, Inc.

   This file is part of the GNU Offloading and Multi Processing Library
   (libgomp).

   Libgomp is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3, or (at your option)
   any later version.

   Libgomp is distributed in the hope that it will be useful, but WITHOUT ANY
   WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
   FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
   more details.

   Under Section 7 of GPL version 3, you are granted additional
   permissions described in the GCC Runtime Library Exception, version
   3.1, as published by the Free Software Foundation.

   You should have received a copy of the GNU General Public License and
   a copy of the GCC Runtime Library Exception along with this program;
   see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see
   <http://www.gnu.org/licenses/>.  */

#include "cuda-lib.h"

#if PLUGIN_NVPTX_DYNAMIC

# include <dlfcn.h>

struct cuda_lib_s cuda_lib;

/* -1 if init_cuda_lib has not been called yet, false
   if it has been and failed, true if it has been and succeeded.  */
static signed char cuda_lib_inited = -1;

/* Dynamically load the CUDA runtime library and initialize function
   pointers, return false if unsuccessful, true if successful.  */

bool
init_cuda_lib (void)
{
  if (cuda_lib_inited != -1)
    return cuda_lib_inited;
  const char *cuda_runtime_lib = "libcuda.so.1";
  void *h = dlopen (cuda_runtime_lib, RTLD_LAZY);
  cuda_lib_inited = false;
  if (h == NULL)
    return false;
# undef CUDA_ONE_CALL
# define CUDA_ONE_CALL(call) CUDA_ONE_CALL_1 (call)
# define CUDA_ONE_CALL_1(call) \
  cuda_lib.call = dlsym (h, #call);	\
  if (cuda_lib.call == NULL)		\
    return false;
#include "cuda-lib.def"
# undef CUDA_ONE_CALL
# undef CUDA_ONE_CALL_1
  cuda_lib_inited = true;
  return true;
}
#endif

const char *
cuda_error (CUresult r)
{
#if CUDA_VERSION < 7000
  /* Specified in documentation and present in library from at least
     5.5.  Not declared in header file prior to 7.0.  */
  extern CUresult cuGetErrorString (CUresult, const char **);
#endif
  const char *desc;

  r = CUDA_CALL_NOCHECK (cuGetErrorString, r, &desc);
  if (r != CUDA_SUCCESS)
    desc = "unknown cuda error";

  return desc;
}
