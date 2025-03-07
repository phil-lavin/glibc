/* Return complex conjugate of complex float value.
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

#define __conjf __conjf_not_defined
#define conjf conjf_not_defined

#include <complex.h>
#include "cfloat-compat.h"

#undef __conjf
#undef conjf

c1_cfloat_rettype
__c1_conjf (c1_cfloat_decl (z))
{
  _Complex float r = ~ c1_cfloat_value (z);
  return c1_cfloat_return (r);
}

c2_cfloat_rettype
__c2_conjf (c2_cfloat_decl (z))
{
  _Complex float r = ~ c2_cfloat_value (z);
  return c2_cfloat_return (r);
}

cfloat_versions (conj);
