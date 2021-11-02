.global	rand
.global	abs
.global	longDiv
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
	mov	-4(%rbp), %edx
	mov	-16(%rbp), %eax
	imul	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -16(%rbp)
	mov	$1, %edx
	mov	-12(%rbp), %eax
	add	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -12(%rbp)
.Lexp1:
	movl	-12(%rbp), %eax
	movl	-8(%rbp), %ecx
	cmpl	%ecx, %eax
	jl	.Lexp0
	movl	-16(%rbp), %eax
	leave
	ret
longDiv:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$16, %rsp
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	mov	-4(%rbp), %eax
	cltd
	idivl	-4(%rbp)
	movl	%edx, -12(%rbp)
	mov	$10, %edx
	mov	-12(%rbp), %eax
	imul	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -12(%rbp)
	mov	-12(%rbp), %eax
	cltd
	idivl	-8(%rbp)
	movl	%eax, %ebx
	movl	%ebx, -12(%rbp)
	movl	-12(%rbp), %eax
	leave
	ret
abs:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$16, %rsp
	movl	%edi, -4(%rbp)
	movl	-4(%rbp), %eax
	movl	$0, %ecx
	cmpl	%ecx, %eax
	jge	.Labs2
	mov	$-1, %edx
	mov	-4(%rbp), %eax
	imul	%edx, %eax
	movl	%eax, %eax
	leave
	ret
.Labs2:
	movl	-4(%rbp), %eax
	leave
	ret
rand:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$16, %rsp
	movl	%edi, -4(%rbp)
	movq	$0, %rax
	movq	%rax, %rdi
	call	newTime
	movq	%rax, -12(%rbp)
	movq	-12(%rbp), %rax
	movq	%rax, %rdi
	call	sys_time
	movq	-12(%rbp), %rax
	movl	(%rax), %eax
	movl	%eax, -16(%rbp)
	movl	-16(%rbp), %eax
	movl	%eax, %edi
	movl	$3, %eax
	movl	%eax, %esi
	call	exp
	movl	%eax, %ebx
	movl	%ebx, -16(%rbp)
	mov	-16(%rbp), %eax
	cltd
	idivl	-16(%rbp)
	movl	%edx, %ebx
	movl	%ebx, -16(%rbp)
	movl	-16(%rbp), %eax
	leave
	ret


.data



.bss

