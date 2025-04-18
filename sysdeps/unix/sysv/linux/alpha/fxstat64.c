/* fxstat64 using old-style Unix stat system call.
   Copyright (C) 2004-2025 Free Software Foundation, Inc.
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
   License along with the GNU C Library.  If not, see
   <https://www.gnu.org/licenses/>.  */

#define __fxstat __redirect___fxstat64
#include <sys/stat.h>
#undef __fxstat
#include <kernel_stat.h>
#include <sysdep.h>
#include <xstatconv.h>

/* Get information about the file NAME in BUF.  */
int
__fxstat64 (int vers, int fd, struct stat64 *buf)
{
  switch (vers)
    {
    case _STAT_VER_KERNEL64:
      return INLINE_SYSCALL_CALL (fstat64, fd, buf);

    default:
      {
        struct kernel_stat kbuf;
	int r = INTERNAL_SYSCALL_CALL (fstat, fd, &kbuf);
	if (r == 0)
	  return __xstat_conv (vers, &kbuf, buf);
	return INLINE_SYSCALL_ERROR_RETURN_VALUE (-r);
      }
    }
}
strong_alias (__fxstat64, __fxstat);
