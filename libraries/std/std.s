.global	newTimes
.global	newBit
.global	realloc
.global	malloc
.global	free
.global	memcopy
.global	getBlock
.global	newTime
.global	requestSpace
.global	findFreeBlock


.text

findFreeBlock:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$32, %rsp
	movq	%rdi, -8(%rbp)
	movl	%esi, -12(%rbp)
	movq	head, %rbx
	movq	%rbx, -20(%rbp)
	mov	$1, %edx
	mov	-12(%rbp), %eax
	sub	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -12(%rbp)
	jmp	.LfindFreeBlock1
.LfindFreeBlock0:
	movq	-20(%rbp), %rbx
	movq	%rbx, -8(%rbp)
	movq	-20(%rbp), %rdx
	movl	4(%rdx), %eax
	movl	$1, %ecx
	cmpl	%ecx, %eax
	jne	.LfindFreeBlock2
	movq	-20(%rbp), %rdx
	movl	0(%rdx), %eax
	movl	-12(%rbp), %ecx
	cmpl	%ecx, %eax
	jle	.LfindFreeBlock3
	movq	-20(%rbp), %rax
	leave
	ret
.LfindFreeBlock3:
.LfindFreeBlock2:
	movq	-20(%rbp), %rdx
	movq	8(%rdx), %rbx
	movq	%rbx, -20(%rbp)
	movq	-20(%rbp), %rbx
	movq	%rbx, -8(%rbp)
.LfindFreeBlock1:
	movq	-20(%rbp), %rax
	movq	$0, %rcx
	cmpq	%rcx, %rax
	jne	.LfindFreeBlock0
	movq	$0, %rax
	leave
	ret
requestSpace:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$32, %rsp
	movq	%rdi, -8(%rbp)
	movl	%esi, -12(%rbp)
	movl	$0, %eax
	movl	%eax, %edi
	call	brk
	movq	%rax, -20(%rbp)
	mov	$16, %edx
	mov	-12(%rbp), %eax
	add	%edx, %eax
	movl	%eax, -24(%rbp)
	mov	-24(%rbp), %edx
	mov	-20(%rbp), %rax
	add	%rdx, %rax
	movl	%eax, %eax
	movl	%eax, %edi
	call	brk
	movq	%rax, -32(%rbp)
	movq	-32(%rbp), %rax
	movq	$0, %rcx
	cmpq	%rcx, %rax
	jge	.LrequestSpace4
	movq	$0, %rax
	leave
	ret
.LrequestSpace4:
	movq	-8(%rbp), %rax
	movq	$0, %rcx
	cmpq	%rcx, %rax
	je	.LrequestSpace5
	movq	-8(%rbp), %rdx
	movq	-20(%rbp), %rbx
	movq	%rbx, 8(%rdx)
.LrequestSpace5:
	movq	-20(%rbp), %rdx
	movq	$0, %rbx
	movq	%rbx, 8(%rdx)
	movq	-20(%rbp), %rdx
	movl	-12(%rbp), %ebx
	movl	%ebx, 0(%rdx)
	movq	-20(%rbp), %rdx
	movl	$0, %ebx
	movl	%ebx, 4(%rdx)
	movq	-20(%rbp), %rax
	leave
	ret
newTime:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$16, %rsp
	movq	%rdi, -8(%rbp)
	movl	$8, %eax
	movl	%eax, %edi
	call	malloc
	movq	%rax, -16(%rbp)
	movq	-16(%rbp), %rdx
	movq	-8(%rbp), %rbx
	movq	%rbx, 0(%rdx)
	movq	-16(%rbp), %rax
	leave
	ret
getBlock:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$16, %rsp
	movq	%rdi, -8(%rbp)
	mov	$16, %edx
	mov	-8(%rbp), %rax
	sub	%rdx, %rax
	movl	%eax, %ebx
	movl	%ebx, -8(%rbp)
	movq	-8(%rbp), %rax
	leave
	ret
memcopy:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$32, %rsp
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	movl	%edx, -20(%rbp)
	movl	$0, -24(%rbp)
	jmp	.Lmemcopy7
.Lmemcopy6:
	movq	-8(%rbp), %rax
	movb	(%rax), %al
	movq	-16(%rbp), %rax
	movb	%al, %bl
	movb	%bl, (%rax)
	mov	$1, %rdx
	mov	-16(%rbp), %rax
	add	%rdx, %rax
	movl	%eax, %ebx
	movl	%ebx, -16(%rbp)
	mov	$1, %rdx
	mov	-8(%rbp), %rax
	add	%rdx, %rax
	movl	%eax, %ebx
	movl	%ebx, -8(%rbp)
.Lmemcopy7:
	movl	-24(%rbp), %eax
	movl	-20(%rbp), %ecx
	cmpl	%ecx, %eax
	jl	.Lmemcopy6
	movl	$0, %eax
	leave
	ret
free:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$16, %rsp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rax
	movq	$0, %rcx
	cmpq	%rcx, %rax
	jne	.Lfree8
	movl	$0, %eax
	leave
	ret
.Lfree8:
	movq	-8(%rbp), %rax
	movq	%rax, %rdi
	call	getBlock
	movq	%rax, -16(%rbp)
	movq	-16(%rbp), %rdx
	movl	$1, %ebx
	movl	%ebx, 4(%rdx)
	movl	$0, %eax
	leave
	ret
malloc:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$32, %rsp
	movl	%edi, -4(%rbp)
	movl	-4(%rbp), %eax
	movl	$0, %ecx
	cmpl	%ecx, %eax
	jge	.Lmalloc9
	movq	$0, %rax
	leave
	ret
.Lmalloc9:
	movq	head, %rax
	movq	$0, %rcx
	cmpq	%rcx, %rax
	jne	.Lmalloc10
	movq	$0, %rax
	movq	%rax, %rdi
	movl	-4(%rbp), %eax
	movl	%eax, %esi
	call	requestSpace
	movq	%rax, %rbx
	movq	%rbx, -12(%rbp)
	mov	$16, %rdx
	mov	-12(%rbp), %rax
	add	%rdx, %rax
	movl	%eax, %ebx
	movl	%ebx, -12(%rbp)
	movq	-12(%rbp), %rax
	leave
	ret
.Lmalloc10:
	movq	head, %rbx
	movq	%rbx, -20(%rbp)
	movq	head, %rax
	movq	%rax, %rdi
	movl	-4(%rbp), %eax
	movl	%eax, %esi
	call	findFreeBlock
	movq	%rax, %rbx
	movq	%rbx, -12(%rbp)
	movq	-12(%rbp), %rax
	movq	$0, %rcx
	cmpq	%rcx, %rax
	jne	.Lmalloc11
	movq	-20(%rbp), %rax
	movq	%rax, %rdi
	movl	-4(%rbp), %eax
	movl	%eax, %esi
	call	requestSpace
	movq	%rax, %rbx
	movq	%rbx, -12(%rbp)
	movq	-12(%rbp), %rax
	movq	$0, %rcx
	cmpq	%rcx, %rax
	jne	.Lmalloc12
	movq	$0, %rax
	leave
	ret
.Lmalloc12:
.Lmalloc11:
	mov	$16, %edx
	mov	-12(%rbp), %rax
	add	%rdx, %rax
	movl	%eax, %ebx
	movl	%ebx, -12(%rbp)
	movq	-12(%rbp), %rax
	leave
	ret
realloc:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$32, %rsp
	movq	%rdi, -8(%rbp)
	movl	%esi, -12(%rbp)
	movq	-8(%rbp), %rax
	movq	$0, %rcx
	cmpq	%rcx, %rax
	jne	.Lrealloc13
	movl	-12(%rbp), %eax
	movl	%eax, %edi
	call	malloc
	movq	%rax, %rax
	leave
	ret
.Lrealloc13:
	movq	-8(%rbp), %rax
	movq	%rax, %rdi
	call	getBlock
	movq	%rax, -20(%rbp)
	mov	$1, %edx
	mov	-12(%rbp), %eax
	sub	%edx, %eax
	movl	%eax, -24(%rbp)
	movq	-20(%rbp), %rdx
	movl	0(%rdx), %eax
	movl	-24(%rbp), %ecx
	cmpl	%ecx, %eax
	jle	.Lrealloc14
	movq	-8(%rbp), %rax
	leave
	ret
.Lrealloc14:
	movl	-12(%rbp), %eax
	movl	%eax, %edi
	call	malloc
	movq	%rax, -32(%rbp)
	movq	-32(%rbp), %rax
	movq	$0, %rcx
	cmpq	%rcx, %rax
	jne	.Lrealloc15
	movq	$0, %rax
	leave
	ret
.Lrealloc15:
	movq	-8(%rbp), %rax
	movq	%rax, %rdi
	movq	-32(%rbp), %rax
	movq	%rax, %rsi
	movq	-20(%rbp), %rdx
	movl	0(%rdx), %eax
	movl	%eax, %edx
	call	memcopy
	movq	-8(%rbp), %rax
	movq	%rax, %rdi
	call	free
	movq	-32(%rbp), %rax
	leave
	ret
newBit:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$16, %rsp
	movl	%edi, -4(%rbp)
	movl	$20, %eax
	movl	%eax, %edi
	call	malloc
	movq	%rax, -12(%rbp)
	movq	-12(%rbp), %rdx
	movl	-4(%rbp), %ebx
	movl	%ebx, 0(%rdx)
	movq	-12(%rbp), %rdx
	movq	$0, %rbx
	movq	%rbx, 4(%rdx)
	movq	-12(%rbp), %rdx
	movq	$0, %rbx
	movq	%rbx, 12(%rdx)
	movq	-12(%rbp), %rax
	leave
	ret
newTimes:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$16, %rsp
	movl	$32, %eax
	movl	%eax, %edi
	call	malloc
	movq	%rax, -8(%rbp)
	movq	-8(%rbp), %rdx
	movq	$0, %rbx
	movq	%rbx, 24(%rdx)
	movq	-8(%rbp), %rdx
	movq	$0, %rbx
	movq	%rbx, 16(%rdx)
	movq	-8(%rbp), %rdx
	movq	$0, %rbx
	movq	%rbx, 8(%rdx)
	movq	-8(%rbp), %rdx
	movq	$0, %rbx
	movq	%rbx, 0(%rdx)
	movq	-8(%rbp), %rax
	leave
	ret


.data

head:
.quad	0


.bss

