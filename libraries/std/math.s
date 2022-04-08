.global	math.abs
.global	math.longDiv
.global	math.longExp
.global	math.exp
.global	pub_Random_init
.global	pub_Random_nextInt


.text

pub_Random_nextInt:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$16, %rsp
	movq	%rdi, -8(%rbp)
	movl	%esi, -12(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	$910230123, %edx
	movq	-8(%rbp), %r14
	movl	0(%r14), %edi
	add	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movq	-8(%rbp), %rdx
	movl	%eax, %ebx
	movl	%ebx, 0(%rdx)
	movq	-8(%rbp), %r14
	movl	0(%r14), %ebx
	movl	%ebx, -16(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	$0, %edx
	movl	-16(%rbp), %edi
	cmpl	%edx, %edi
	setl	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.Lexp0
	pushq	%rdi
	pushq	%rdx
	movl	$-1, %edx
	movl	-16(%rbp), %edi
	imul	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -16(%rbp)
.Lexp0:
	pushq	%rdi
	pushq	%rdx
	movl	-16(%rbp), %eax
	movl	-12(%rbp), %ecx
	cltd
	idiv	%ecx
	movl	%edx, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %eax
	leave
	ret
pub_Random_init:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$16, %rsp
	movq	%rdi, -8(%rbp)
	pushq	%rdx
	movq	$0, %rdi
	call	DateTime.Now
	popq	%rdx
	movq	%rax, %rbx
	movq	%rbx, -16(%rbp)
	pushq	%rdx
	lea	-16(%rbp), %rax
	pushq	%rdi
	movq	(%rax), %rax
	movq	%rax, %rdi
	call	pub_DateTime_getSeconds
	popq	%rdi
	popq	%rdx
	movq	-8(%rbp), %rdx
	movl	%eax, %ebx
	movl	%ebx, 0(%rdx)
	pushq	%rdx
	lea	-8(%rbp), %rax
	pushq	%rdi
	movq	(%rax), %rax
	movq	%rax, %rdi
	pushq	%rsi
	movl	$100, %eax
	movl	%eax, %esi
	call	pub_Random_nextInt
	popq	%rsi
	popq	%rdi
	popq	%rdx
	popq	%rbx
	movq	-8(%rbp), %rax
	leave
	ret
math.exp:
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
	jmp	.Lexp2
.Lexp1:
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
.Lexp2:
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
	je	.Lexp1
	movl	-16(%rbp), %eax
	leave
	ret
math.longExp:
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
	jmp	.LlongExp4
.LlongExp3:
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
.LlongExp4:
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
	je	.LlongExp3
	movq	-24(%rbp), %rax
	leave
	ret
math.longDiv:
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
math.abs:
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
	je	.Labs5
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
.Labs5:
	movl	-4(%rbp), %eax
	leave
	ret


.data



.bss

