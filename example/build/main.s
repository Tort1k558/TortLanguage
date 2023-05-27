	.text
	.def	@feat.00;
	.scl	3;
	.type	0;
	.endef
	.globl	@feat.00
.set @feat.00, 0
	.file	"main"
	.def	sum;
	.scl	2;
	.type	32;
	.endef
	.globl	sum
	.p2align	4, 0x90
sum:
.seh_proc sum
	pushq	%rax
	.seh_stackalloc 8
	.seh_endprologue
	movl	%ecx, 4(%rsp)
	movl	%edx, (%rsp)
	leal	(%rcx,%rdx), %eax
	popq	%rcx
	retq
	.seh_endproc

	.def	div;
	.scl	2;
	.type	32;
	.endef
	.globl	div
	.p2align	4, 0x90
div:
.seh_proc div
	subq	$16, %rsp
	.seh_stackalloc 16
	.seh_endprologue
	vmovsd	%xmm0, 8(%rsp)
	vmovsd	%xmm1, (%rsp)
	vdivsd	%xmm1, %xmm0, %xmm0
	addq	$16, %rsp
	retq
	.seh_endproc

	.def	main;
	.scl	2;
	.type	32;
	.endef
	.globl	main
	.p2align	4, 0x90
main:
.seh_proc main
	subq	$56, %rsp
	movb	$1, %al
	.seh_stackalloc 56
	.seh_endprologue
	movl	$2, 48(%rsp)
	movl	$1, 44(%rsp)
	testb	%al, %al
	jne	.LBB2_2
	cmpl	$-4, 44(%rsp)
	setne	%al
.LBB2_2:
	cmpl	$0, 48(%rsp)
	movb	%al, 52(%rsp)
	jle	.LBB2_4
	movl	48(%rsp), %edx
	leaq	.L__unnamed_1(%rip), %rcx
	jmp	.LBB2_11
.LBB2_4:
	cmpl	$98, 44(%rsp)
	jg	.LBB2_7
	leaq	.L__unnamed_2(%rip), %rcx
	movl	$10, %edx
	jmp	.LBB2_11
.LBB2_7:
	cmpl	$0, 48(%rsp)
	setne	%al
	je	.LBB2_8
	testb	%al, %al
	je	.LBB2_10
.LBB2_6:
	leaq	.L__unnamed_3(%rip), %rcx
	movl	$9, %edx
	jmp	.LBB2_11
.LBB2_8:
	cmpl	$0, 44(%rsp)
	setne	%al
	testb	%al, %al
	jne	.LBB2_6
.LBB2_10:
	leaq	.L__unnamed_4(%rip), %rcx
	movl	$99999, %edx
.LBB2_11:
	callq	printf
	leaq	.L__unnamed_5(%rip), %rcx
	leaq	.L__unnamed_6(%rip), %rdx
	callq	printf
	movl	52(%rsp), %edx
	leaq	.L__unnamed_7(%rip), %rcx
	callq	printf
	movl	52(%rsp), %eax
	addq	$56, %rsp
	retq
	.seh_endproc

	.section	.rdata,"dr"
.L__unnamed_1:
	.asciz	"%d\n"

.L__unnamed_2:
	.asciz	"%d\n"

.L__unnamed_3:
	.asciz	"%d\n"

.L__unnamed_4:
	.asciz	"%d\n"

.L__unnamed_5:
	.asciz	"%s"

.L__unnamed_6:
	.asciz	"true\n"

.L__unnamed_8:
	.asciz	"false\n"

.L__unnamed_7:
	.asciz	"%d\n"

	.globl	_fltused
