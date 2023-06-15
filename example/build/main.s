	.text
	.def	@feat.00;
	.scl	3;
	.type	0;
	.endef
	.globl	@feat.00
.set @feat.00, 0
	.file	"main"
	.def	main;
	.scl	2;
	.type	32;
	.endef
	.globl	main
	.p2align	4, 0x90
main:
.seh_proc main
	pushq	%rbp
	.seh_pushreg %rbp
	subq	$16, %rsp
	movl	$192, %eax
	leaq	16(%rsp), %rbp
	.seh_stackalloc 16
	.seh_setframe %rbp, 16
	.seh_endprologue
	movq	%rsp, %rcx
	movl	$5, -8(%rbp)
	callq	__chkstk
	subq	%rax, %rsp
	cmpl	$5, -8(%rbp)
	movq	%rsp, %rax
	jne	.LBB0_2
	movl	$10, -4(%rbp)
	jmp	.LBB0_3
.LBB0_2:
	movl	104(%rax), %eax
	movl	%eax, -4(%rbp)
.LBB0_3:
	movq	%rcx, %rsp
	movl	-4(%rbp), %eax
	movq	%rbp, %rsp
	popq	%rbp
	retq
	.seh_endproc

