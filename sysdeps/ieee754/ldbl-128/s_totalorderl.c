/* Total order operation.  ldbl-128 version.
   Copyright (C) 2016-2025 Free Software Foundation, Inc.
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

#include <math.h>
#include <math_private.h>
#include <libm-alias-ldouble.h>
#include <nan-high-order-bit.h>
#include <stdint.h>
#include <shlib-compat.h>
#include <first-versions.h>

int
__totalorderl (const _Float128 *x, const _Float128 *y)
{
  int64_t hx, hy;
  uint64_t lx, ly;
  GET_LDOUBLE_WORDS64 (hx, lx, *x);
  GET_LDOUBLE_WORDS64 (hy, ly, *y);
#if HIGH_ORDER_BIT_IS_SET_FOR_SNAN
  uint64_t uhx = hx & 0x7fffffffffffffffULL;
  uint64_t uhy = hy & 0x7fffffffffffffffULL;
  /* For the preferred quiet NaN convention, this operation is a
     comparison of the representations of the arguments interpreted as
     sign-magnitude integers.  If both arguments are NaNs, invert the
     quiet/signaling bit so comparing that way works.  */
  if ((uhx > 0x7fff000000000000ULL || (uhx == 0x7fff000000000000ULL
				       && lx != 0))
      && (uhy > 0x7fff000000000000ULL || (uhy == 0x7fff000000000000ULL
					  && ly != 0)))
    {
      hx ^= 0x0000800000000000ULL;
      hy ^= 0x0000800000000000ULL;
    }
#endif
  uint64_t hx_sign = hx >> 63;
  uint64_t hy_sign = hy >> 63;
  hx ^= hx_sign >> 1;
  lx ^= hx_sign;
  hy ^= hy_sign >> 1;
  ly ^= hy_sign;
  return hx < hy || (hx == hy && lx <= ly);
}
#ifdef SHARED
# define CONCATX(x, y) x ## y
# define CONCAT(x, y) CONCATX (x, y)
# define UNIQUE_ALIAS(name) CONCAT (name, __COUNTER__)
# define do_symbol(orig_name, name, aliasname)		\
  strong_alias (orig_name, name)			\
  versioned_symbol (libm, name, aliasname, GLIBC_2_31)
# undef weak_alias
# define weak_alias(name, aliasname)			\
  do_symbol (name, UNIQUE_ALIAS (name), aliasname);
#endif
libm_alias_ldouble (__totalorder, totalorder)
#if SHLIB_COMPAT (libm, GLIBC_2_25, GLIBC_2_31)
int
attribute_compat_text_section
__totalorder_compatl (_Float128 x, _Float128 y)
{
  return __totalorderl (&x, &y);
}
/* On platforms that reuse the _Float128 implementation for IEEE long
   double (powerpc64le), the libm_alias_float128_other_r_ldbl macro
   (which is called by the libm_alias_ldouble macro) is used to create
   aliases between *f128 (_Float128 API) and __*ieee128 functions.
   However, this compat version of totalorderl is older than the
   availability of __ieee*128 symbols, thus, the compat alias is not
   required, nor desired.  */
#undef libm_alias_float128_other_r_ldbl
#define libm_alias_float128_other_r_ldbl(from, to, r)
#undef do_symbol
#define do_symbol(orig_name, name, aliasname)			\
  strong_alias (orig_name, name)				\
  compat_symbol (libm, name, aliasname,				\
		 CONCAT (FIRST_VERSION_libm_, aliasname))
libm_alias_ldouble (__totalorder_compat, totalorder)
#endif
