.global	pub_Random_next
.global	abs
.global	longDiv
.global	exp
.global	newRandom


.text

newRandom:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$32, %rsp
	movl	$4, %eax
	movl	%eax, %edi
	call	malloc
	movq	%rax, -8(%rbp)
	call	newTimes
	movq	%rax, -16(%rbp)
	movq	-16(%rbp), %rax
	movq	%rax, %rdi
	call	sys_times
	movq	-16(%rbp), %rax
	movl	(%rax), %eax
	movl	%eax, %ebx
	movl	%ebx, -20(%rbp)
	movl	-20(%rbp), %eax
	movl	%eax, %edi
	movl	$3, %eax
	movl	%eax, %esi
	call	exp
	movl	%eax, %ebx
	movl	%ebx, -20(%rbp)
	movl	$10, -24(%rbp)
	mov	-20(%rbp), %eax
	cltd
	idivl	-24(%rbp)
	movq	-8(%rbp), %rdx
	movl	%edx, %ebx
	movl	%ebx, 0(%rdx)
	movq	-8(%rbp), %rax
	leave
	ret
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
	idivl	-8(%rbp)
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
pub_Random_next:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$32, %rsp
	movq	%rdi, -8(%rbp)
	movl	%esi, -12(%rbp)
	call	newTimes
	movq	%rax, -20(%rbp)
	movq	-20(%rbp), %rax
	movq	%rax, %rdi
	call	sys_times
	movq	-20(%rbp), %rdx
	movq	0(%rdx), %rbx
	movq	%rbx, -28(%rbp)
	movq	-8(%rbp), %rdx
	movl	0(%rdx), %ebx
	movl	%ebx, -24(%rbp)
	mov	$1, %edx
	mov	-24(%rbp), %eax
	add	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -24(%rbp)
	call	abs
	mov	%eax, %edx
	mov	-28(%rbp), %eax
	add	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -24(%rbp)
	movl	-24(%rbp), %eax
	movl	%eax, %edi
	mov	-24(%rbp), %eax
	cltd
	idivl	-12(%rbp)
	movl	%edx, %eax
	movl	%eax, %esi
	call	exp
	movl	%eax, %ebx
	movl	%ebx, -24(%rbp)
	mov	-24(%rbp), %eax
	cltd
	idivl	-12(%rbp)
	movl	%edx, %ebx
	movl	%ebx, -24(%rbp)
	movq	-8(%rbp), %rdx
	movl	-24(%rbp), %ebx
	movl	%ebx, 0(%rdx)
	movl	-24(%rbp), %eax
	movl	%eax, %edi
	call	abs
	movl	%eax, %eax
	leave
	ret


.data



.bss

