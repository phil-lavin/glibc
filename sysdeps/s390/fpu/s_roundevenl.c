/* roundevenl() - S390 version.
   Copyright (C) 2019-2025 Free Software Foundation, Inc.

   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public License as
   published by the Free Software Foundation; either version 2.1 of the
   License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, see
   <https://www.gnu.org/licenses/>.  */

#ifdef HAVE_S390_MIN_Z196_ZARCH_ASM_SUPPORT
# define NO_MATH_REDIRECT
# include <math.h>
# include <math_private.h>
# include <libm-alias-ldouble.h>

_Float128
__roundevenl (_Float128 x)
{
  _Float128 y;
  /* The z196 zarch "load fp integer" (fixbra) instruction is rounding
     x to the nearest integer with "ties to even" rounding mode
     (M3-field: 4) where inexact exceptions are suppressed (M4-field: 4).  */
  __asm__ ("fixbra %0,4,%1,4" : "=f" (y) : "f" (x));
  return y;
}
libm_alias_ldouble (__roundeven, roundeven)

#else
# include <sysdeps/ieee754/ldbl-128/s_roundevenl.c>
#endif
