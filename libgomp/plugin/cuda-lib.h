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

#ifndef CUDA_LIB_H
#define CUDA_LIB_H 1
#include <cuda.h>

extern const char *cuda_error (CUresult r);

/* Convenience macros for the frequently used CUDA library call and
   error handling sequence as well as CUDA library calls that
   do the error checking themselves or don't do it at all.  */

#define CUDA_CALL_ERET(ERET, FN, ...)		\
  do {						\
    unsigned __r				\
      = CUDA_CALL_PREFIX FN (__VA_ARGS__);	\
    if (__r != CUDA_SUCCESS)			\
      {						\
	GOMP_PLUGIN_error (#FN " error: %s",	\
			   cuda_error (__r));	\
	return ERET;				\
      }						\
  } while (0)

#define CUDA_CALL(FN, ...)			\
  CUDA_CALL_ERET (false, FN, __VA_ARGS__)

#define CUDA_CALL_ASSERT(FN, ...)		\
  do {						\
    unsigned __r				\
      = CUDA_CALL_PREFIX FN (__VA_ARGS__);	\
    if (__r != CUDA_SUCCESS)			\
      {						\
	GOMP_PLUGIN_fatal (#FN " error: %s",	\
			   cuda_error (__r));	\
      }						\
  } while (0)

#define CUDA_CALL_NOCHECK(FN, ...)		\
  CUDA_CALL_PREFIX FN (__VA_ARGS__)


#if PLUGIN_NVPTX_DYNAMIC

# define CUDA_ONE_CALL(call) \
  __typeof (call) *call;
struct cuda_lib_s {
#include "cuda-lib.def"
};
extern struct cuda_lib_s cuda_lib;

# define CUDA_CALL_PREFIX cuda_lib.
extern bool init_cuda_lib (void);

#else /* PLUGIN_NVPTX_DYNAMIC */

# define CUDA_CALL_PREFIX
# define init_cuda_lib() true

#endif /* PLUGIN_NVPTX_DYNAMIC */

#endif /* CUDA_LIB_H */
