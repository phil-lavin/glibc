/* Double-Precision vector (Advanced SIMD) inverse sinpi function

   Copyright (C) 2025 Free Software Foundation, Inc.
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

static const struct data
{
  float64x2_t c0, c2, c4, c6, c8, c10;
  float64x2_t pi_over_2, inv_pi;
  uint64x2_t abs_mask;
  double c1, c3, c5, c7, c9, c11;
} data = {
  /* Polynomial approximation of  (asin(sqrt(x)) - sqrt(x)) / (x * sqrt(x))
     on [ 0x1p-106, 0x1p-2 ], relative error: 0x1.c3d8e169p-57.  */
  .c0 = V2 (0x1.555555555554ep-3),	  .c1 = 0x1.3333333337233p-4,
  .c2 = V2 (0x1.6db6db67f6d9fp-5),	  .c3 = 0x1.f1c71fbd29fbbp-6,
  .c4 = V2 (0x1.6e8b264d467d6p-6),	  .c5 = 0x1.1c5997c357e9dp-6,
  .c6 = V2 (0x1.c86a22cd9389dp-7),	  .c7 = 0x1.856073c22ebbep-7,
  .c8 = V2 (0x1.fd1151acb6bedp-8),	  .c9 = 0x1.087182f799c1dp-6,
  .c10 = V2 (-0x1.6602748120927p-7),	  .c11 = 0x1.cfa0dd1f9478p-6,
  .pi_over_2 = V2 (0x1.921fb54442d18p+0), .abs_mask = V2 (0x7fffffffffffffff),
  .inv_pi = V2 (0x1.45f306dc9c883p-2),
};

/* Double-precision implementation of vector asinpi(x).

   For |x| in [0, 0.5], use an order 11 polynomial P such that the final
   approximation is an odd polynomial: asin(x) ~ x + x^3 P(x^2).
   asinpi(x) = asin(x) * 1/pi.

   The largest observed error in this region is 1.63 ulps,
   _ZGVnN2v_asinpi (0x1.9125919fa617p-19) got 0x1.fec183497ea53p-21
					 want 0x1.fec183497ea51p-21.

   For |x| in [0.5, 1.0], use same approximation with a change of variable

     asin(x) = pi/2 - (y + y * z * P(z)), with  z = (1-x)/2 and y = sqrt(z).

   The largest observed error in this region is 3.04 ulps,
   _ZGVnN2v_asinpi (0x1.0479b7bd98553p-1) got 0x1.5beebec797326p-3
					 want 0x1.5beebec797329p-3.  */

float64x2_t VPCS_ATTR V_NAME_D1 (asinpi) (float64x2_t x)
{
  const struct data *d = ptr_barrier (&data);
  float64x2_t ax = vabsq_f64 (x);

  uint64x2_t a_lt_half = vcaltq_f64 (x, v_f64 (0.5));

  /* Evaluate polynomial Q(x) = y + y * z * P(z) with
     z = x ^ 2 and y = |x|            , if |x| < 0.5
     z = (1 - |x|) / 2 and y = sqrt(z), if |x| >= 0.5.  */
  float64x2_t z2 = vbslq_f64 (a_lt_half, vmulq_f64 (x, x),
			      vfmsq_n_f64 (v_f64 (0.5), ax, 0.5));
  float64x2_t z = vbslq_f64 (a_lt_half, ax, vsqrtq_f64 (z2));

  /* Use a single polynomial approximation P for both intervals.  */
  float64x2_t z4 = vmulq_f64 (z2, z2);
  float64x2_t z8 = vmulq_f64 (z4, z4);
  float64x2_t z16 = vmulq_f64 (z8, z8);

  /* order-11 Estrin.  */
  float64x2_t c13 = vld1q_f64 (&d->c1);
  float64x2_t c57 = vld1q_f64 (&d->c5);
  float64x2_t c911 = vld1q_f64 (&d->c9);

  float64x2_t p01 = vfmaq_laneq_f64 (d->c0, z2, c13, 0);
  float64x2_t p23 = vfmaq_laneq_f64 (d->c2, z2, c13, 1);
  float64x2_t p03 = vfmaq_f64 (p01, z4, p23);

  float64x2_t p45 = vfmaq_laneq_f64 (d->c4, z2, c57, 0);
  float64x2_t p67 = vfmaq_laneq_f64 (d->c6, z2, c57, 1);
  float64x2_t p47 = vfmaq_f64 (p45, z4, p67);

  float64x2_t p89 = vfmaq_laneq_f64 (d->c8, z2, c911, 0);
  float64x2_t p1011 = vfmaq_laneq_f64 (d->c10, z2, c911, 1);
  float64x2_t p811 = vfmaq_f64 (p89, z4, p1011);

  float64x2_t p07 = vfmaq_f64 (p03, z8, p47);
  float64x2_t p = vfmaq_f64 (p07, z16, p811);

  /* Finalize polynomial: z + z * z2 * P(z2).  */
  p = vfmaq_f64 (z, vmulq_f64 (z, z2), p);

  /* asin(|x|) = Q(|x|)          , for |x| < 0.5
	       = pi/2 - 2 Q(|x|), for |x| >= 0.5.  */
  float64x2_t y = vbslq_f64 (a_lt_half, p, vfmsq_n_f64 (d->pi_over_2, p, 2.0));
  /* asinpi(|x|) = asin(|x|) /pi.  */
  y = vmulq_f64 (y, d->inv_pi);

  /* Copy sign.  */
  return vbslq_f64 (d->abs_mask, y, x);
}
