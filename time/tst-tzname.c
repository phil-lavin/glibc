/* Test that tzset sets tzname correctly (BZ #19253).
   Copyright (C) 2015-2025 Free Software Foundation, Inc.
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static int
do_test (void)
{
  int result = 0;

  setenv ("TZ", TZDEFRULES, 1);
  tzset ();
  const char *stdtz = strdup (tzname[0]);
  setenv ("TZ", "STD-1DST", 1);
  tzset ();
  if (strcmp (tzname[0], "STD") != 0)
    {
      printf ("FAIL: TZ=STD-1DST, tzname[0] = %s\n", tzname[0]);
      result = 1;
    }
  setenv ("TZ", TZDEFRULES, 1);
  tzset ();
  if (strcmp (tzname[0], stdtz) != 0)
    {
      printf ("FAIL: TZ=%s, tzname[0] = %s\n", TZDEFRULES, tzname[0]);
      result = 1;
    }
  return result;
}

#define TEST_FUNCTION do_test ()
#include "../test-skeleton.c"
