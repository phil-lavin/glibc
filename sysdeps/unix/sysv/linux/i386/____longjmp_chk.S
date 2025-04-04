/* Copyright (C) 2001-2025 Free Software Foundation, Inc.
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

#include <sysdep.h>
#include <pointer_guard.h>
#include <jmpbuf-offsets.h>
#include <asm-syntax.h>
#include <stap-probe.h>


	.section .rodata.str1.1,"aMS",@progbits,1
	.type	longjmp_msg,@object
longjmp_msg:
	.string "longjmp causes uninitialized stack frame"
	.size	longjmp_msg, .-longjmp_msg


#ifdef PIC
# define CALL_FAIL	movl	%ebx, %ecx;				      \
			cfi_register(%ebx,%ecx);			      \
			LOAD_PIC_REG (bx);				      \
			leal	longjmp_msg@GOTOFF(%ebx), %eax;		      \
			movl	%eax, (%esp);				      \
			call	HIDDEN_JUMPTARGET(__fortify_fail)
#else
# define CALL_FAIL	movl	$longjmp_msg, %eax;			      \
			movl	%eax, (%esp);				      \
			call	HIDDEN_JUMPTARGET(__fortify_fail)
#endif


	.text
ENTRY (____longjmp_chk)
	movl	4(%esp), %ecx	/* User's jmp_buf in %ecx.  */

	/* Save the return address now.  */
	movl	(JB_PC*4)(%ecx), %edx
	/* Get the stack pointer.  */
	movl	(JB_SP*4)(%ecx), %edi
	cfi_undefined(%edi)
	PTR_DEMANGLE (%edx)
	PTR_DEMANGLE (%edi)

	cmpl	%edi, %esp
	jbe	.Lok

	subl	$12, %esp
	cfi_adjust_cfa_offset(12)
	xorl	%ebx, %ebx
	movl	%esp, %ecx
	movl	$__NR_sigaltstack, %eax
	ENTER_KERNEL
	/* Without working sigaltstack we cannot perform the test.  */
	test	%eax, %eax
	jne	.Lok2
	testl	$1, 4(%esp)
	jz	.Lfail

	movl	(%esp), %eax
	addl	8(%esp), %eax
	subl	%edi, %eax
	cmpl	8(%esp), %eax
	jae	.Lok2

.Lfail:	CALL_FAIL

.Lok2:	addl	$12, %esp
	cfi_adjust_cfa_offset(-12)
	movl	4(%esp), %ecx

.Lok:
	LIBC_PROBE (longjmp, 3, 4@%ecx, -4@8(%esp), 4@%edx)
	/* We add unwind information for the target here.  */
	cfi_def_cfa(%ecx, 0)
	cfi_register(%eip, %edx)
	cfi_register(%esp, %edi)
	cfi_offset(%ebx, JB_BX*4)
	cfi_offset(%esi, JB_SI*4)
	cfi_offset(%edi, JB_DI*4)
	cfi_offset(%ebp, JB_BP*4)

	movl	8(%esp), %eax	/* Second argument is return value.  */
	movl	%edi, %esp

	/* Restore registers.  */
	movl	(JB_BX*4)(%ecx), %ebx
	movl	(JB_SI*4)(%ecx), %esi
	movl	(JB_DI*4)(%ecx), %edi
	movl	(JB_BP*4)(%ecx), %ebp
	cfi_restore(%ebx)
	cfi_restore(%esi)
	cfi_restore(%edi)
	cfi_restore(%ebp)

	/* Jump to saved PC.  */
	LIBC_PROBE (longjmp_target, 3, 4@%ecx, -4@%eax, 4@%edx)
	jmp	*%edx
END (____longjmp_chk)
