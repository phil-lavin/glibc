ifeq ($(subdir),math)
# sqrtf128 requires soft-fp.
CPPFLAGS += -I../soft-fp

libm-support += powl_helper
tests += \
  test-fenv-clear-sse \
  test-fenv-sse \
  test-fenv-sse-2 \
  test-fenv-x87 \
  test-flt-eval-method-387 \
  test-flt-eval-method-sse \
# tests
CFLAGS-test-fenv-sse.c += -msse2 -mfpmath=sse
CFLAGS-test-fenv-clear-sse.c += -msse2 -mfpmath=sse
CFLAGS-test-fenv-sse-2.c += -msse2 -mfpmath=sse
CFLAGS-test-flt-eval-method-387.c += -fexcess-precision=standard -mfpmath=387
CFLAGS-test-flt-eval-method-sse.c += -fexcess-precision=standard -msse2 \
				     -mfpmath=sse
endif
