/* Get file status.  Linux/MIPSn64 version.
   Copyright (C) 2022-2025 Free Software Foundation, Inc.
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

#include <sys/stat.h>
#include <sysdep.h>

/* Different than other ABIs, mips64 has different layouts for non-LFS
   and LFS struct stat.  */
int
__fstatat (int fd, const char *file, struct stat *buf, int flag)
{
  struct __stat64_t64 st64;
  int r = __fstatat64_time64 (fd, file, &st64, flag);
  if (r == 0)
    {
      /* Clear internal pad and reserved fields.  */
      memset (buf, 0, sizeof (*buf));

      buf->st_dev = st64.st_dev;
      buf->st_ino = st64.st_ino;
      buf->st_mode = st64.st_mode;
      buf->st_nlink = st64.st_nlink;
      buf->st_uid = st64.st_uid;
      buf->st_gid = st64.st_gid;
      buf->st_rdev = st64.st_rdev;
      buf->st_size = st64.st_size;
      buf->st_blksize = st64.st_blksize;
      buf->st_blocks  = st64.st_blocks;
      buf->st_atim = st64.st_atim;
      buf->st_mtim = st64.st_mtim;
      buf->st_ctim = st64.st_ctim;
    }
  return r;
}

weak_alias (__fstatat, fstatat)
