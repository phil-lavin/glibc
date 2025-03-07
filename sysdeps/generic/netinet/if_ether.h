/* Copyright (C) 1996-2025 Free Software Foundation, Inc.
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

#ifndef __NETINET_IF_ETHER_H
#define __NETINET_IF_ETHER_H	1

#include <sys/types.h>
#include <stdint.h>

#define ETH_ALEN	6	/* Octets in one ethernet address.  */

/* This is a name for the 48 bit ethernet address available on many
   systems.  */
struct ether_addr
{
  uint8_t ether_addr_octet[ETH_ALEN];
} __attribute__ ((__packed__));

#endif /* netinet/if_ether.h */
