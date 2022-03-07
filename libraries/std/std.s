.global	newTimes
.global	newBit
.global	inspectHeap
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
	movq	-20(%rbp), %rdx
	movl	4(%rdx), %eax
	movl	$1, %ecx
	cmpl	%ecx, %eax
	jne	.LfindFreeBlock2
	movq	-20(%rbp), %rdx
	movl	0(%rdx), %eax
	movl	-12(%rbp), %ecx
	cmpl	%ecx, %eax
	jle	.LfindFreeBlock5
	movq	-20(%rbp), %rax
	leave
	ret
.LfindFreeBlock5:
.LfindFreeBlock2:
	movq	-20(%rbp), %rdx
	movq	8(%rdx), %rbx
	movq	%rbx, -20(%rbp)
.LfindFreeBlock1:
	movq	-20(%rbp), %rax
	movq	$0, %rcx
	cmpq	%rcx, %rax
	jne	.LfindFreeBlock0
	movq	$0, %rax
	leave
	ret
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
	movq	%rax, %rbx
	movq	%rbx, -20(%rbp)
	mov	$16, %edx
	mov	-12(%rbp), %eax
	add	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -24(%rbp)
	mov	-24(%rbp), %edx
	mov	-20(%rbp), %rax
	add	%rdx, %rax
	movl	%eax, %eax
	movl	%eax, %edi
	call	brk
	movq	%rax, %rbx
	movq	%rbx, -32(%rbp)
	movq	-32(%rbp), %rax
	movq	$0, %rcx
	cmpq	%rcx, %rax
	jge	.LrequestSpace8
	movq	$0, %rax
	leave
	ret
.LrequestSpace8:
	movq	-8(%rbp), %rax
	movq	$0, %rcx
	cmpq	%rcx, %rax
	je	.LrequestSpace11
	movq	-8(%rbp), %rdx
	movq	-20(%rbp), %rbx
	movq	%rbx, 8(%rdx)
.LrequestSpace11:
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
	movq	%rax, %rbx
	movq	%rbx, -16(%rbp)
	movq	-16(%rbp), %rdx
	movq	-8(%rbp), %rbx
	movq	%rbx, 0(%rdx)
	movq	-16(%rbp), %rax
	leave
	ret
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
	leave
	ret
memcopy:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$32, %rsp
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	movl	%edx, -20(%rbp)
	movl	$0, %ebx
	movl	%ebx, -24(%rbp)
	jmp	.Lmemcopy15
.Lmemcopy14:
	movq	-8(%rbp), %rax
	movb	(%rax), %al
	movb	%al, %bl
	movb	%bl, -25(%rbp)
	movq	-16(%rbp), %rax
	movb	-25(%rbp), %bl
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
	mov	$1, %edx
	mov	-24(%rbp), %eax
	add	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -24(%rbp)
.Lmemcopy15:
	movl	-24(%rbp), %eax
	movl	-20(%rbp), %ecx
	cmpl	%ecx, %eax
	jl	.Lmemcopy14
	movl	$0, %eax
	leave
	ret
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
	jne	.Lfree16
	movl	$0, %eax
	leave
	ret
.Lfree16:
	movq	-8(%rbp), %rax
	movq	%rax, %rdi
	call	getBlock
	movq	%rax, %rbx
	movq	%rbx, -16(%rbp)
	movq	-16(%rbp), %rdx
	movl	$1, %ebx
	movl	%ebx, 4(%rdx)
	movl	$0, %eax
	leave
	ret
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
	jge	.Lmalloc19
	movq	$0, %rax
	leave
	ret
.Lmalloc19:
	movq	head, %rax
	movq	$0, %rcx
	cmpq	%rcx, %rax
	jne	.Lmalloc22
	movq	$0, %rax
	movq	%rax, %rdi
	movl	-4(%rbp), %eax
	movl	%eax, %esi
	call	requestSpace
	movq	%rax, %rbx
	movq	%rbx, -12(%rbp)
	movq	-12(%rbp), %rbx
	movq	%rbx, head
	mov	$16, %rdx
	mov	-12(%rbp), %rax
	add	%rdx, %rax
	movl	%eax, %ebx
	movl	%ebx, -12(%rbp)
	movq	-12(%rbp), %rax
	leave
	ret
.Lmalloc22:
	movq	head, %rbx
	movq	%rbx, -20(%rbp)
	jmp	.Lmalloc26
.Lmalloc25:
	movq	-20(%rbp), %rdx
	movq	8(%rdx), %rbx
	movq	%rbx, -20(%rbp)
.Lmalloc26:
	movq	-20(%rbp), %rdx
	movq	8(%rdx), %rax
	movq	$0, %rcx
	cmpq	%rcx, %rax
	jne	.Lmalloc25
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
	jne	.Lmalloc27
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
	jne	.Lmalloc30
	movq	$0, %rax
	leave
	ret
.Lmalloc30:
.Lmalloc27:
	movq	-12(%rbp), %rdx
	movl	$0, %ebx
	movl	%ebx, 4(%rdx)
	mov	$16, %edx
	mov	-12(%rbp), %rax
	add	%rdx, %rax
	movl	%eax, %ebx
	movl	%ebx, -12(%rbp)
	movq	-12(%rbp), %rax
	leave
	ret
	leave
	ret
realloc:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$48, %rsp
	movq	%rdi, -8(%rbp)
	movl	%esi, -12(%rbp)
	movq	-8(%rbp), %rax
	movq	$0, %rcx
	cmpq	%rcx, %rax
	jne	.Lrealloc33
	movl	-12(%rbp), %eax
	movl	%eax, %edi
	call	malloc
	movq	%rax, %rax
	leave
	ret
.Lrealloc33:
	movq	-8(%rbp), %rax
	movq	%rax, %rdi
	call	getBlock
	movq	%rax, %rbx
	movq	%rbx, -20(%rbp)
	mov	$1, %edx
	mov	-12(%rbp), %eax
	sub	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -24(%rbp)
	movq	-20(%rbp), %rdx
	movl	0(%rdx), %eax
	movl	-24(%rbp), %ecx
	cmpl	%ecx, %eax
	jle	.Lrealloc36
	movq	-8(%rbp), %rax
	leave
	ret
.Lrealloc36:
	movl	-12(%rbp), %eax
	movl	%eax, %edi
	call	malloc
	movq	%rax, %rbx
	movq	%rbx, -32(%rbp)
	movq	-32(%rbp), %rax
	movq	$0, %rcx
	cmpq	%rcx, %rax
	jne	.Lrealloc39
	movq	$0, %rax
	leave
	ret
.Lrealloc39:
	movq	-20(%rbp), %rdx
	movl	0(%rdx), %ebx
	movl	%ebx, -36(%rbp)
	movq	-8(%rbp), %rax
	movq	%rax, %rdi
	movq	-32(%rbp), %rax
	movq	%rax, %rsi
	movl	-36(%rbp), %eax
	movl	%eax, %edx
	call	memcopy
	movq	-8(%rbp), %rax
	movq	%rax, %rdi
	call	free
	movq	-32(%rbp), %rax
	leave
	ret
	leave
	ret
inspectHeap:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$16, %rsp
	movq	head, %rbx
	movq	%rbx, -8(%rbp)
	movq	$.strinspectHeap42, %rax
	movq	%rax, %rdi
	call	print
	movq	$.strinspectHeap43, %rax
	movq	%rax, %rdi
	call	print
	jmp	.LinspectHeap45
.LinspectHeap44:
	movq	-8(%rbp), %rax
	movq	%rax, %rdi
	call	printHex
	movq	$.strinspectHeap46, %rax
	movq	%rax, %rdi
	call	print
	movq	-8(%rbp), %rdx
	movl	4(%rdx), %eax
	movl	%eax, %edi
	call	printInt
	movq	$.strinspectHeap47, %rax
	movq	%rax, %rdi
	call	print
	movq	-8(%rbp), %rdx
	movl	0(%rdx), %eax
	movl	%eax, %edi
	call	printInt
	movq	$.strinspectHeap48, %rax
	movq	%rax, %rdi
	call	print
	movq	-8(%rbp), %rdx
	movq	8(%rdx), %rbx
	movq	%rbx, -8(%rbp)
	movq	$.strinspectHeap49, %rax
	movq	%rax, %rdi
	call	print
.LinspectHeap45:
	movq	-8(%rbp), %rax
	movq	$0, %rcx
	cmpq	%rcx, %rax
	jne	.LinspectHeap44
	movq	$.strinspectHeap50, %rax
	movq	%rax, %rdi
	call	print
	movl	$0, %eax
	leave
	ret
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
	movq	%rax, %rbx
	movq	%rbx, -12(%rbp)
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
	leave
	ret
newTimes:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$16, %rsp
	movl	$16, %eax
	movl	%eax, %edi
	call	malloc
	movq	%rax, %rbx
	movq	%rbx, -8(%rbp)
	movq	-8(%rbp), %rdx
	movl	$0, %ebx
	movl	%ebx, 12(%rdx)
	movq	-8(%rbp), %rdx
	movl	$0, %ebx
	movl	%ebx, 8(%rdx)
	movq	-8(%rbp), %rdx
	movl	$0, %ebx
	movl	%ebx, 4(%rdx)
	movq	-8(%rbp), %rdx
	movl	$0, %ebx
	movl	%ebx, 0(%rdx)
	movq	-8(%rbp), %rax
	leave
	ret
	leave
	ret


.data

.strinspectHeap50:
	.asciz	 "NULL\n"
.strinspectHeap49:
	.asciz	 "         "
.strinspectHeap48:
	.asciz	 "\n"
.strinspectHeap47:
	.asciz	 " size: "
.strinspectHeap46:
	.asciz	 ": status: "
.strinspectHeap43:
	.asciz	 "HEAD --> "
.strinspectHeap42:
	.asciz	 "Heap:\n"
head:
.quad	0


.bss

