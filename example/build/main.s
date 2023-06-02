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
	.globl	__real@4024000000000000
	.section	.rdata,"dr",discard,__real@4024000000000000
	.p2align	3, 0x0
__real@4024000000000000:
	.quad	0x4024000000000000
	.globl	__real@4014000000000000
	.section	.rdata,"dr",discard,__real@4014000000000000
	.p2align	3, 0x0
__real@4014000000000000:
	.quad	0x4014000000000000
	.text
	.globl	main
	.p2align	4, 0x90
main:
.seh_proc main
	pushq	%rbp
	.seh_pushreg %rbp
	pushq	%rsi
	.seh_pushreg %rsi
	subq	$24, %rsp
	leaq	16(%rsp), %rbp
	movb	$1, %al
	.seh_stackalloc 24
	.seh_setframe %rbp, 16
	.seh_endprologue
	movl	$2, -4(%rbp)
	movl	$-1, 4(%rbp)
	testb	%al, %al
	jne	.LBB3_2
	cmpl	$0, 4(%rbp)
	setne	%al
.LBB3_2:
	movzbl	%al, %edx
	orl	$4, %edx
	movl	%edx, (%rbp)
	subq	$32, %rsp
	leaq	.L__unnamed_1(%rip), %rcx
	callq	printf
	addq	$32, %rsp
	cmpl	$0, -4(%rbp)
	js	.LBB3_8
	cmpl	$98, 4(%rbp)
	jg	.LBB3_7
	leaq	.L__unnamed_2(%rip), %rsi
	cmpl	$4, 4(%rbp)
	jg	.LBB3_8
	.p2align	4, 0x90
.LBB3_6:
	movl	4(%rbp), %edx
	subq	$32, %rsp
	movq	%rsi, %rcx
	callq	printf
	addq	$32, %rsp
	incl	4(%rbp)
	cmpl	$4, 4(%rbp)
	jle	.LBB3_6
	jmp	.LBB3_8
.LBB3_7:
	subq	$32, %rsp
	leaq	.L__unnamed_3(%rip), %rcx
	xorl	%edx, %edx
	callq	printf
	addq	$32, %rsp
.LBB3_8:
	subq	$32, %rsp
	movl	$6, %ecx
	callq	fact
	leaq	.L__unnamed_4(%rip), %rcx
	movl	%eax, %edx
	callq	printf
	leaq	.L__unnamed_5(%rip), %rcx
	movl	$3, %edx
	callq	printf
	vmovq	__real@4024000000000000(%rip), %xmm0
	vmovsd	__real@4014000000000000(%rip), %xmm1
	callq	pow
	vmovq	%xmm0, %rdx
	leaq	.L__unnamed_6(%rip), %rcx
	vmovdqa	%xmm0, %xmm1
	callq	printf
	leaq	.L__unnamed_7(%rip), %rcx
	leaq	.L.str(%rip), %rdx
	callq	printf
	addq	$32, %rsp
	movl	$16, %eax
	callq	__chkstk
	subq	%rax, %rsp
	movq	%rsp, %rax
	movabsq	$4607182418800017408, %rcx
	movq	%rcx, (%rax)
	movl	(%rbp), %eax
	leaq	8(%rbp), %rsp
	popq	%rsi
	popq	%rbp
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

.L__unnamed_6:
	.asciz	"%f\n"

.L.str:
	.asciz	"Hello World!"

.L__unnamed_7:
	.asciz	"%s"

	.globl	_fltused
