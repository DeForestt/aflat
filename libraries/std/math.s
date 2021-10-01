.global	exp


.text

exp:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$16, %rsp
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	movl	$0, -12(%rbp)
	movl	$1, -16(%rbp)
	jmp	.Lexp1
.Lexp0:
	movl	-16(%rbp), %edx
	movl	-4(%rbp), %eax
	imul	%edx, %eax
	movl	%eax, -16(%rbp)
	mov	-12(%rbp), %edx
	mov	$1, %eax
	add	%edx, %eax
	movl	%eax, -12(%rbp)
.Lexp1:
	movl	-12(%rbp), %eax
	movl	-8(%rbp), %ecx
	cmpl	%ecx, %eax
	jl	.Lexp0
	movl	-16(%rbp), %eax
	leave
	ret
