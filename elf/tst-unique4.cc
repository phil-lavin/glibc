// BZ 12511
#include "tst-unique4.h"

#include <cstdio>
#include <libc-diag.h>

/* clang warns that the instantiation of the variable is required, but no
   definition is available.  They are implemented on tst-unique4lib.so.  */
DIAG_PUSH_NEEDS_COMMENT_CLANG;
DIAG_IGNORE_NEEDS_COMMENT_CLANG (13, "-Wundefined-var-template");
static int a[24] =
  {
    S<1>::i, S<2>::i, S<3>::i, S<4>::i, S<5>::i, S<6>::i, S<7>::i, S<8>::i,
    S<9>::i, S<10>::i, S<11>::i, S<12>::i, S<13>::i, S<14>::i, S<15>::i,
    S<16>::i, S<17>::i, S<18>::i, S<19>::i, S<20>::i, S<21>::i, S<22>::i,
    S<23>::i, S<24>::i
  };
DIAG_POP_NEEDS_COMMENT_CLANG;

int
main (void)
{
  int result = 0;
  for (int i = 0; i < 24; ++i)
    {
      printf("%d ", a[i]);
      result |= a[i] != i + 1;
    }

  printf("\n%d\n", S<1>::j);
  result |= S<1>::j != -1;

  return result;
}
