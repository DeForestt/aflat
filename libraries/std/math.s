.global	pub_Random_next
.global	abs
.global	longDiv
.global	longExp
.global	exp
.global	newRandom


.text

newRandom:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$32, %rsp
	pushq	%rdx
	pushq	%rdi
	movl	$4, %eax
	movl	%eax, %edi
	call	malloc
	popq	%rdi
	popq	%rdx
	movq	%rax, %rbx
	movq	%rbx, -8(%rbp)
	pushq	%rdx
	call	newTimes
	popq	%rdx
	movq	%rax, %rbx
	movq	%rbx, -16(%rbp)
	pushq	%rdx
	pushq	%rdi
	movq	-16(%rbp), %rax
	movq	%rax, %rdi
	call	sys_times
	popq	%rdi
	popq	%rdx
	movq	-16(%rbp), %rax
	movl	(%rax), %eax
	movl	%eax, %ebx
	movl	%ebx, -20(%rbp)
	pushq	%rdx
	pushq	%rdi
	movl	-20(%rbp), %eax
	movl	%eax, %edi
	pushq	%rsi
	movl	$3, %eax
	movl	%eax, %esi
	call	exp
	popq	%rsi
	popq	%rdi
	popq	%rdx
	movl	%eax, %ebx
	movl	%ebx, -20(%rbp)
	movl	$10, %ebx
	movl	%ebx, -24(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	-20(%rbp), %eax
	movl	-24(%rbp), %ecx
	cltd
	idiv	%ecx
	movl	%edx, %eax
	popq	%rdx
	popq	%rdi
	movq	-8(%rbp), %rdx
	movl	%eax, %ebx
	movl	%ebx, 0(%rdx)
	movq	-8(%rbp), %rax
	leave
	ret
exp:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$16, %rsp
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	movl	$0, %ebx
	movl	%ebx, -12(%rbp)
	movl	$1, %ebx
	movl	%ebx, -16(%rbp)
	jmp	.Lexp1
.Lexp0:
	pushq	%rdi
	pushq	%rdx
	movl	-4(%rbp), %edx
	movl	-16(%rbp), %edi
	imul	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -16(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movl	-12(%rbp), %edi
	add	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -12(%rbp)
.Lexp1:
	pushq	%rdi
	pushq	%rdx
	movl	-8(%rbp), %edx
	movl	-12(%rbp), %edi
	cmpl	%edx, %edi
	setl	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$1, %al
	je	.Lexp0
	movl	-16(%rbp), %eax
	leave
	ret
longExp:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$32, %rsp
	movq	%rdi, -8(%rbp)
	movl	%esi, -12(%rbp)
	movl	$0, %ebx
	movl	%ebx, -16(%rbp)
	movq	$1, %rbx
	movq	%rbx, -24(%rbp)
	jmp	.LlongExp3
.LlongExp2:
	pushq	%rdi
	pushq	%rdx
	movq	-8(%rbp), %rdx
	movq	-24(%rbp), %rdi
	imul	%rdx, %rdi
	movq	%rdi, %rax
	popq	%rdx
	popq	%rdi
	movq	%rax, %rbx
	movq	%rbx, -24(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movl	-16(%rbp), %edi
	add	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -16(%rbp)
.LlongExp3:
	pushq	%rdi
	pushq	%rdx
	movl	-12(%rbp), %edx
	movl	-16(%rbp), %edi
	cmpl	%edx, %edi
	setl	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$1, %al
	je	.LlongExp2
	movq	-24(%rbp), %rax
	leave
	ret
longDiv:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$16, %rsp
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	-4(%rbp), %eax
	movl	-8(%rbp), %ecx
	cltd
	idiv	%ecx
	movl	%edx, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -12(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	$10, %edx
	movl	-12(%rbp), %edi
	imul	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -12(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	-12(%rbp), %eax
	movl	-8(%rbp), %ecx
	cltd
	idiv	%ecx
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -12(%rbp)
	movl	-12(%rbp), %eax
	leave
	ret
abs:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$16, %rsp
	movl	%edi, -4(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	$0, %edx
	movl	-4(%rbp), %edi
	cmpl	%edx, %edi
	setl	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.Labs4
	pushq	%rdi
	pushq	%rdx
	movl	$-1, %edx
	movl	-4(%rbp), %edi
	imul	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %eax
	leave
	ret
.Labs4:
	movl	-4(%rbp), %eax
	leave
	ret
pub_Random_next:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$32, %rsp
	movq	%rdi, -8(%rbp)
	movl	%esi, -12(%rbp)
	pushq	%rdx
	call	newTimes
	popq	%rdx
	movq	%rax, %rbx
	movq	%rbx, -20(%rbp)
	pushq	%rdx
	pushq	%rdi
	movq	-20(%rbp), %rax
	movq	%rax, %rdi
	call	sys_times
	popq	%rdi
	popq	%rdx
	movq	-20(%rbp), %r14
	movl	0(%r14), %ebx
	movl	%ebx, -28(%rbp)
	movq	-8(%rbp), %r14
	movl	0(%r14), %ebx
	movl	%ebx, -24(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movl	-24(%rbp), %edi
	add	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -24(%rbp)
	pushq	%rdi
	pushq	%rdx
	pushq	%rdx
	pushq	%rdi
	movl	-24(%rbp), %eax
	movl	%eax, %edi
	call	abs
	popq	%rdi
	popq	%rdx
	movl	%eax, %edx
	movl	-28(%rbp), %edi
	add	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -24(%rbp)
	pushq	%rdx
	pushq	%rdi
	movl	-24(%rbp), %eax
	movl	%eax, %edi
	pushq	%rdi
	pushq	%rdx
	movl	-24(%rbp), %eax
	movl	-12(%rbp), %ecx
	cltd
	idiv	%ecx
	movl	%edx, %eax
	popq	%rdx
	popq	%rdi
	pushq	%rsi
	movl	%eax, %eax
	movl	%eax, %esi
	call	exp
	popq	%rsi
	popq	%rdi
	popq	%rdx
	movl	%eax, %ebx
	movl	%ebx, -24(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	-24(%rbp), %eax
	movl	-12(%rbp), %ecx
	cltd
	idiv	%ecx
	movl	%edx, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -24(%rbp)
	movq	-8(%rbp), %rdx
	movl	-24(%rbp), %ebx
	movl	%ebx, 0(%rdx)
	pushq	%rdx
	pushq	%rdi
	movl	-24(%rbp), %eax
	movl	%eax, %edi
	call	abs
	popq	%rdi
	popq	%rdx
	movl	%eax, %eax
	leave
	ret


.data



.bss

