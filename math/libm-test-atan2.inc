/* Test atan2.
   Copyright (C) 1997-2025 Free Software Foundation, Inc.
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

#include "libm-test-driver.c"

static const struct test_ff_f_data atan2_test_data[] =
  {
    TEST_ff_f (atan2, qnan_value, qnan_value, qnan_value, NO_INEXACT_EXCEPTION|ERRNO_UNCHANGED),
    TEST_ff_f (atan2, qnan_value, -qnan_value, qnan_value, NO_INEXACT_EXCEPTION|ERRNO_UNCHANGED),
    TEST_ff_f (atan2, -qnan_value, qnan_value, qnan_value, NO_INEXACT_EXCEPTION|ERRNO_UNCHANGED),
    TEST_ff_f (atan2, -qnan_value, -qnan_value, qnan_value, NO_INEXACT_EXCEPTION|ERRNO_UNCHANGED),
    TEST_ff_f (atan2, snan_value, qnan_value, qnan_value, INVALID_EXCEPTION),
    TEST_ff_f (atan2, snan_value, -qnan_value, qnan_value, INVALID_EXCEPTION),
    TEST_ff_f (atan2, -snan_value, qnan_value, qnan_value, INVALID_EXCEPTION),
    TEST_ff_f (atan2, -snan_value, -qnan_value, qnan_value, INVALID_EXCEPTION),
    TEST_ff_f (atan2, qnan_value, snan_value, qnan_value, INVALID_EXCEPTION),
    TEST_ff_f (atan2, qnan_value, -snan_value, qnan_value, INVALID_EXCEPTION),
    TEST_ff_f (atan2, -qnan_value, snan_value, qnan_value, INVALID_EXCEPTION),
    TEST_ff_f (atan2, -qnan_value, -snan_value, qnan_value, INVALID_EXCEPTION),
    TEST_ff_f (atan2, snan_value, snan_value, qnan_value, INVALID_EXCEPTION),
    TEST_ff_f (atan2, snan_value, -snan_value, qnan_value, INVALID_EXCEPTION),
    TEST_ff_f (atan2, -snan_value, snan_value, qnan_value, INVALID_EXCEPTION),
    TEST_ff_f (atan2, -snan_value, -snan_value, qnan_value, INVALID_EXCEPTION),
    TEST_ff_f (atan2, qnan_value, plus_infty, qnan_value, NO_INEXACT_EXCEPTION|ERRNO_UNCHANGED),
    TEST_ff_f (atan2, qnan_value, minus_infty, qnan_value, NO_INEXACT_EXCEPTION|ERRNO_UNCHANGED),
    TEST_ff_f (atan2, qnan_value, plus_zero, qnan_value, NO_INEXACT_EXCEPTION|ERRNO_UNCHANGED),
    TEST_ff_f (atan2, qnan_value, minus_zero, qnan_value, NO_INEXACT_EXCEPTION|ERRNO_UNCHANGED),
    TEST_ff_f (atan2, qnan_value, 1, qnan_value, NO_INEXACT_EXCEPTION|ERRNO_UNCHANGED),
    TEST_ff_f (atan2, qnan_value, -1, qnan_value, NO_INEXACT_EXCEPTION|ERRNO_UNCHANGED),
    TEST_ff_f (atan2, qnan_value, min_value, qnan_value, NO_INEXACT_EXCEPTION|ERRNO_UNCHANGED),
    TEST_ff_f (atan2, qnan_value, -min_value, qnan_value, NO_INEXACT_EXCEPTION|ERRNO_UNCHANGED),
    TEST_ff_f (atan2, qnan_value, min_subnorm_value, qnan_value, NO_INEXACT_EXCEPTION|ERRNO_UNCHANGED),
    TEST_ff_f (atan2, qnan_value, -min_subnorm_value, qnan_value, NO_INEXACT_EXCEPTION|ERRNO_UNCHANGED),
    TEST_ff_f (atan2, qnan_value, max_value, qnan_value, NO_INEXACT_EXCEPTION|ERRNO_UNCHANGED),
    TEST_ff_f (atan2, qnan_value, -max_value, qnan_value, NO_INEXACT_EXCEPTION|ERRNO_UNCHANGED),
    TEST_ff_f (atan2, -qnan_value, plus_infty, qnan_value, NO_INEXACT_EXCEPTION|ERRNO_UNCHANGED),
    TEST_ff_f (atan2, -qnan_value, minus_infty, qnan_value, NO_INEXACT_EXCEPTION|ERRNO_UNCHANGED),
    TEST_ff_f (atan2, -qnan_value, plus_zero, qnan_value, NO_INEXACT_EXCEPTION|ERRNO_UNCHANGED),
    TEST_ff_f (atan2, -qnan_value, minus_zero, qnan_value, NO_INEXACT_EXCEPTION|ERRNO_UNCHANGED),
    TEST_ff_f (atan2, -qnan_value, 1, qnan_value, NO_INEXACT_EXCEPTION|ERRNO_UNCHANGED),
    TEST_ff_f (atan2, -qnan_value, -1, qnan_value, NO_INEXACT_EXCEPTION|ERRNO_UNCHANGED),
    TEST_ff_f (atan2, -qnan_value, min_value, qnan_value, NO_INEXACT_EXCEPTION|ERRNO_UNCHANGED),
    TEST_ff_f (atan2, -qnan_value, -min_value, qnan_value, NO_INEXACT_EXCEPTION|ERRNO_UNCHANGED),
    TEST_ff_f (atan2, -qnan_value, min_subnorm_value, qnan_value, NO_INEXACT_EXCEPTION|ERRNO_UNCHANGED),
    TEST_ff_f (atan2, -qnan_value, -min_subnorm_value, qnan_value, NO_INEXACT_EXCEPTION|ERRNO_UNCHANGED),
    TEST_ff_f (atan2, -qnan_value, max_value, qnan_value, NO_INEXACT_EXCEPTION|ERRNO_UNCHANGED),
    TEST_ff_f (atan2, -qnan_value, -max_value, qnan_value, NO_INEXACT_EXCEPTION|ERRNO_UNCHANGED),
    TEST_ff_f (atan2, snan_value, plus_infty, qnan_value, INVALID_EXCEPTION),
    TEST_ff_f (atan2, snan_value, minus_infty, qnan_value, INVALID_EXCEPTION),
    TEST_ff_f (atan2, snan_value, plus_zero, qnan_value, INVALID_EXCEPTION),
    TEST_ff_f (atan2, snan_value, minus_zero, qnan_value, INVALID_EXCEPTION),
    TEST_ff_f (atan2, snan_value, 1, qnan_value, INVALID_EXCEPTION),
    TEST_ff_f (atan2, snan_value, -1, qnan_value, INVALID_EXCEPTION),
    TEST_ff_f (atan2, snan_value, min_value, qnan_value, INVALID_EXCEPTION),
    TEST_ff_f (atan2, snan_value, -min_value, qnan_value, INVALID_EXCEPTION),
    TEST_ff_f (atan2, snan_value, min_subnorm_value, qnan_value, INVALID_EXCEPTION),
    TEST_ff_f (atan2, snan_value, -min_subnorm_value, qnan_value, INVALID_EXCEPTION),
    TEST_ff_f (atan2, snan_value, max_value, qnan_value, INVALID_EXCEPTION),
    TEST_ff_f (atan2, snan_value, -max_value, qnan_value, INVALID_EXCEPTION),
    TEST_ff_f (atan2, -snan_value, plus_infty, qnan_value, INVALID_EXCEPTION),
    TEST_ff_f (atan2, -snan_value, minus_infty, qnan_value, INVALID_EXCEPTION),
    TEST_ff_f (atan2, -snan_value, plus_zero, qnan_value, INVALID_EXCEPTION),
    TEST_ff_f (atan2, -snan_value, minus_zero, qnan_value, INVALID_EXCEPTION),
    TEST_ff_f (atan2, -snan_value, 1, qnan_value, INVALID_EXCEPTION),
    TEST_ff_f (atan2, -snan_value, -1, qnan_value, INVALID_EXCEPTION),
    TEST_ff_f (atan2, -snan_value, min_value, qnan_value, INVALID_EXCEPTION),
    TEST_ff_f (atan2, -snan_value, -min_value, qnan_value, INVALID_EXCEPTION),
    TEST_ff_f (atan2, -snan_value, min_subnorm_value, qnan_value, INVALID_EXCEPTION),
    TEST_ff_f (atan2, -snan_value, -min_subnorm_value, qnan_value, INVALID_EXCEPTION),
    TEST_ff_f (atan2, -snan_value, max_value, qnan_value, INVALID_EXCEPTION),
    TEST_ff_f (atan2, -snan_value, -max_value, qnan_value, INVALID_EXCEPTION),
    TEST_ff_f (atan2, plus_infty, qnan_value, qnan_value, NO_INEXACT_EXCEPTION|ERRNO_UNCHANGED),
    TEST_ff_f (atan2, minus_infty, qnan_value, qnan_value, NO_INEXACT_EXCEPTION|ERRNO_UNCHANGED),
    TEST_ff_f (atan2, plus_zero, qnan_value, qnan_value, NO_INEXACT_EXCEPTION|ERRNO_UNCHANGED),
    TEST_ff_f (atan2, minus_zero, qnan_value, qnan_value, NO_INEXACT_EXCEPTION|ERRNO_UNCHANGED),
    TEST_ff_f (atan2, 1, qnan_value, qnan_value, NO_INEXACT_EXCEPTION|ERRNO_UNCHANGED),
    TEST_ff_f (atan2, -1, qnan_value, qnan_value, NO_INEXACT_EXCEPTION|ERRNO_UNCHANGED),
    TEST_ff_f (atan2, min_value, qnan_value, qnan_value, NO_INEXACT_EXCEPTION|ERRNO_UNCHANGED),
    TEST_ff_f (atan2, -min_value, qnan_value, qnan_value, NO_INEXACT_EXCEPTION|ERRNO_UNCHANGED),
    TEST_ff_f (atan2, min_subnorm_value, qnan_value, qnan_value, NO_INEXACT_EXCEPTION|ERRNO_UNCHANGED),
    TEST_ff_f (atan2, -min_subnorm_value, qnan_value, qnan_value, NO_INEXACT_EXCEPTION|ERRNO_UNCHANGED),
    TEST_ff_f (atan2, max_value, qnan_value, qnan_value, NO_INEXACT_EXCEPTION|ERRNO_UNCHANGED),
    TEST_ff_f (atan2, -max_value, qnan_value, qnan_value, NO_INEXACT_EXCEPTION|ERRNO_UNCHANGED),
    TEST_ff_f (atan2, plus_infty, -qnan_value, qnan_value, NO_INEXACT_EXCEPTION|ERRNO_UNCHANGED),
    TEST_ff_f (atan2, minus_infty, -qnan_value, qnan_value, NO_INEXACT_EXCEPTION|ERRNO_UNCHANGED),
    TEST_ff_f (atan2, plus_zero, -qnan_value, qnan_value, NO_INEXACT_EXCEPTION|ERRNO_UNCHANGED),
    TEST_ff_f (atan2, minus_zero, -qnan_value, qnan_value, NO_INEXACT_EXCEPTION|ERRNO_UNCHANGED),
    TEST_ff_f (atan2, 1, -qnan_value, qnan_value, NO_INEXACT_EXCEPTION|ERRNO_UNCHANGED),
    TEST_ff_f (atan2, -1, -qnan_value, qnan_value, NO_INEXACT_EXCEPTION|ERRNO_UNCHANGED),
    TEST_ff_f (atan2, min_value, -qnan_value, qnan_value, NO_INEXACT_EXCEPTION|ERRNO_UNCHANGED),
    TEST_ff_f (atan2, -min_value, -qnan_value, qnan_value, NO_INEXACT_EXCEPTION|ERRNO_UNCHANGED),
    TEST_ff_f (atan2, min_subnorm_value, -qnan_value, qnan_value, NO_INEXACT_EXCEPTION|ERRNO_UNCHANGED),
    TEST_ff_f (atan2, -min_subnorm_value, -qnan_value, qnan_value, NO_INEXACT_EXCEPTION|ERRNO_UNCHANGED),
    TEST_ff_f (atan2, max_value, -qnan_value, qnan_value, NO_INEXACT_EXCEPTION|ERRNO_UNCHANGED),
    TEST_ff_f (atan2, -max_value, -qnan_value, qnan_value, NO_INEXACT_EXCEPTION|ERRNO_UNCHANGED),
    TEST_ff_f (atan2, plus_infty, snan_value, qnan_value, INVALID_EXCEPTION),
    TEST_ff_f (atan2, minus_infty, snan_value, qnan_value, INVALID_EXCEPTION),
    TEST_ff_f (atan2, plus_zero, snan_value, qnan_value, INVALID_EXCEPTION),
    TEST_ff_f (atan2, minus_zero, snan_value, qnan_value, INVALID_EXCEPTION),
    TEST_ff_f (atan2, 1, snan_value, qnan_value, INVALID_EXCEPTION),
    TEST_ff_f (atan2, -1, snan_value, qnan_value, INVALID_EXCEPTION),
    TEST_ff_f (atan2, min_value, snan_value, qnan_value, INVALID_EXCEPTION),
    TEST_ff_f (atan2, -min_value, snan_value, qnan_value, INVALID_EXCEPTION),
    TEST_ff_f (atan2, min_subnorm_value, snan_value, qnan_value, INVALID_EXCEPTION),
    TEST_ff_f (atan2, -min_subnorm_value, snan_value, qnan_value, INVALID_EXCEPTION),
    TEST_ff_f (atan2, max_value, snan_value, qnan_value, INVALID_EXCEPTION),
    TEST_ff_f (atan2, -max_value, snan_value, qnan_value, INVALID_EXCEPTION),
    TEST_ff_f (atan2, plus_infty, -snan_value, qnan_value, INVALID_EXCEPTION),
    TEST_ff_f (atan2, minus_infty, -snan_value, qnan_value, INVALID_EXCEPTION),
    TEST_ff_f (atan2, plus_zero, -snan_value, qnan_value, INVALID_EXCEPTION),
    TEST_ff_f (atan2, minus_zero, -snan_value, qnan_value, INVALID_EXCEPTION),
    TEST_ff_f (atan2, 1, -snan_value, qnan_value, INVALID_EXCEPTION),
    TEST_ff_f (atan2, -1, -snan_value, qnan_value, INVALID_EXCEPTION),
    TEST_ff_f (atan2, min_value, -snan_value, qnan_value, INVALID_EXCEPTION),
    TEST_ff_f (atan2, -min_value, -snan_value, qnan_value, INVALID_EXCEPTION),
    TEST_ff_f (atan2, min_subnorm_value, -snan_value, qnan_value, INVALID_EXCEPTION),
    TEST_ff_f (atan2, -min_subnorm_value, -snan_value, qnan_value, INVALID_EXCEPTION),
    TEST_ff_f (atan2, max_value, -snan_value, qnan_value, INVALID_EXCEPTION),
    TEST_ff_f (atan2, -max_value, -snan_value, qnan_value, INVALID_EXCEPTION),

    AUTO_TESTS_ff_f (atan2),
  };

static void
atan2_test (void)
{
  ALL_RM_TEST (atan2, 0, atan2_test_data, RUN_TEST_LOOP_ff_f, END);
}

static void
do_test (void)
{
  atan2_test ();
}

/*
 * Local Variables:
 * mode:c
 * End:
 */
