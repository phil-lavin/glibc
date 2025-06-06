/* Link-time constants for ARM EABI - signal handlers.
   Copyright (C) 2005-2025 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   In addition to the permissions in the GNU Lesser General Public
   License, the Free Software Foundation gives you unlimited
   permission to link the compiled version of this file with other
   programs, and to distribute those programs without any restriction
   coming from the use of this file. (The GNU Lesser General Public
   License restrictions do apply in other respects; for example, they
   cover modification of the file, and distribution when not linked
   into another program.)

   Note that people who make modified versions of this file are not
   obligated to grant this special exception for their modified
   versions; it is their choice whether to do so. The GNU Lesser
   General Public License gives permission to release a modified
   version without this exception; this exception also makes it
   possible to release a modified version which carries forward this
   exception.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library.  If not, see
   <https://www.gnu.org/licenses/>.  */

/* The ARM EABI defines these as "functions".  */

#include <sysdep.h>

	.global __aeabi_SIG_DFL
	.hidden __aeabi_SIG_DFL
	.type __aeabi_SIG_DFL, %function
	.set __aeabi_SIG_DFL, 0

	.global __aeabi_SIG_IGN
	.hidden __aeabi_SIG_IGN
	.type __aeabi_SIG_IGN, %function
	.set __aeabi_SIG_IGN, 1

	.global __aeabi_SIG_ERR
	.hidden __aeabi_SIG_ERR
	.type __aeabi_SIG_ERR, %function
	.set __aeabi_SIG_ERR, -1
