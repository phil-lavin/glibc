/* Copyright (C) 1991-2025 Free Software Foundation, Inc.
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
#include <signal.h>

#include <sigset-cvt-mask.h>

/* Set the mask of blocked signals to MASK, returning the old mask.  */
int
__sigsetmask (int mask)
{
  sigset_t set, oset;

  if (sigset_set_old_mask (&set, mask) < 0)
    return -1;

  if (__sigprocmask (SIG_SETMASK, &set, &oset) < 0)
    return -1;

  return sigset_get_old_mask (&oset);
}

weak_alias (__sigsetmask, sigsetmask)
