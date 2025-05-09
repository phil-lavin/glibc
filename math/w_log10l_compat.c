/* Copyright (C) 2011-2025 Free Software Foundation, Inc.
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

#include <fenv.h>
#include <math.h>
#include <math_private.h>
#include <math-svid-compat.h>
#include <libm-alias-ldouble.h>


#if LIBM_SVID_COMPAT
/* wrapper log10l(x) */
long double
__log10l (long double x)
{
  if (__builtin_expect (islessequal (x, 0.0L), 0) && _LIB_VERSION != _IEEE_)
    {
      if (x == 0.0L)
	{
	  feraiseexcept (FE_DIVBYZERO);
	  return __kernel_standard_l (x, x, 218); /* log10(0) */
	}
      else
	{
	  feraiseexcept (FE_INVALID);
	  return __kernel_standard_l (x, x, 219); /* log10(x<0) */
	}
    }

  return  __ieee754_log10l (x);
}
libm_alias_ldouble (__log10, log10)
#endif
