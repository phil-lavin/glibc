/* Set current context.

   Copyright (C) 2009-2025 Free Software Foundation, Inc.

   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public License as
   published by the Free Software Foundation; either version 2.1 of the
   License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, see
   <https://www.gnu.org/licenses/>.  */

#include <sysdep.h>
#include "ucontext_i.h"
#include "ucontext-internal.h"

/*  int __setcontext (const ucontext_t *ucp)

  Restores the machine context in UCP and thereby resumes execution
  in that context.

  This implementation is intended to be used for *synchronous* context
  switches only.  Therefore, it does not have to restore anything
  other than the PRESERVED state.  */

	.text

ENTRY (__setcontext)
	/* Save a copy of UCP.  */
	mov	x9, x0

	/* Set the signal mask with
	   rt_sigprocmask (SIG_SETMASK, mask, NULL, _NSIG/8).  */
	mov	x0, #SIG_SETMASK
	add	x1, x9, #UCONTEXT_SIGMASK
	mov	x2, #0
	mov	x3, #_NSIG8
	mov	x8, SYS_ify (rt_sigprocmask)
	svc	0
	cbz	x0, 1f
	b	C_SYMBOL_NAME (__syscall_error)
1:
	/* Clear ZA state of SME.  */
	/* The calling convention of __libc_arm_za_disable allows to do
	   this thus allowing to avoid saving to and reading from stack.
	   As a result we also don't need to sign the return address and
	   check it after returning because it is not stored to stack.  */
	mov	x13, x30
	cfi_register (x30, x13)
	bl	__libc_arm_za_disable
	mov	x30, x13
	cfi_register (x13, x30)
	/* Restore the general purpose registers.  */
	mov	x0, x9
	cfi_def_cfa (x0, 0)
	cfi_offset (x18, oX0 + 18 * SZREG)
	cfi_offset (x19, oX0 + 19 * SZREG)
	cfi_offset (x20, oX0 + 20 * SZREG)
	cfi_offset (x21, oX0 + 21 * SZREG)
	cfi_offset (x22, oX0 + 22 * SZREG)
	cfi_offset (x23, oX0 + 23 * SZREG)
	cfi_offset (x24, oX0 + 24 * SZREG)
	cfi_offset (x25, oX0 + 25 * SZREG)
	cfi_offset (x26, oX0 + 26 * SZREG)
	cfi_offset (x27, oX0 + 27 * SZREG)
	cfi_offset (x28, oX0 + 28 * SZREG)
	cfi_offset (x29, oX0 + 29 * SZREG)
	cfi_offset (x30, oX0 + 30 * SZREG)

	cfi_offset ( d8, oV0 + 8 * SZVREG)
	cfi_offset ( d9, oV0 + 9 * SZVREG)
	cfi_offset (d10, oV0 + 10 * SZVREG)
	cfi_offset (d11, oV0 + 11 * SZVREG)
	cfi_offset (d12, oV0 + 12 * SZVREG)
	cfi_offset (d13, oV0 + 13 * SZVREG)
	cfi_offset (d14, oV0 + 14 * SZVREG)
	cfi_offset (d15, oV0 + 15 * SZVREG)
	ldp	x18, x19, [x0, oX0 + 18 * SZREG]
	ldp	x20, x21, [x0, oX0 + 20 * SZREG]
	ldp	x22, x23, [x0, oX0 + 22 * SZREG]
	ldp	x24, x25, [x0, oX0 + 24 * SZREG]
	ldp	x26, x27, [x0, oX0 + 26 * SZREG]
	ldp	x28, x29, [x0, oX0 + 28 * SZREG]
	ldr     x30,      [x0, oX0 + 30 * SZREG]
	ldr     x2, [x0, oSP]
	mov	sp, x2

	/* Check for FP SIMD context.  We don't support restoring
	   contexts created by the kernel, so this context must have
	   been created by getcontext.  Hence we can rely on the
	   first extension block being the FP SIMD context.  */
	add     x2, x0, #oEXTENSION

	mov	w3, #(FPSIMD_MAGIC & 0xffff)
	movk	w3, #(FPSIMD_MAGIC >> 16), lsl #16
	ldr	w1, [x2, #oHEAD + oMAGIC]
	cmp	w1, w3
	b.ne	2f

	/* Restore the FP SIMD context.  */
	add	x3, x2, #oV0 + 8 * SZVREG
	ldp	 q8,  q9, [x3], #2 * SZVREG
	ldp	q10, q11, [x3], #2 * SZVREG
	ldp	q12, q13, [x3], #2 * SZVREG
	ldp	q14, q15, [x3], #2 * SZVREG

	add	x3, x2, oFPSR

	ldr	w4, [x3]
	msr	fpsr, x4

	ldr	w4, [x3, oFPCR - oFPSR]
	msr	fpcr, x4

	/* Restore the GCS.  */
	mov	x16, 1 /* _CHKFEAT_GCS */
	CHKFEAT_X16
	tbnz	x16, 0, L(gcs_done)
	/* Get target GCS from GCS context.  */
	ldr	w1, [x2, #oHEAD + oSIZE]
	add	x2, x2, x1
	mov	w3, #(GCS_MAGIC & 0xffff)
	movk	w3, #(GCS_MAGIC >> 16), lsl #16
	ldr	w1, [x2, #oHEAD + oMAGIC]
	cmp	w1, w3
	b.ne	L(gcs_done)
	ldr	x3, [x2, #oGCSPR]
	MRS_GCSPR (x2)
	mov	x4, x3
	/* x2: GCSPR now.  x3, x4: target GCSPR.  x5, x6: tmp regs.  */
L(gcs_scan):
	cmp	x2, x4
	b.eq	L(gcs_pop)
	sub	x4, x4, 8
	/* Check for a cap token.  */
	ldr	x5, [x4]
	and	x6, x4, 0xfffffffffffff000
	orr	x6, x6, 1
	cmp	x5, x6
	b.ne	L(gcs_scan)
L(gcs_switch):
	add	x2, x4, 8
	GCSSS1 (x4)
	GCSSS2 (xzr)
L(gcs_pop):
	cmp	x2, x3
	b.eq	L(gcs_done)
	GCSPOPM (xzr)
	add	x2, x2, 8
	b	L(gcs_pop)
L(gcs_done):

2:
	ldr     x16, [x0, oPC]
	/* Restore arg registers.  */
	ldp	x2, x3, [x0, oX0 + 2 * SZREG]
	ldp	x4, x5, [x0, oX0 + 4 * SZREG]
	ldp	x6, x7, [x0, oX0 + 6 * SZREG]
	ldp	x0, x1, [x0, oX0 + 0 * SZREG]
	/* Jump to the new pc value.  */
	br	x16
PSEUDO_END (__setcontext)
weak_alias (__setcontext, setcontext)

/* makecontext start function: receives uc_link in x19 and func in x20.
   Arguments of func, x29, x30 and sp are set up by the caller.  */
ENTRY (__startcontext)
	cfi_undefined (x30)
	blr	x20
	mov	x0, x19
	cbnz	x0, __setcontext
1:	b       HIDDEN_JUMPTARGET (exit)
END (__startcontext)
