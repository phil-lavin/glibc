/* Double-precision vector (AdvSIMD) cbrt function

   Copyright (C) 2024-2025 Free Software Foundation, Inc.
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

#include "v_math.h"
#include "poly_advsimd_f64.h"

const static struct data
{
  float64x2_t poly[4], one_third, shift;
  int64x2_t exp_bias;
  uint64x2_t abs_mask, tiny_bound;
  uint32x4_t thresh;
  double table[5];
} data = {
  .shift = V2 (0x1.8p52),
  .poly = { /* Generated with fpminimax in [0.5, 1].  */
            V2 (0x1.c14e8ee44767p-2), V2 (0x1.dd2d3f99e4c0ep-1),
	    V2 (-0x1.08e83026b7e74p-1), V2 (0x1.2c74eaa3ba428p-3) },
  .exp_bias = V2 (1022),
  .abs_mask = V2(0x7fffffffffffffff),
  .tiny_bound = V2(0x0010000000000000), /* Smallest normal.  */
  .thresh = V4(0x7fe00000),   /* asuint64 (infinity) - tiny_bound.  */
  .one_third = V2(0x1.5555555555555p-2),
  .table = { /* table[i] = 2^((i - 2) / 3).  */
             0x1.428a2f98d728bp-1, 0x1.965fea53d6e3dp-1, 0x1p0,
	     0x1.428a2f98d728bp0, 0x1.965fea53d6e3dp0 }
};

#define MantissaMask v_u64 (0x000fffffffffffff)

static float64x2_t NOINLINE VPCS_ATTR
special_case (float64x2_t x, float64x2_t y, uint32x2_t special)
{
  return v_call_f64 (cbrt, x, y, vmovl_u32 (special));
}

/* Approximation for double-precision vector cbrt(x), using low-order polynomial
   and two Newton iterations. Greatest observed error is 1.79 ULP. Errors repeat
   according to the exponent, for instance an error observed for double value
   m * 2^e will be observed for any input m * 2^(e + 3*i), where i is an
   integer.
   __v_cbrt(0x1.fffff403f0bc6p+1) got 0x1.965fe72821e9bp+0
				 want 0x1.965fe72821e99p+0.  */
VPCS_ATTR float64x2_t V_NAME_D1 (cbrt) (float64x2_t x)
{
  const struct data *d = ptr_barrier (&data);
  uint64x2_t iax = vreinterpretq_u64_f64 (vabsq_f64 (x));

  /* Subnormal, +/-0 and special values.  */
  uint32x2_t special
      = vcge_u32 (vsubhn_u64 (iax, d->tiny_bound), vget_low_u32 (d->thresh));

  /* Decompose |x| into m * 2^e, where m is in [0.5, 1.0]. This is a vector
     version of frexp, which gets subnormal values wrong - these have to be
     special-cased as a result.  */
  float64x2_t m = vbslq_f64 (MantissaMask, x, v_f64 (0.5));
  int64x2_t exp_bias = d->exp_bias;
  uint64x2_t ia12 = vshrq_n_u64 (iax, 52);
  int64x2_t e = vsubq_s64 (vreinterpretq_s64_u64 (ia12), exp_bias);

  /* Calculate rough approximation for cbrt(m) in [0.5, 1.0], starting point for
     Newton iterations.  */
  float64x2_t p = v_pairwise_poly_3_f64 (m, vmulq_f64 (m, m), d->poly);
  float64x2_t one_third = d->one_third;
  /* Two iterations of Newton's method for iteratively approximating cbrt.  */
  float64x2_t m_by_3 = vmulq_f64 (m, one_third);
  float64x2_t two_thirds = vaddq_f64 (one_third, one_third);
  float64x2_t a
      = vfmaq_f64 (vdivq_f64 (m_by_3, vmulq_f64 (p, p)), two_thirds, p);
  a = vfmaq_f64 (vdivq_f64 (m_by_3, vmulq_f64 (a, a)), two_thirds, a);

  /* Assemble the result by the following:

     cbrt(x) = cbrt(m) * 2 ^ (e / 3).

     We can get 2 ^ round(e / 3) using ldexp and integer divide, but since e is
     not necessarily a multiple of 3 we lose some information.

     Let q = 2 ^ round(e / 3), then t = 2 ^ (e / 3) / q.

     Then we know t = 2 ^ (i / 3), where i is the remainder from e / 3, which is
     an integer in [-2, 2], and can be looked up in the table T. Hence the
     result is assembled as:

     cbrt(x) = cbrt(m) * t * 2 ^ round(e / 3) * sign.  */

  float64x2_t ef = vcvtq_f64_s64 (e);
  float64x2_t eb3f = vrndnq_f64 (vmulq_f64 (ef, one_third));
  int64x2_t em3 = vcvtq_s64_f64 (vfmsq_f64 (ef, eb3f, v_f64 (3)));
  int64x2_t ey = vcvtq_s64_f64 (eb3f);

  float64x2_t my = (float64x2_t){ d->table[em3[0] + 2], d->table[em3[1] + 2] };
  my = vmulq_f64 (my, a);

  /* Vector version of ldexp.  */
  float64x2_t y = vreinterpretq_f64_s64 (
      vshlq_n_s64 (vaddq_s64 (ey, vaddq_s64 (exp_bias, v_s64 (1))), 52));
  y = vmulq_f64 (y, my);

  if (__glibc_unlikely (v_any_u32h (special)))
    return special_case (x, vbslq_f64 (d->abs_mask, y, x), special);

  /* Copy sign.  */
  return vbslq_f64 (d->abs_mask, y, x);
}
