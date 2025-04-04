/* Copyright (C) 2009-2025 Free Software Foundation, Inc.
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

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef __GT_FILE
# define __GT_FILE 0
#endif

/* Generate a unique temporary file name from TEMPLATE.  The last six
   characters before a suffix of length SUFFIXLEN of TEMPLATE must be
   "XXXXXX"; they are replaced with a string that makes the filename
   unique.  Then open the file and return a fd. */
int
mkstemps (char *template, int suffixlen)
{
  if (suffixlen < 0)
    {
      __set_errno (EINVAL);
      return -1;
    }

  return __gen_tempname (template, suffixlen, 0, __GT_FILE);
}

#if !defined O_LARGEFILE || O_LARGEFILE == 0
weak_alias (mkstemps, mkstemps64)
#endif
