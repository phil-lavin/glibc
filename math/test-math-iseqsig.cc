/* Test for the C++ implementation of iseqsig.
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

#define _GNU_SOURCE 1
#include <float.h>
#include <math.h>
#include <stdio.h>

#include <limits>

/* There is no NaN for _Float128 in std::numeric_limits.
   Include ieee754_float128.h and use the bitfields in the union
   ieee854_float128.ieee_nan to build a NaN.  */
#if __HAVE_DISTINCT_FLOAT128
# include <ieee754_float128.h>
#endif

#include <support/check.h>

static void
check (int actual, int expected, const char *actual_expr, int line)
{
  if (actual != expected)
    {
      support_record_failure ();
      printf ("%s:%d: error: %s\n", __FILE__, line, actual_expr);
      printf ("%s:%d:   expected: %d\n", __FILE__, line, expected);
      printf ("%s:%d:   actual: %d\n", __FILE__, line, actual);
    }
}

#define CHECK(actual, expected) \
  check ((actual), (expected), #actual, __LINE__)

template <class T1, class T2>
static void
check_type ()
{
  T1 t1 = 0;
  T2 t2 = 0;
  CHECK (iseqsig (t1, t2), 1);

  t2 = 1;
  CHECK (iseqsig (t1, t2), 0);

  if (std::numeric_limits<T1>::has_quiet_NaN
      && std::numeric_limits<T2>::has_quiet_NaN)
    {
      CHECK (iseqsig (std::numeric_limits<T1>::quiet_NaN (), t2), 0);
      CHECK (iseqsig (t1, std::numeric_limits<T2>::quiet_NaN ()), 0);
      CHECK (iseqsig (std::numeric_limits<T1>::quiet_NaN (),
		      std::numeric_limits<T2>::quiet_NaN ()), 0);
    }
}

/* Run minimal tests of iseqsig for a pair of types, without relying
   on C++ library support or knowledge of the representation.  */
template <class T1, class T2>
static void
check_type_minimal ()
{
  T1 t1 = 0;
  T2 t2 = 0;
  CHECK (iseqsig (t1, t2), 1);

  t2 = 1;
  CHECK (iseqsig (t1, t2), 0);
}

#if __HAVE_DISTINCT_FLOAT128
static void
check_float128 ()
{
  ieee854_float128 q1, q2, q3_nan;

  q1.d = 0;
  q2.d = 1;
  q3_nan.ieee_nan.negative = 0;
  q3_nan.ieee_nan.exponent = 0x7FFF;
  q3_nan.ieee_nan.quiet_nan = 1;
  q3_nan.ieee_nan.mantissa0 = 0x0000;
  q3_nan.ieee_nan.mantissa1 = 0x00000000;
  q3_nan.ieee_nan.mantissa2 = 0x00000000;
  q3_nan.ieee_nan.mantissa3 = 0x00000000;

  CHECK (iseqsig (q1.d, q1.d), 1);
  CHECK (iseqsig (q1.d, q2.d), 0);
  CHECK (iseqsig (q1.d, q3_nan.d), 0);
  CHECK (iseqsig (q3_nan.d, q3_nan.d), 0);
}
#endif

static int
do_test (void)
{
  check_type<float, float> ();
  check_type<float, double> ();
  check_type<float, long double> ();
  check_type<double, float> ();
  check_type<double, double> ();
  check_type<double, long double> ();
  check_type<long double, float> ();
  check_type<long double, double> ();
  check_type<long double, long double> ();
#if __HAVE_DISTINCT_FLOAT128
  check_float128 ();
#endif

#if __HAVE_FLOAT16
  check_type_minimal<float, _Float16> ();
#endif
#if __HAVE_FLOAT32
  check_type_minimal<float, _Float32> ();
#endif
#if __HAVE_FLOAT64
  check_type_minimal<float, _Float64> ();
#endif
#if __HAVE_FLOAT128
  check_type_minimal<float, _Float128> ();
#endif
#if __HAVE_FLOAT32X
  check_type_minimal<float, _Float32x> ();
#endif
#if __HAVE_FLOAT64X
  check_type_minimal<float, _Float64x> ();
#endif
#if __HAVE_FLOAT128X
  check_type_minimal<float, _Float128x> ();
#endif
#if __HAVE_FLOAT16
  check_type_minimal<double, _Float16> ();
#endif
#if __HAVE_FLOAT32
  check_type_minimal<double, _Float32> ();
#endif
#if __HAVE_FLOAT64
  check_type_minimal<double, _Float64> ();
#endif
#if __HAVE_FLOAT128
  check_type_minimal<double, _Float128> ();
#endif
#if __HAVE_FLOAT32X
  check_type_minimal<double, _Float32x> ();
#endif
#if __HAVE_FLOAT64X
  check_type_minimal<double, _Float64x> ();
#endif
#if __HAVE_FLOAT128X
  check_type_minimal<double, _Float128x> ();
#endif
#if __HAVE_FLOAT16
  check_type_minimal<long double, _Float16> ();
#endif
#if __HAVE_FLOAT32
  check_type_minimal<long double, _Float32> ();
#endif
#if __HAVE_FLOAT64
  check_type_minimal<long double, _Float64> ();
#endif
#if __HAVE_FLOAT128 && LDBL_MANT_DIG != 106
  check_type_minimal<long double, _Float128> ();
#endif
#if __HAVE_FLOAT32X
  check_type_minimal<long double, _Float32x> ();
#endif
#if __HAVE_FLOAT64X && LDBL_MANT_DIG != 106
  check_type_minimal<long double, _Float64x> ();
#endif
#if __HAVE_FLOAT128X && LDBL_MANT_DIG != 106
  check_type_minimal<long double, _Float128x> ();
#endif
#if __HAVE_FLOAT16
  check_type_minimal<_Float16, float> ();
#endif
#if __HAVE_FLOAT16
  check_type_minimal<_Float16, double> ();
#endif
#if __HAVE_FLOAT16
  check_type_minimal<_Float16, long double> ();
#endif
#if __HAVE_FLOAT16
  check_type_minimal<_Float16, _Float16> ();
#endif
#if __HAVE_FLOAT16 && __HAVE_FLOAT32
  check_type_minimal<_Float16, _Float32> ();
#endif
#if __HAVE_FLOAT16 && __HAVE_FLOAT64
  check_type_minimal<_Float16, _Float64> ();
#endif
#if __HAVE_FLOAT16 && __HAVE_FLOAT128
  check_type_minimal<_Float16, _Float128> ();
#endif
#if __HAVE_FLOAT16 && __HAVE_FLOAT32X
  check_type_minimal<_Float16, _Float32x> ();
#endif
#if __HAVE_FLOAT16 && __HAVE_FLOAT64X
  check_type_minimal<_Float16, _Float64x> ();
#endif
#if __HAVE_FLOAT16 && __HAVE_FLOAT128X
  check_type_minimal<_Float16, _Float128x> ();
#endif
#if __HAVE_FLOAT32
  check_type_minimal<_Float32, float> ();
#endif
#if __HAVE_FLOAT32
  check_type_minimal<_Float32, double> ();
#endif
#if __HAVE_FLOAT32
  check_type_minimal<_Float32, long double> ();
#endif
#if __HAVE_FLOAT32 && __HAVE_FLOAT16
  check_type_minimal<_Float32, _Float16> ();
#endif
#if __HAVE_FLOAT32
  check_type_minimal<_Float32, _Float32> ();
#endif
#if __HAVE_FLOAT32 && __HAVE_FLOAT64
  check_type_minimal<_Float32, _Float64> ();
#endif
#if __HAVE_FLOAT32 && __HAVE_FLOAT128
  check_type_minimal<_Float32, _Float128> ();
#endif
#if __HAVE_FLOAT32 && __HAVE_FLOAT32X
  check_type_minimal<_Float32, _Float32x> ();
#endif
#if __HAVE_FLOAT32 && __HAVE_FLOAT64X
  check_type_minimal<_Float32, _Float64x> ();
#endif
#if __HAVE_FLOAT32 && __HAVE_FLOAT128X
  check_type_minimal<_Float32, _Float128x> ();
#endif
#if __HAVE_FLOAT64
  check_type_minimal<_Float64, float> ();
#endif
#if __HAVE_FLOAT64
  check_type_minimal<_Float64, double> ();
#endif
#if __HAVE_FLOAT64
  check_type_minimal<_Float64, long double> ();
#endif
#if __HAVE_FLOAT64 && __HAVE_FLOAT16
  check_type_minimal<_Float64, _Float16> ();
#endif
#if __HAVE_FLOAT64 && __HAVE_FLOAT32
  check_type_minimal<_Float64, _Float32> ();
#endif
#if __HAVE_FLOAT64
  check_type_minimal<_Float64, _Float64> ();
#endif
#if __HAVE_FLOAT64 && __HAVE_FLOAT128
  check_type_minimal<_Float64, _Float128> ();
#endif
#if __HAVE_FLOAT64 && __HAVE_FLOAT32X
  check_type_minimal<_Float64, _Float32x> ();
#endif
#if __HAVE_FLOAT64 && __HAVE_FLOAT64X
  check_type_minimal<_Float64, _Float64x> ();
#endif
#if __HAVE_FLOAT64 && __HAVE_FLOAT128X
  check_type_minimal<_Float64, _Float128x> ();
#endif
#if __HAVE_FLOAT128
  check_type_minimal<_Float128, float> ();
#endif
#if __HAVE_FLOAT128
  check_type_minimal<_Float128, double> ();
#endif
#if __HAVE_FLOAT128 && LDBL_MANT_DIG != 106
  check_type_minimal<_Float128, long double> ();
#endif
#if __HAVE_FLOAT128 && __HAVE_FLOAT16
  check_type_minimal<_Float128, _Float16> ();
#endif
#if __HAVE_FLOAT128 && __HAVE_FLOAT32
  check_type_minimal<_Float128, _Float32> ();
#endif
#if __HAVE_FLOAT128 && __HAVE_FLOAT64
  check_type_minimal<_Float128, _Float64> ();
#endif
#if __HAVE_FLOAT128
  check_type_minimal<_Float128, _Float128> ();
#endif
#if __HAVE_FLOAT128 && __HAVE_FLOAT32X
  check_type_minimal<_Float128, _Float32x> ();
#endif
#if __HAVE_FLOAT128 && __HAVE_FLOAT64X
  check_type_minimal<_Float128, _Float64x> ();
#endif
#if __HAVE_FLOAT128 && __HAVE_FLOAT128X
  check_type_minimal<_Float128, _Float128x> ();
#endif
#if __HAVE_FLOAT32X
  check_type_minimal<_Float32x, float> ();
#endif
#if __HAVE_FLOAT32X
  check_type_minimal<_Float32x, double> ();
#endif
#if __HAVE_FLOAT32X
  check_type_minimal<_Float32x, long double> ();
#endif
#if __HAVE_FLOAT32X && __HAVE_FLOAT16
  check_type_minimal<_Float32x, _Float16> ();
#endif
#if __HAVE_FLOAT32X && __HAVE_FLOAT32
  check_type_minimal<_Float32x, _Float32> ();
#endif
#if __HAVE_FLOAT32X && __HAVE_FLOAT64
  check_type_minimal<_Float32x, _Float64> ();
#endif
#if __HAVE_FLOAT32X && __HAVE_FLOAT128
  check_type_minimal<_Float32x, _Float128> ();
#endif
#if __HAVE_FLOAT32X
  check_type_minimal<_Float32x, _Float32x> ();
#endif
#if __HAVE_FLOAT32X && __HAVE_FLOAT64X
  check_type_minimal<_Float32x, _Float64x> ();
#endif
#if __HAVE_FLOAT32X && __HAVE_FLOAT128X
  check_type_minimal<_Float32x, _Float128x> ();
#endif
#if __HAVE_FLOAT64X
  check_type_minimal<_Float64x, float> ();
#endif
#if __HAVE_FLOAT64X
  check_type_minimal<_Float64x, double> ();
#endif
#if __HAVE_FLOAT64X && LDBL_MANT_DIG != 106
  check_type_minimal<_Float64x, long double> ();
#endif
#if __HAVE_FLOAT64X && __HAVE_FLOAT16
  check_type_minimal<_Float64x, _Float16> ();
#endif
#if __HAVE_FLOAT64X && __HAVE_FLOAT32
  check_type_minimal<_Float64x, _Float32> ();
#endif
#if __HAVE_FLOAT64X && __HAVE_FLOAT64
  check_type_minimal<_Float64x, _Float64> ();
#endif
#if __HAVE_FLOAT64X && __HAVE_FLOAT128
  check_type_minimal<_Float64x, _Float128> ();
#endif
#if __HAVE_FLOAT64X && __HAVE_FLOAT32X
  check_type_minimal<_Float64x, _Float32x> ();
#endif
#if __HAVE_FLOAT64X
  check_type_minimal<_Float64x, _Float64x> ();
#endif
#if __HAVE_FLOAT64X && __HAVE_FLOAT128X
  check_type_minimal<_Float64x, _Float128x> ();
#endif
#if __HAVE_FLOAT128X
  check_type_minimal<_Float128x, float> ();
#endif
#if __HAVE_FLOAT128X
  check_type_minimal<_Float128x, double> ();
#endif
#if __HAVE_FLOAT128X && LDBL_MANT_DIG != 106
  check_type_minimal<_Float128x, long double> ();
#endif
#if __HAVE_FLOAT128X && __HAVE_FLOAT16
  check_type_minimal<_Float128x, _Float16> ();
#endif
#if __HAVE_FLOAT128X && __HAVE_FLOAT32
  check_type_minimal<_Float128x, _Float32> ();
#endif
#if __HAVE_FLOAT128X && __HAVE_FLOAT64
  check_type_minimal<_Float128x, _Float64> ();
#endif
#if __HAVE_FLOAT128X && __HAVE_FLOAT128
  check_type_minimal<_Float128x, _Float128> ();
#endif
#if __HAVE_FLOAT128X && __HAVE_FLOAT32X
  check_type_minimal<_Float128x, _Float32x> ();
#endif
#if __HAVE_FLOAT128X && __HAVE_FLOAT64X
  check_type_minimal<_Float128x, _Float64x> ();
#endif
#if __HAVE_FLOAT128X
  check_type_minimal<_Float128x, _Float128x> ();
#endif

  return 0;
}

#include <support/test-driver.c>
