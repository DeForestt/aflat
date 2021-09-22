.global	print


.text

print:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$16, %rsp
	movl	%edi, -8(%rbp)
	movl	$0, -12(%rbp)
	jmp	print1
print0:
	movq	$1, %rdi
	movq	-8(%rbp), %rsi
	movq	$1, %rdx
	movq	$1, %rax
	syscall	
	mov	-8(%rbp), %rdx
	mov	$1, %eax
	add	%edx, %eax
	movl	%eax, -8(%rbp)
print1:
	movq	-8(%rbp), %rax
	movb	(%rax), %al
	movb	%al, %al
	movb	-12(%rbp), %cl
	cmpb	%cl, %al
	jne	print0
	movl	$0, %eax
	leave
	ret
