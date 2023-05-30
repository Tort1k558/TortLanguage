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

	.def	fact;
	.scl	2;
	.type	32;
	.endef
	.globl	fact
	.p2align	4, 0x90
fact:
.seh_proc fact
	subq	$40, %rsp
	.seh_stackalloc 40
	.seh_endprologue
	movl	%ecx, 32(%rsp)
	testl	%ecx, %ecx
	sete	%al
	je	.LBB1_2
	cmpl	$1, 32(%rsp)
	sete	%al
.LBB1_2:
	testb	%al, %al
	je	.LBB1_4
	movl	$1, 36(%rsp)
	jmp	.LBB1_5
.LBB1_4:
	movl	32(%rsp), %ecx
	decl	%ecx
	callq	fact
	imull	32(%rsp), %eax
	movl	%eax, 36(%rsp)
.LBB1_5:
	movl	36(%rsp), %eax
	addq	$40, %rsp
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
	jne	.LBB3_2
	cmpl	$0, 44(%rsp)
	setne	%al
.LBB3_2:
	movzbl	%al, %edx
	leaq	.L__unnamed_1(%rip), %rcx
	orl	$4, %edx
	movl	%edx, 52(%rsp)
	callq	printf
	cmpl	$0, 48(%rsp)
	jg	.LBB3_9
	cmpl	$99, 44(%rsp)
	jl	.LBB3_9
	cmpl	$0, 48(%rsp)
	setne	%al
	je	.LBB3_5
	testb	%al, %al
	je	.LBB3_7
.LBB3_10:
	leaq	.L__unnamed_2(%rip), %rcx
	movl	$9, %edx
	jmp	.LBB3_8
.LBB3_5:
	cmpl	$0, 44(%rsp)
	setne	%al
	testb	%al, %al
	jne	.LBB3_10
.LBB3_7:
	leaq	.L__unnamed_3(%rip), %rcx
	movl	$99999, %edx
.LBB3_8:
	callq	printf
.LBB3_9:
	movl	$5, %ecx
	callq	fact
	leaq	.L__unnamed_4(%rip), %rcx
	movl	%eax, %edx
	callq	printf
	leaq	.L__unnamed_5(%rip), %rcx
	movl	$3, %edx
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
	.asciz	"%d\n"

	.globl	_fltused
