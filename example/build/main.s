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
	subq	$40, %rsp
	leaq	.L__unnamed_1(%rip), %rdx
	leaq	.L__unnamed_2(%rip), %rcx
	.seh_stackalloc 40
	.seh_endprologue
	movb	$0, 39(%rsp)
	callq	printf
	leaq	.L__unnamed_3(%rip), %rdx
	leaq	.L__unnamed_4(%rip), %rcx
	movb	$0, 39(%rsp)
	callq	printf
	leaq	.L__unnamed_5(%rip), %rcx
	movl	$2, %edx
	callq	printf
	leaq	.L__unnamed_6(%rip), %rcx
	movl	$55555, %edx
	callq	printf
	xorl	%eax, %eax
	addq	$40, %rsp
	retq
	.seh_endproc

	.section	.rdata,"dr"
.L__unnamed_2:
	.asciz	"%s"

.L__unnamed_7:
	.asciz	"true\n"

.L__unnamed_1:
	.asciz	"false\n"

.L__unnamed_4:
	.asciz	"%s"

.L__unnamed_8:
	.asciz	"true\n"

.L__unnamed_3:
	.asciz	"false\n"

.L__unnamed_9:
	.asciz	"%d\n"

.L__unnamed_5:
	.asciz	"%d\n"

.L__unnamed_10:
	.asciz	"%d\n"

.L__unnamed_6:
	.asciz	"%d\n"

	.globl	_fltused
