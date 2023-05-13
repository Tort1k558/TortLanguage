	.text
	.def	@feat.00;
	.scl	3;
	.type	0;
	.endef
	.globl	@feat.00
.set @feat.00, 0
	.file	"Tort"
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
	.globl	__real@4014000000000000
	.section	.rdata,"dr",discard,__real@4014000000000000
	.p2align	3, 0x0
__real@4014000000000000:
	.quad	0x4014000000000000
	.globl	__real@4000000000000000
	.section	.rdata,"dr",discard,__real@4000000000000000
	.p2align	3, 0x0
__real@4000000000000000:
	.quad	0x4000000000000000
	.text
	.globl	main
	.p2align	4, 0x90
main:
.seh_proc main
	subq	$56, %rsp
	leaq	.L__unnamed_1(%rip), %rcx
	movl	$10, %edx
	.seh_stackalloc 56
	.seh_endprologue
	movl	$10, 44(%rsp)
	movl	$5, 40(%rsp)
	callq	printf
	movl	40(%rsp), %edx
	leaq	.L__unnamed_2(%rip), %rcx
	callq	printf
	movl	$5, %ecx
	movl	$8, %edx
	callq	sum
	leal	(%rax,%rax,4), %edx
	leaq	.L__unnamed_3(%rip), %rcx
	movl	%eax, 40(%rsp)
	addl	%edx, %edx
	addl	44(%rsp), %edx
	callq	printf
	vmovq	__real@4014000000000000(%rip), %xmm0
	vmovsd	__real@4000000000000000(%rip), %xmm1
	callq	div
	vmovq	%xmm0, %rdx
	leaq	.L__unnamed_4(%rip), %rcx
	vmovdqa	%xmm0, %xmm1
	vmovq	%xmm0, 48(%rsp)
	callq	printf
	vmovsd	48(%rsp), %xmm0
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
	.asciz	"%f\n"

	.globl	_fltused
