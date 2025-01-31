/* Clean up allocated libpthread memory on demand.
   Copyright (C) 2018-2025 Free Software Foundation, Inc.
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

#include <libc-symbols.h>
#include <pthreadP.h>
#include <nptl-stack.h>

/* Free libpthread.so resources.
   Note: Caller ensures we are called only once.  */
void
__libpthread_freeres (void)
{
  call_function_static_weak (__default_pthread_attr_freeres);
  __nptl_free_stacks (0);
}
