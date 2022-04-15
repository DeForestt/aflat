.global	sleep
.global	assert
.global	panic
.global	newTimes
.global	newBit
.global	inspectHeap
.global	realloc
.global	malloc
.global	free
.global	memcopy
.global	deFragAll
.global	defragment
.global	splitBlock
.global	getBlock
.global	newTime
.global	requestSpace
.global	findFreeBlock


.text

findFreeBlock:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$32, %rsp
	movq	%rdi, -8(%rbp)
	movl	%esi, -12(%rbp)
	movq	head, %r15
	movq	%r15, %rbx
	movq	%rbx, -20(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movl	-12(%rbp), %r15d
	movl	%r15d, %edi
	sub	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -12(%rbp)
	jmp	.LfindFreeBlock1
.LfindFreeBlock0:
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	pushq	%r14
	movq	-20(%rbp), %r14
	movl	4(%r14), %r15d
	popq	%r14
	movl	%r15d, %edi
	cmpl	%edx, %edi
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.LfindFreeBlock2
	pushq	%rdi
	pushq	%rdx
	movl	-12(%rbp), %r15d
	movl	%r15d, %edx
	pushq	%r14
	movq	-20(%rbp), %r14
	movl	0(%r14), %r15d
	popq	%r14
	movl	%r15d, %edi
	cmpl	%edx, %edi
	setg	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.LfindFreeBlock3
	movq	-20(%rbp), %r15
	movq	%r15, %rax
	leave
	ret
.LfindFreeBlock3:
.LfindFreeBlock2:
	pushq	%r14
	movq	-20(%rbp), %r14
	movq	8(%r14), %r15
	popq	%r14
	movq	%r15, %rbx
	movq	%rbx, -20(%rbp)
.LfindFreeBlock1:
	pushq	%rdi
	pushq	%rdx
	movq	$0, %rdx
	movq	-20(%rbp), %r15
	movq	%r15, %rdi
	cmpq	%rdx, %rdi
	setne	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$1, %al
	je	.LfindFreeBlock0
	movq	$0, %rax
	leave
	ret
requestSpace:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$32, %rsp
	movq	%rdi, -8(%rbp)
	movl	%esi, -12(%rbp)
	pushq	%rdx
	pushq	%rdi
	movq	$0, %rax
	movq	%rax, %rdi
	call	brk
	popq	%rdi
	popq	%rdx
	movq	%rax, %rbx
	movq	%rbx, -20(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	$16, %edx
	movl	-12(%rbp), %r15d
	movl	%r15d, %edi
	add	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -24(%rbp)
	pushq	%rdx
	pushq	%rdi
	pushq	%rdx
	movl	-24(%rbp), %r15d
	movl	%r15d, %edx
	movq	-20(%rbp), %r15
	movq	%r15, %rdi
	add	%rdx, %rdi
	movq	%rdi, %rax
	popq	%rdx
	popq	%rdi
	pushq	%rdi
	movq	%rax, %rax
	movq	%rax, %rdi
	call	brk
	popq	%rdi
	popq	%rdx
	movq	%rax, %rbx
	movq	%rbx, -32(%rbp)
	pushq	%rdi
	pushq	%rdx
	movq	$0, %rdx
	movq	-32(%rbp), %r15
	movq	%r15, %rdi
	cmpq	%rdx, %rdi
	setl	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.LrequestSpace4
	movq	$0, %rax
	leave
	ret
.LrequestSpace4:
	pushq	%rdi
	pushq	%rdx
	movq	$0, %rdx
	movq	-8(%rbp), %r15
	movq	%r15, %rdi
	cmpq	%rdx, %rdi
	setne	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.LrequestSpace5
	pushq	%r14
	movq	-8(%rbp), %r14
	movq	-20(%rbp), %r15
	movq	%r15, %rbx
	movq	%rbx, 8(%r14)
	popq	%r14
.LrequestSpace5:
	pushq	%r14
	movq	-20(%rbp), %r14
	movq	$0, %rbx
	movq	%rbx, 8(%r14)
	popq	%r14
	pushq	%r14
	movq	-20(%rbp), %r14
	movl	-12(%rbp), %r15d
	movl	%r15d, %ebx
	movl	%ebx, 0(%r14)
	popq	%r14
	pushq	%r14
	movq	-20(%rbp), %r14
	movl	$0, %ebx
	movl	%ebx, 4(%r14)
	popq	%r14
	movq	-20(%rbp), %r15
	movq	%r15, %rax
	leave
	ret
newTime:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$16, %rsp
	movq	%rdi, -8(%rbp)
	pushq	%rdx
	pushq	%rdi
	movl	$8, %eax
	movl	%eax, %edi
	call	malloc
	popq	%rdi
	popq	%rdx
	movq	%rax, %rbx
	movq	%rbx, -16(%rbp)
	pushq	%r14
	movq	-16(%rbp), %r14
	movq	-8(%rbp), %r15
	movq	%r15, %rbx
	movq	%rbx, 0(%r14)
	popq	%r14
	movq	-16(%rbp), %r15
	movq	%r15, %rax
	leave
	ret
getBlock:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$16, %rsp
	movq	%rdi, -8(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	$16, %edx
	movq	-8(%rbp), %r15
	movq	%r15, %rdi
	sub	%rdx, %rdi
	movq	%rdi, %rax
	popq	%rdx
	popq	%rdi
	movq	%rax, %rbx
	movq	%rbx, -8(%rbp)
	movq	-8(%rbp), %r15
	movq	%r15, %rax
	leave
	ret
splitBlock:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$48, %rsp
	movq	%rdi, -8(%rbp)
	movl	%esi, -12(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	$16, %edx
	movl	-12(%rbp), %r15d
	movl	%r15d, %edi
	add	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -16(%rbp)
	pushq	%r14
	movq	-8(%rbp), %r14
	movl	0(%r14), %r15d
	popq	%r14
	movl	%r15d, %ebx
	movl	%ebx, -20(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	-16(%rbp), %r15d
	movl	%r15d, %edx
	pushq	%r14
	movq	-8(%rbp), %r14
	movl	0(%r14), %r15d
	popq	%r14
	movl	%r15d, %edi
	cmpl	%edx, %edi
	setg	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.LsplitBlock6
	movq	-8(%rbp), %r15
	movq	%r15, %rbx
	movq	%rbx, -28(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	-12(%rbp), %r15d
	movl	%r15d, %edx
	movq	-8(%rbp), %r15
	movq	%r15, %rdi
	add	%rdx, %rdi
	movq	%rdi, %rax
	popq	%rdx
	popq	%rdi
	movq	%rax, %rbx
	movq	%rbx, -28(%rbp)
	pushq	%rdi
	pushq	%rdx
	movq	$16, %rdx
	movq	-28(%rbp), %r15
	movq	%r15, %rdi
	add	%rdx, %rdi
	movq	%rdi, %rax
	popq	%rdx
	popq	%rdi
	movq	%rax, %rbx
	movq	%rbx, -28(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	$16, %edx
	movl	-20(%rbp), %r15d
	movl	%r15d, %edi
	sub	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -20(%rbp)
	pushq	%r14
	movq	-28(%rbp), %r14
	pushq	%rdi
	pushq	%rdx
	movl	-12(%rbp), %r15d
	movl	%r15d, %edx
	movl	-20(%rbp), %r15d
	movl	%r15d, %edi
	sub	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, 0(%r14)
	popq	%r14
	pushq	%r14
	movq	-28(%rbp), %r14
	movl	$1, %ebx
	movl	%ebx, 4(%r14)
	popq	%r14
	pushq	%r14
	movq	-8(%rbp), %r14
	movq	8(%r14), %r15
	popq	%r14
	movq	%r15, %rbx
	movq	%rbx, -36(%rbp)
	pushq	%r14
	movq	-28(%rbp), %r14
	movq	-36(%rbp), %r15
	movq	%r15, %rbx
	movq	%rbx, 8(%r14)
	popq	%r14
	pushq	%r14
	movq	-8(%rbp), %r14
	movq	-28(%rbp), %r15
	movq	%r15, %rbx
	movq	%rbx, 8(%r14)
	popq	%r14
	pushq	%r14
	movq	-8(%rbp), %r14
	movl	-12(%rbp), %r15d
	movl	%r15d, %ebx
	movl	%ebx, 0(%r14)
	popq	%r14
.LsplitBlock6:
	leave
	ret
defragment:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$32, %rsp
	movq	%rdi, -8(%rbp)
	pushq	%r14
	movq	-8(%rbp), %r14
	movq	8(%r14), %r15
	popq	%r14
	movq	%r15, %rbx
	movq	%rbx, -16(%rbp)
	pushq	%rdi
	pushq	%rdx
	movq	$0, %rdx
	pushq	%r14
	movq	-8(%rbp), %r14
	movq	8(%r14), %r15
	popq	%r14
	movq	%r15, %rdi
	cmpq	%rdx, %rdi
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.Ldefragment7
	movl	$0, %eax
	leave
	ret
.Ldefragment7:
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	pushq	%r14
	movq	-8(%rbp), %r14
	movl	4(%r14), %r15d
	popq	%r14
	movl	%r15d, %edi
	cmpl	%edx, %edi
	setne	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.Ldefragment8
	movl	$0, %eax
	leave
	ret
.Ldefragment8:
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	pushq	%r14
	movq	-16(%rbp), %r14
	movl	4(%r14), %r15d
	popq	%r14
	movl	%r15d, %edi
	cmpl	%edx, %edi
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.Ldefragment9
	pushq	%r14
	movq	-16(%rbp), %r14
	movl	0(%r14), %r15d
	popq	%r14
	movl	%r15d, %ebx
	movl	%ebx, -20(%rbp)
	pushq	%r14
	movq	-8(%rbp), %r14
	movl	0(%r14), %r15d
	popq	%r14
	movl	%r15d, %ebx
	movl	%ebx, -24(%rbp)
	pushq	%r14
	movq	-16(%rbp), %r14
	movq	8(%r14), %r15
	popq	%r14
	movq	%r15, %rbx
	movq	%rbx, -32(%rbp)
	pushq	%r14
	movq	-8(%rbp), %r14
	pushq	%rdi
	pushq	%rdx
	movl	$16, %edx
	pushq	%rdi
	pushq	%rdx
	movl	-20(%rbp), %r15d
	movl	%r15d, %edx
	movl	-24(%rbp), %r15d
	movl	%r15d, %edi
	add	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %edi
	add	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, 0(%r14)
	popq	%r14
	pushq	%r14
	movq	-8(%rbp), %r14
	movq	-32(%rbp), %r15
	movq	%r15, %rbx
	movq	%rbx, 8(%r14)
	popq	%r14
	movl	$1, %eax
	leave
	ret
.Ldefragment9:
	movl	$0, %eax
	leave
	ret
deFragAll:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$32, %rsp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %r15
	movq	%r15, %rbx
	movq	%rbx, -16(%rbp)
	jmp	.LdeFragAll11
.LdeFragAll10:
	pushq	%rdx
	movq	-16(%rbp), %r15
	pushq	%rdi
	movq	%r15, %rax
	movq	%rax, %rdi
	call	defragment
	popq	%rdi
	popq	%rdx
	movl	%eax, %ebx
	movl	%ebx, -20(%rbp)
	jmp	.LdeFragAll13
.LdeFragAll12:
	pushq	%rdx
	movq	-16(%rbp), %r15
	pushq	%rdi
	movq	%r15, %rax
	movq	%rax, %rdi
	call	defragment
	popq	%rdi
	popq	%rdx
	movl	%eax, %ebx
	movl	%ebx, -20(%rbp)
.LdeFragAll13:
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movl	-20(%rbp), %r15d
	movl	%r15d, %edi
	cmpl	%edx, %edi
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$1, %al
	je	.LdeFragAll12
	pushq	%r14
	movq	-16(%rbp), %r14
	movq	8(%r14), %r15
	popq	%r14
	movq	%r15, %rbx
	movq	%rbx, -16(%rbp)
.LdeFragAll11:
	pushq	%rdi
	pushq	%rdx
	movq	$0, %rdx
	movq	-16(%rbp), %r15
	movq	%r15, %rdi
	cmpq	%rdx, %rdi
	setne	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$1, %al
	je	.LdeFragAll10
	movl	$0, %eax
	leave
	ret
memcopy:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
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
	movb	-25(%rbp), %r15b
	movq	-16(%rbp), %rax
	movb	%r15b, %bl
	movb	%bl, (%rax)
	pushq	%rdi
	pushq	%rdx
	movq	$1, %rdx
	movq	-16(%rbp), %r15
	movq	%r15, %rdi
	add	%rdx, %rdi
	movq	%rdi, %rax
	popq	%rdx
	popq	%rdi
	movq	%rax, %rbx
	movq	%rbx, -16(%rbp)
	pushq	%rdi
	pushq	%rdx
	movq	$1, %rdx
	movq	-8(%rbp), %r15
	movq	%r15, %rdi
	add	%rdx, %rdi
	movq	%rdi, %rax
	popq	%rdx
	popq	%rdi
	movq	%rax, %rbx
	movq	%rbx, -8(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movl	-24(%rbp), %r15d
	movl	%r15d, %edi
	add	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -24(%rbp)
.Lmemcopy15:
	pushq	%rdi
	pushq	%rdx
	movl	-20(%rbp), %r15d
	movl	%r15d, %edx
	movl	-24(%rbp), %r15d
	movl	%r15d, %edi
	cmpl	%edx, %edi
	setl	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$1, %al
	je	.Lmemcopy14
	movl	$0, %eax
	leave
	ret
free:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$16, %rsp
	movq	%rdi, -8(%rbp)
	pushq	%rdi
	pushq	%rdx
	movq	$0, %rdx
	movq	-8(%rbp), %r15
	movq	%r15, %rdi
	cmpq	%rdx, %rdi
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.Lfree16
	movl	$0, %eax
	leave
	ret
.Lfree16:
	pushq	%rdx
	movq	-8(%rbp), %r15
	pushq	%rdi
	movq	%r15, %rax
	movq	%rax, %rdi
	call	getBlock
	popq	%rdi
	popq	%rdx
	movq	%rax, %rbx
	movq	%rbx, -16(%rbp)
	pushq	%r14
	movq	-16(%rbp), %r14
	movl	$1, %ebx
	movl	%ebx, 4(%r14)
	popq	%r14
	pushq	%rdx
	movq	head, %r15
	pushq	%rdi
	movq	%r15, %rax
	movq	%rax, %rdi
	call	deFragAll
	popq	%rdi
	popq	%rdx
	movl	$0, %eax
	leave
	ret
malloc:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$32, %rsp
	movl	%edi, -4(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	$0, %edx
	movl	-4(%rbp), %r15d
	movl	%r15d, %edi
	cmpl	%edx, %edi
	setl	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.Lmalloc17
	movq	$0, %rax
	leave
	ret
.Lmalloc17:
	pushq	%rdi
	pushq	%rdx
	movq	$0, %rdx
	movq	head, %r15
	movq	%r15, %rdi
	cmpq	%rdx, %rdi
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.Lmalloc18
	pushq	%rdx
	pushq	%rdi
	movq	$0, %rax
	movq	%rax, %rdi
	movl	-4(%rbp), %r15d
	pushq	%rsi
	movl	%r15d, %eax
	movl	%eax, %esi
	call	requestSpace
	popq	%rsi
	popq	%rdi
	popq	%rdx
	movq	%rax, %rbx
	movq	%rbx, -12(%rbp)
	movq	-12(%rbp), %r15
	movq	%r15, %rbx
	movq	%rbx, head
	pushq	%rdi
	pushq	%rdx
	movq	$16, %rdx
	movq	-12(%rbp), %r15
	movq	%r15, %rdi
	add	%rdx, %rdi
	movq	%rdi, %rax
	popq	%rdx
	popq	%rdi
	movq	%rax, %rbx
	movq	%rbx, -12(%rbp)
	movq	-12(%rbp), %r15
	movq	%r15, %rax
	leave
	ret
.Lmalloc18:
	movq	head, %r15
	movq	%r15, %rbx
	movq	%rbx, -20(%rbp)
	jmp	.Lmalloc20
.Lmalloc19:
	pushq	%r14
	movq	-20(%rbp), %r14
	movq	8(%r14), %r15
	popq	%r14
	movq	%r15, %rbx
	movq	%rbx, -20(%rbp)
.Lmalloc20:
	pushq	%rdi
	pushq	%rdx
	movq	$0, %rdx
	pushq	%r14
	movq	-20(%rbp), %r14
	movq	8(%r14), %r15
	popq	%r14
	movq	%r15, %rdi
	cmpq	%rdx, %rdi
	setne	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$1, %al
	je	.Lmalloc19
	pushq	%rdx
	movq	head, %r15
	pushq	%rdi
	movq	%r15, %rax
	movq	%rax, %rdi
	movl	-4(%rbp), %r15d
	pushq	%rsi
	movl	%r15d, %eax
	movl	%eax, %esi
	call	findFreeBlock
	popq	%rsi
	popq	%rdi
	popq	%rdx
	movq	%rax, %rbx
	movq	%rbx, -12(%rbp)
	pushq	%rdi
	pushq	%rdx
	movq	$0, %rdx
	movq	-12(%rbp), %r15
	movq	%r15, %rdi
	cmpq	%rdx, %rdi
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.Lmalloc21
	pushq	%rdx
	movq	-20(%rbp), %r15
	pushq	%rdi
	movq	%r15, %rax
	movq	%rax, %rdi
	movl	-4(%rbp), %r15d
	pushq	%rsi
	movl	%r15d, %eax
	movl	%eax, %esi
	call	requestSpace
	popq	%rsi
	popq	%rdi
	popq	%rdx
	movq	%rax, %rbx
	movq	%rbx, -12(%rbp)
	pushq	%rdi
	pushq	%rdx
	movq	$0, %rdx
	movq	-12(%rbp), %r15
	movq	%r15, %rdi
	cmpq	%rdx, %rdi
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.Lmalloc22
	movq	$0, %rax
	leave
	ret
.Lmalloc22:
.Lmalloc21:
	pushq	%r14
	movq	-12(%rbp), %r14
	movl	$0, %ebx
	movl	%ebx, 4(%r14)
	popq	%r14
	pushq	%rdx
	movq	-12(%rbp), %r15
	pushq	%rdi
	movq	%r15, %rax
	movq	%rax, %rdi
	movl	-4(%rbp), %r15d
	pushq	%rsi
	movl	%r15d, %eax
	movl	%eax, %esi
	call	splitBlock
	popq	%rsi
	popq	%rdi
	popq	%rdx
	pushq	%rdi
	pushq	%rdx
	movl	$16, %edx
	movq	-12(%rbp), %r15
	movq	%r15, %rdi
	add	%rdx, %rdi
	movq	%rdi, %rax
	popq	%rdx
	popq	%rdi
	movq	%rax, %rbx
	movq	%rbx, -12(%rbp)
	movq	-12(%rbp), %r15
	movq	%r15, %rax
	leave
	ret
realloc:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$48, %rsp
	movq	%rdi, -8(%rbp)
	movl	%esi, -12(%rbp)
	pushq	%rdi
	pushq	%rdx
	movq	$0, %rdx
	movq	-8(%rbp), %r15
	movq	%r15, %rdi
	cmpq	%rdx, %rdi
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.Lrealloc23
	pushq	%rdx
	movl	-12(%rbp), %r15d
	pushq	%rdi
	movl	%r15d, %eax
	movl	%eax, %edi
	call	malloc
	popq	%rdi
	popq	%rdx
	movq	%rax, %rax
	leave
	ret
.Lrealloc23:
	pushq	%rdx
	movq	-8(%rbp), %r15
	pushq	%rdi
	movq	%r15, %rax
	movq	%rax, %rdi
	call	getBlock
	popq	%rdi
	popq	%rdx
	movq	%rax, %rbx
	movq	%rbx, -20(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movl	-12(%rbp), %r15d
	movl	%r15d, %edi
	sub	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -24(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	-24(%rbp), %r15d
	movl	%r15d, %edx
	pushq	%r14
	movq	-20(%rbp), %r14
	movl	0(%r14), %r15d
	popq	%r14
	movl	%r15d, %edi
	cmpl	%edx, %edi
	setg	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.Lrealloc24
	movq	-8(%rbp), %r15
	movq	%r15, %rax
	leave
	ret
.Lrealloc24:
	pushq	%rdx
	movl	-12(%rbp), %r15d
	pushq	%rdi
	movl	%r15d, %eax
	movl	%eax, %edi
	call	malloc
	popq	%rdi
	popq	%rdx
	movq	%rax, %rbx
	movq	%rbx, -32(%rbp)
	pushq	%rdi
	pushq	%rdx
	movq	$0, %rdx
	movq	-32(%rbp), %r15
	movq	%r15, %rdi
	cmpq	%rdx, %rdi
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.Lrealloc25
	movq	$0, %rax
	leave
	ret
.Lrealloc25:
	pushq	%r14
	movq	-20(%rbp), %r14
	movl	0(%r14), %r15d
	popq	%r14
	movl	%r15d, %ebx
	movl	%ebx, -36(%rbp)
	pushq	%rdx
	movq	-8(%rbp), %r15
	pushq	%rdi
	movq	%r15, %rax
	movq	%rax, %rdi
	movq	-32(%rbp), %r15
	pushq	%rsi
	movq	%r15, %rax
	movq	%rax, %rsi
	movl	-36(%rbp), %r15d
	pushq	%rdx
	movl	%r15d, %eax
	movl	%eax, %edx
	call	memcopy
	popq	%rdx
	popq	%rsi
	popq	%rdi
	popq	%rdx
	pushq	%rdx
	movq	-8(%rbp), %r15
	pushq	%rdi
	movq	%r15, %rax
	movq	%rax, %rdi
	call	free
	popq	%rdi
	popq	%rdx
	movq	-32(%rbp), %r15
	movq	%r15, %rax
	leave
	ret
inspectHeap:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$16, %rsp
	movq	head, %r15
	movq	%r15, %rbx
	movq	%rbx, -8(%rbp)
	pushq	%rdx
	pushq	%rdi
	movq	$.strinspectHeap26, %rax
	movq	%rax, %rdi
	movq	$0, %rsi
	call	io.print
	popq	%rdi
	popq	%rdx
	pushq	%rdx
	pushq	%rdi
	movq	$.strinspectHeap27, %rax
	movq	%rax, %rdi
	movq	$0, %rsi
	call	io.print
	popq	%rdi
	popq	%rdx
	jmp	.LinspectHeap29
.LinspectHeap28:
	pushq	%rdx
	movq	-8(%rbp), %r15
	pushq	%rdi
	movq	%r15, %rax
	movq	%rax, %rdi
	call	io.printHex
	popq	%rdi
	popq	%rdx
	pushq	%rdx
	pushq	%rdi
	movq	$.strinspectHeap30, %rax
	movq	%rax, %rdi
	movq	$0, %rsi
	call	io.print
	popq	%rdi
	popq	%rdx
	pushq	%rdx
	pushq	%r14
	movq	-8(%rbp), %r14
	movl	4(%r14), %r15d
	popq	%r14
	pushq	%rdi
	movl	%r15d, %eax
	movl	%eax, %edi
	call	io.printInt
	popq	%rdi
	popq	%rdx
	pushq	%rdx
	pushq	%rdi
	movq	$.strinspectHeap31, %rax
	movq	%rax, %rdi
	movq	$0, %rsi
	call	io.print
	popq	%rdi
	popq	%rdx
	pushq	%rdx
	pushq	%r14
	movq	-8(%rbp), %r14
	movl	0(%r14), %r15d
	popq	%r14
	pushq	%rdi
	movl	%r15d, %eax
	movl	%eax, %edi
	call	io.printInt
	popq	%rdi
	popq	%rdx
	pushq	%rdx
	pushq	%rdi
	movq	$.strinspectHeap32, %rax
	movq	%rax, %rdi
	movq	$0, %rsi
	call	io.print
	popq	%rdi
	popq	%rdx
	pushq	%rdx
	pushq	%rdi
	movl	$16, %eax
	movl	%eax, %edi
	call	io.printInt
	popq	%rdi
	popq	%rdx
	pushq	%rdx
	pushq	%rdi
	movq	$.strinspectHeap33, %rax
	movq	%rax, %rdi
	movq	$0, %rsi
	call	io.print
	popq	%rdi
	popq	%rdx
	pushq	%r14
	movq	-8(%rbp), %r14
	movq	8(%r14), %r15
	popq	%r14
	movq	%r15, %rbx
	movq	%rbx, -8(%rbp)
	pushq	%rdx
	pushq	%rdi
	movq	$.strinspectHeap34, %rax
	movq	%rax, %rdi
	movq	$0, %rsi
	call	io.print
	popq	%rdi
	popq	%rdx
.LinspectHeap29:
	pushq	%rdi
	pushq	%rdx
	movq	$0, %rdx
	movq	-8(%rbp), %r15
	movq	%r15, %rdi
	cmpq	%rdx, %rdi
	setne	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$1, %al
	je	.LinspectHeap28
	pushq	%rdx
	pushq	%rdi
	movq	$.strinspectHeap35, %rax
	movq	%rax, %rdi
	movq	$0, %rsi
	call	io.print
	popq	%rdi
	popq	%rdx
	movl	$0, %eax
	leave
	ret
newBit:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$16, %rsp
	movl	%edi, -4(%rbp)
	pushq	%rdx
	pushq	%rdi
	movl	$20, %eax
	movl	%eax, %edi
	call	malloc
	popq	%rdi
	popq	%rdx
	movq	%rax, %rbx
	movq	%rbx, -12(%rbp)
	pushq	%r14
	movq	-12(%rbp), %r14
	movl	-4(%rbp), %r15d
	movl	%r15d, %ebx
	movl	%ebx, 0(%r14)
	popq	%r14
	pushq	%r14
	movq	-12(%rbp), %r14
	movq	$0, %rbx
	movq	%rbx, 4(%r14)
	popq	%r14
	pushq	%r14
	movq	-12(%rbp), %r14
	movq	$0, %rbx
	movq	%rbx, 12(%r14)
	popq	%r14
	movq	-12(%rbp), %r15
	movq	%r15, %rax
	leave
	ret
newTimes:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$16, %rsp
	pushq	%rdx
	pushq	%rdi
	movl	$16, %eax
	movl	%eax, %edi
	call	malloc
	popq	%rdi
	popq	%rdx
	movq	%rax, %rbx
	movq	%rbx, -8(%rbp)
	pushq	%r14
	movq	-8(%rbp), %r14
	movl	$0, %ebx
	movl	%ebx, 12(%r14)
	popq	%r14
	pushq	%r14
	movq	-8(%rbp), %r14
	movl	$0, %ebx
	movl	%ebx, 8(%r14)
	popq	%r14
	pushq	%r14
	movq	-8(%rbp), %r14
	movl	$0, %ebx
	movl	%ebx, 4(%r14)
	popq	%r14
	pushq	%r14
	movq	-8(%rbp), %r14
	movl	$0, %ebx
	movl	%ebx, 0(%r14)
	popq	%r14
	movq	-8(%rbp), %r15
	movq	%r15, %rax
	leave
	ret
panic:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$16, %rsp
	movq	%rdi, -8(%rbp)
	pushq	%rdx
	pushq	%rdi
	movq	$.strpanic36, %rax
	movq	%rax, %rdi
	pushq	%rsi
	movb	$114, %al
	movb	%al, %sil
	call	io.print
	popq	%rsi
	popq	%rdi
	popq	%rdx
	pushq	%rdx
	movq	-8(%rbp), %r15
	pushq	%rdi
	movq	%r15, %rax
	movq	%rax, %rdi
	movq	$0, %rsi
	call	io.print
	popq	%rdi
	popq	%rdx
	pushq	%rdx
	pushq	%rdi
	movq	$.strpanic37, %rax
	movq	%rax, %rdi
	movq	$0, %rsi
	call	io.print
	popq	%rdi
	popq	%rdx
	pushq	%rdx
	pushq	%rdi
	movl	$1, %eax
	movl	%eax, %edi
	call	sys_exit
	popq	%rdi
	popq	%rdx
	leave
	ret
assert:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$16, %rsp
	movb	%dil, -1(%rbp)
	movq	%rsi, -9(%rbp)
	movb	-1(%rbp), %r15b
	movzbl	%r15b, %eax
	xor	$1, %eax
	movb	%al, %al
	cmpb	$0, %al
	je	.Lassert38
	pushq	%rdx
	pushq	%rdi
	movq	$.strassert39, %rax
	movq	%rax, %rdi
	movq	$0, %rsi
	call	io.print
	popq	%rdi
	popq	%rdx
	pushq	%rdi
	pushq	%rdx
	movq	$0, %rdx
	movq	-9(%rbp), %r15
	movq	%r15, %rdi
	cmpq	%rdx, %rdi
	setne	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.Lassert40
	pushq	%rdx
	movq	-9(%rbp), %r15
	pushq	%rdi
	movq	%r15, %rax
	movq	%rax, %rdi
	movq	$0, %rsi
	call	io.print
	popq	%rdi
	popq	%rdx
.Lassert40:
	pushq	%rdx
	pushq	%rdi
	movq	$.strassert41, %rax
	movq	%rax, %rdi
	movq	$0, %rsi
	call	io.print
	popq	%rdi
	popq	%rdx
	pushq	%rdx
	pushq	%rdi
	movl	$1, %eax
	movl	%eax, %edi
	call	sys_exit
	popq	%rdi
	popq	%rdx
.Lassert38:
	movl	$0, %eax
	leave
	ret
sleep:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$32, %rsp
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	-4(%rbp), %r15d
	movl	%r15d, %edx
	movq	$0, %rdi
	add	%rdx, %rdi
	movq	%rdi, %rax
	popq	%rdx
	popq	%rdi
	movq	%rax, %rbx
	movq	%rbx, -16(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	-8(%rbp), %r15d
	movl	%r15d, %edx
	movq	$0, %rdi
	add	%rdx, %rdi
	movq	%rdi, %rax
	popq	%rdx
	popq	%rdi
	movq	%rax, %rbx
	movq	%rbx, -24(%rbp)
	pushq	%rdx
	pushq	%rdi
	movl	$16, %eax
	movl	%eax, %edi
	call	malloc
	popq	%rdi
	popq	%rdx
	movq	%rax, %rbx
	movq	%rbx, -32(%rbp)
	pushq	%r14
	movq	-32(%rbp), %r14
	movq	-16(%rbp), %r15
	movq	%r15, %rbx
	movq	%rbx, 0(%r14)
	popq	%r14
	pushq	%r14
	movq	-32(%rbp), %r14
	movq	-24(%rbp), %r15
	movq	%r15, %rbx
	movq	%rbx, 8(%r14)
	popq	%r14
	pushq	%rdx
	movq	-32(%rbp), %r15
	pushq	%rdi
	movq	%r15, %rax
	movq	%rax, %rdi
	movq	$0, %rsi
	call	sys_nsleep
	popq	%rdi
	popq	%rdx
	pushq	%rdx
	movq	-32(%rbp), %r15
	pushq	%rdi
	movq	%r15, %rax
	movq	%rax, %rdi
	call	free
	popq	%rdi
	popq	%rdx
	movl	$0, %eax
	leave
	ret


.data

.strassert41:
	.asciz	 "\n"
.strassert39:
	.asciz	 "Failed assert: "
.strpanic37:
	.asciz	 "\n"
.strpanic36:
	.asciz	 "Panic: "
.strinspectHeap35:
	.asciz	 "NULL\n"
.strinspectHeap34:
	.asciz	 "         "
.strinspectHeap33:
	.asciz	 "\n"
.strinspectHeap32:
	.asciz	 " overhead: "
.strinspectHeap31:
	.asciz	 " size: "
.strinspectHeap30:
	.asciz	 ": status: "
.strinspectHeap27:
	.asciz	 "HEAD --> "
.strinspectHeap26:
	.asciz	 "Heap:\n"
head:
.quad	0


.bss

