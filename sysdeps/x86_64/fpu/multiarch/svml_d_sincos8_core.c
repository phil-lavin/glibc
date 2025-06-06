/* Multiple versions of vectorized sincos, vector length is 8.
   Copyright (C) 2017-2025 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, see
   <https://www.gnu.org/licenses/>.  */

#define SYMBOL_NAME _ZGVeN8vvv_sincos
#include "ifunc-mathvec-avx512.h"

libc_ifunc_redirected (REDIRECT_NAME, SYMBOL_NAME, IFUNC_SELECTOR ());

#ifdef SHARED
__hidden_ver1 (_ZGVeN8vvv_sincos, __GI__ZGVeN8vvv_sincos,
	       __redirect__ZGVeN8vvv_sincos)
  __attribute__ ((visibility ("hidden")));
#endif
