# Copyright (C) 1991-2025 Free Software Foundation, Inc.
# This file is part of the GNU C Library.

# The GNU C Library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.

# The GNU C Library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.

# You should have received a copy of the GNU Lesser General Public
# License along with the GNU C Library; if not, see
# <https://www.gnu.org/licenses/>.

#
#	Makefile for the terminal I/O functions.
#
subdir	:= termios

include ../Makeconfig

headers	:= termios.h bits/termios.h bits/termios-baud.h \
	   sys/ttydefaults.h sys/termios.h sys/ttychars.h

routines	:= speed cfsetspeed tcsetattr tcgetattr tcgetpgrp tcsetpgrp \
		   tcdrain tcflow tcflush tcsendbrk cfmakeraw tcgetsid \
		   baud cfsetbaud

include ../Rules

CFLAGS-tcdrain.c += -fexceptions -fasynchronous-unwind-tables
CFLAGS-tcsetattr.c += $(config-cflags-wno-ignored-attributes)
