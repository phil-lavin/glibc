/* PowerPC32 default implementation of strchrnul.
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

#include <string.h>

#define STRCHRNUL  __strchrnul_ppc
extern __typeof (strchrnul) __strchrnul_ppc attribute_hidden;

#include <string/strchrnul.c>
#undef __strchrnul
weak_alias (__strchrnul_ppc, __strchrnul)
#ifdef SHARED
__hidden_ver1 (__strchrnul_ppc, __GI___strchrnul, __strchrnul_ppc);
#else
weak_alias (__strchrnul_ppc, strchrnul)
#endif
