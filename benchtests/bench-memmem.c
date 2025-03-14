/* Measure memmem functions.
   Copyright (C) 2013-2025 Free Software Foundation, Inc.
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

#define TEST_MAIN
#define TEST_NAME "memmem"
#define BUF1PAGES 20
#define ITERATIONS 100
#include "bench-string.h"
#include "json-lib.h"

typedef char *(*proto_t) (const void *, size_t, const void *, size_t);

void *
basic_memmem (const void *haystack, size_t hs_len, const void *needle,
	      size_t ne_len)
{
  const char *hs = haystack;
  const char *ne = needle;

  if (ne_len == 0)
    return (void *)hs;
  int i;
  int c = ne[0];
  const char *end = hs + hs_len - ne_len;

  for ( ; hs <= end; hs++)
  {
    if (hs[0] != c)
      continue;
    for (i = ne_len - 1; i != 0; i--)
      if (hs[i] != ne[i])
        break;
    if (i == 0)
      return (void *)hs;
  }

  return NULL;
}

#define RETURN_TYPE void *
#define AVAILABLE(h, h_l, j, n_l) ((j) <= (h_l) - (n_l))
#define FASTSEARCH(S,C,N) (void*) memchr ((void *)(S), (C), (N))
#include "../string/str-two-way.h"

void *
twoway_memmem (const void *haystack_start, size_t haystack_len,
	       const void *needle_start, size_t needle_len)
{
  /* Abstract memory is considered to be an array of 'unsigned char' values,
     not an array of 'char' values.  See ISO C 99 section 6.2.6.1.  */
  const unsigned char *haystack = (const unsigned char *) haystack_start;
  const unsigned char *needle = (const unsigned char *) needle_start;

  if (needle_len == 0)
    /* The first occurrence of the empty string is deemed to occur at
       the beginning of the string.  */
    return (void *) haystack;

  /* Sanity check, otherwise the loop might search through the whole
     memory.  */
  if (__glibc_unlikely (haystack_len < needle_len))
    return NULL;

  /* Use optimizations in memchr when possible, to reduce the search
     size of haystack using a linear algorithm with a smaller
     coefficient.  However, avoid memchr for long needles, since we
     can often achieve sublinear performance.  */
  if (needle_len < LONG_NEEDLE_THRESHOLD)
    {
      haystack = memchr (haystack, *needle, haystack_len);
      if (!haystack || __builtin_expect (needle_len == 1, 0))
	return (void *) haystack;
      haystack_len -= haystack - (const unsigned char *) haystack_start;
      if (haystack_len < needle_len)
	return NULL;
      /* Check whether we have a match.  This improves performance since we
	 avoid the initialization overhead of the two-way algorithm.  */
      if (memcmp (haystack, needle, needle_len) == 0)
	return (void *) haystack;
      return two_way_short_needle (haystack, haystack_len, needle, needle_len);
    }
  else
    return two_way_long_needle (haystack, haystack_len, needle, needle_len);
}

IMPL (memmem, 1)
IMPL (twoway_memmem, 0)
IMPL (basic_memmem, 0)

static void
do_one_test (json_ctx_t *json_ctx, impl_t *impl, const void *haystack,
	     size_t haystack_len, const void *needle, size_t needle_len,
	     const void *expected)
{
  size_t i, iters = INNER_LOOP_ITERS_SMALL;
  timing_t start, stop, cur;
  void *res;
  TIMING_NOW (start);
  for (i = 0; i < iters; ++i)
    {
      res = CALL (impl, haystack, haystack_len, needle, needle_len);
    }
  TIMING_NOW (stop);

  TIMING_DIFF (cur, start, stop);

  json_element_double (json_ctx, (double) cur / (double) iters);

  if (res != expected)
    {
      error (0, 0, "Wrong result in function (%p != %p) %s(%p, %zu, %p, %zu)",
	     res, expected, impl->name, haystack, haystack_len, needle,
	     needle_len);
      ret = 1;
    }
}

static void
do_test (json_ctx_t *json_ctx, const char *str, size_t len, size_t idx)
{
  char tmpbuf[len];

  memcpy (tmpbuf, buf1 + idx, len);
  memcpy (buf1 + idx, str, len);

  json_element_object_begin (json_ctx);
  json_attr_uint (json_ctx, "len_haystack", BUF1PAGES * page_size);
  json_attr_uint (json_ctx, "len_needle", len);
  json_attr_uint (json_ctx, "haystack_ptr", (uintptr_t) buf1);
  json_attr_uint (json_ctx, "needle_ptr", (uintptr_t) str);
  json_attr_uint (json_ctx, "fail", 0);

  json_array_begin (json_ctx, "timings");

  FOR_EACH_IMPL (impl, 0)
    do_one_test (json_ctx, impl, buf1, BUF1PAGES * page_size, str, len,
		 buf1 + idx);

  memcpy (buf1 + idx, tmpbuf, len);

  json_array_end (json_ctx);
  json_element_object_end (json_ctx);
}

static void
do_random_tests (json_ctx_t *json_ctx)
{
  for (size_t n = 0; n < ITERATIONS; ++n)
    {
      char tmpbuf[32];

      size_t shift = random () % 11;
      size_t rel = random () % ((2 << (shift + 1)) * 64);
      size_t idx = MIN ((2 << shift) * 64 + rel, BUF1PAGES * page_size - 2);
      size_t len = random () % (sizeof (tmpbuf) - 1) + 1;
      len = MIN (len, BUF1PAGES * page_size - idx - 1);
      memcpy (tmpbuf, buf1 + idx, len);
      for (size_t i = random () % len / 2 + 1; i > 0; --i)
	{
	  size_t off = random () % len;
	  char ch = '0' + random () % 10;

	  buf1[idx + off] = ch;
	}

      json_element_object_begin (json_ctx);
      json_attr_uint (json_ctx, "len_haystack", BUF1PAGES * page_size);
      json_attr_uint (json_ctx, "len_needle", len);
      json_attr_uint (json_ctx, "haystack_ptr", (uintptr_t) buf1);
      json_attr_uint (json_ctx, "needle_ptr", (uintptr_t) (buf1 + idx));
      json_attr_uint (json_ctx, "fail", 0);

      json_array_begin (json_ctx, "timings");

      FOR_EACH_IMPL (impl, 0)
	do_one_test (json_ctx, impl, buf1, BUF1PAGES * page_size, buf1 + idx,
		     len, buf1 + idx);

      json_array_end (json_ctx);
      json_element_object_end (json_ctx);

      memcpy (buf1 + idx, tmpbuf, len);
    }
}

static const char *const strs[] =
  {
    "00000", "00112233", "0123456789", "0000111100001111",
    "00000111110000022222", "012345678901234567890",
    "abc0", "aaaa0", "abcabc0"
  };

int
test_main (void)
{
  json_ctx_t json_ctx;
  size_t i;

  test_init ();
  json_init (&json_ctx, 0, stdout);

  json_document_begin (&json_ctx);
  json_attr_string (&json_ctx, "timing_type", TIMING_TYPE);

  json_attr_object_begin (&json_ctx, "functions");
  json_attr_object_begin (&json_ctx, TEST_NAME);
  json_attr_string (&json_ctx, "bench-variant", "");

  json_array_begin (&json_ctx, "ifuncs");
  FOR_EACH_IMPL (impl, 0)
    json_element_string (&json_ctx, impl->name);
  json_array_end (&json_ctx);

  json_array_begin (&json_ctx, "results");

  for (i = 0; i < BUF1PAGES * page_size; ++i)
    buf1[i] = 60 + random () % 32;

  for (i = 0; i < sizeof (strs) / sizeof (strs[0]); ++i)
    for (size_t j = 0; j < 120; j += 7)
      {
	size_t len = strlen (strs[i]);

	do_test (&json_ctx, strs[i], len, j);
      }

  do_random_tests (&json_ctx);

  json_array_end (&json_ctx);
  json_attr_object_end (&json_ctx);
  json_attr_object_end (&json_ctx);
  json_document_end (&json_ctx);
  return ret;
}

#include <support/test-driver.c>
