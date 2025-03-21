/* Copyright (C) 2002-2025 Free Software Foundation, Inc.
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

#include <pthreadP.h>
#include <shlib-compat.h>

int
___pthread_mutexattr_destroy (pthread_mutexattr_t *attr)
{
  return 0;
}
versioned_symbol (libc, ___pthread_mutexattr_destroy,
                  pthread_mutexattr_destroy, GLIBC_2_34);
#if OTHER_SHLIB_COMPAT (libpthread, GLIBC_2_0, GLIBC_2_34)
compat_symbol (libc, ___pthread_mutexattr_destroy,
               pthread_mutexattr_destroy, GLIBC_2_0);
compat_symbol (libc, ___pthread_mutexattr_destroy,
               __pthread_mutexattr_destroy, GLIBC_2_0);
#endif
