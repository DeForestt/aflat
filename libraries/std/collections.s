.global	pub_Array_forEach
.global	pub_Array_extend
.global	pub_Array_delete
.global	newArray
.global	pub_Array_at
.global	pub_Array_init
.global	pub_LinkedList_size
.global	pub_LinkedList_append
.global	pub_LinkedList_get
.global	pub_LinkedList_delete
.global	newNode
.global	pub_LinkedList_init
.global	newList


.text

newList:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$16, %rsp
	movl	$8, %eax
	movl	%eax, %edi
	call	malloc
	movq	%rax, -8(%rbp)
	lea	-8(%rbp), %rax
	movq	(%rax), %rax
	movq	%rax, %rdi
	call	pub_LinkedList_init
	movq	-8(%rbp), %rax
	leave
	ret
pub_LinkedList_init:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$16, %rsp
	movq	%rdi, -16(%rbp)
	movq	-16(%rbp), %rdx
	movq	$0, %rbx
	movq	%rbx, 0(%rdx)
	movl	$0, %eax
	leave
	ret
newNode:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$16, %rsp
	movl	$16, %eax
	movl	%eax, %edi
	call	malloc
	movq	%rax, -8(%rbp)
	movq	-8(%rbp), %rdx
	movq	$0, %rbx
	movq	%rbx, 8(%rdx)
	movq	-8(%rbp), %rax
	leave
	ret
pub_LinkedList_delete:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$32, %rsp
	movq	%rdi, -16(%rbp)
	movl	%esi, -20(%rbp)
	movq	-16(%rbp), %rdx
	movq	0(%rdx), %rbx
	movq	%rbx, -28(%rbp)
	movl	$0, -32(%rbp)
	jmp	.Ldelete1
.Ldelete0:
	movq	-28(%rbp), %rdx
	movq	0(%rdx), %rbx
	movq	%rbx, -28(%rbp)
	mov	$1, %edx
	mov	-32(%rbp), %eax
	add	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -32(%rbp)
.Ldelete1:
	movl	-32(%rbp), %eax
	movl	-20(%rbp), %ecx
	cmpl	%ecx, %eax
	jl	.Ldelete0
	movq	-28(%rbp), %rdx
	movq	0(%rdx), %rbx
	movq	%rbx, -28(%rbp)
	movl	$0, %eax
	leave
	ret
pub_LinkedList_get:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$48, %rsp
	movq	%rdi, -16(%rbp)
	movl	%esi, -20(%rbp)
	movl	-20(%rbp), %eax
	movl	$0, %ecx
	cmpl	%ecx, %eax
	jge	.Lget2
	movq	$0, %rax
	leave
	ret
.Lget2:
	movq	-16(%rbp), %rdx
	movq	0(%rdx), %rbx
	movq	%rbx, -28(%rbp)
	movq	-28(%rbp), %rbx
	movq	%rbx, -36(%rbp)
	movl	$0, -40(%rbp)
	jmp	.Lget6
.Lget5:
	movq	-36(%rbp), %rdx
	movq	0(%rdx), %rbx
	movq	%rbx, -36(%rbp)
	mov	$1, %edx
	mov	-40(%rbp), %eax
	add	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -40(%rbp)
.Lget6:
	movl	-40(%rbp), %eax
	movl	-20(%rbp), %ecx
	cmpl	%ecx, %eax
	jl	.Lget5
	movq	-36(%rbp), %rdx
	movq	8(%rdx), %rax
	leave
	ret
pub_LinkedList_append:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$48, %rsp
	movq	%rdi, -16(%rbp)
	movq	%rsi, -24(%rbp)
	call	newNode
	movq	%rax, -32(%rbp)
	movq	-32(%rbp), %rdx
	movq	-24(%rbp), %rbx
	movq	%rbx, 8(%rdx)
	movq	-16(%rbp), %rdx
	movq	0(%rdx), %rax
	movq	$0, %rcx
	cmpq	%rcx, %rax
	jne	.Lappend7
	movq	-16(%rbp), %rdx
	movq	-32(%rbp), %rbx
	movq	%rbx, 0(%rdx)
	movl	$0, %eax
	leave
	ret
.Lappend7:
	movq	-16(%rbp), %rdx
	movq	0(%rdx), %rbx
	movq	%rbx, -40(%rbp)
	jmp	.Lappend11
.Lappend10:
	movq	-40(%rbp), %rdx
	movq	0(%rdx), %rbx
	movq	%rbx, -40(%rbp)
.Lappend11:
	movq	-40(%rbp), %rdx
	movq	0(%rdx), %rax
	movq	$0, %rcx
	cmpq	%rcx, %rax
	jne	.Lappend10
	movq	-40(%rbp), %rdx
	movq	-32(%rbp), %rbx
	movq	%rbx, 0(%rdx)
	movl	$0, %eax
	leave
	ret
pub_LinkedList_size:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$32, %rsp
	movq	%rdi, -16(%rbp)
	movq	-16(%rbp), %rdx
	movq	0(%rdx), %rbx
	movq	%rbx, -24(%rbp)
	movl	$0, -28(%rbp)
	jmp	.Lsize13
.Lsize12:
	movq	-24(%rbp), %rdx
	movq	0(%rdx), %rbx
	movq	%rbx, -24(%rbp)
	mov	$1, %edx
	mov	-28(%rbp), %eax
	add	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -28(%rbp)
.Lsize13:
	movq	-24(%rbp), %rax
	movq	$0, %rcx
	cmpq	%rcx, %rax
	jne	.Lsize12
	movl	-28(%rbp), %eax
	leave
	ret
pub_Array_init:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$32, %rsp
	movq	%rdi, -24(%rbp)
	movl	%esi, -28(%rbp)
	movl	%edx, -32(%rbp)
	movq	-24(%rbp), %rdx
	movl	-32(%rbp), %ebx
	movl	%ebx, 12(%rdx)
	movq	-24(%rbp), %rdx
	movl	-28(%rbp), %ebx
	movl	%ebx, 0(%rdx)
	mov	-28(%rbp), %edx
	mov	-32(%rbp), %eax
	imul	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -32(%rbp)
	movl	-32(%rbp), %eax
	movl	%eax, %edi
	call	malloc
	movq	-24(%rbp), %rdx
	movq	%rax, %rbx
	movq	%rbx, 4(%rdx)
	movl	$0, %eax
	leave
	ret
pub_Array_at:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$48, %rsp
	movq	%rdi, -24(%rbp)
	movl	%esi, -28(%rbp)
	movq	-24(%rbp), %rdx
	movl	0(%rdx), %ebx
	movl	%ebx, -32(%rbp)
	mov	-32(%rbp), %edx
	mov	-28(%rbp), %eax
	imul	%edx, %eax
	movl	%eax, -36(%rbp)
	movq	-24(%rbp), %rdx
	movq	4(%rdx), %rbx
	movq	%rbx, -44(%rbp)
	mov	-36(%rbp), %edx
	mov	-44(%rbp), %rax
	add	%rdx, %rax
	movl	%eax, %ebx
	movl	%ebx, -44(%rbp)
	movq	-44(%rbp), %rax
	leave
	ret
newArray:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$16, %rsp
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	movl	$16, %eax
	movl	%eax, %edi
	call	malloc
	movq	%rax, -16(%rbp)
	lea	-16(%rbp), %rax
	movq	(%rax), %rax
	movq	%rax, %rdi
	movl	-4(%rbp), %eax
	movl	%eax, %esi
	movl	-8(%rbp), %eax
	movl	%eax, %edx
	call	pub_Array_init
	movq	-16(%rbp), %rax
	leave
	ret
pub_Array_delete:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$32, %rsp
	movq	%rdi, -24(%rbp)
	movq	-24(%rbp), %rdx
	movq	4(%rdx), %rax
	movq	%rax, %rdi
	call	free
	movq	-24(%rbp), %rax
	movq	%rax, %rdi
	call	free
	movl	$0, %eax
	leave
	ret
pub_Array_extend:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$48, %rsp
	movq	%rdi, -24(%rbp)
	movl	%esi, -28(%rbp)
	movq	-24(%rbp), %rdx
	movq	4(%rdx), %rbx
	movq	%rbx, -36(%rbp)
	movq	-24(%rbp), %rdx
	movl	12(%rdx), %ebx
	movl	%ebx, -40(%rbp)
	mov	-28(%rbp), %edx
	mov	-40(%rbp), %eax
	add	%edx, %eax
	movq	-24(%rbp), %rdx
	movl	%eax, %ebx
	movl	%ebx, 12(%rdx)
	movq	-24(%rbp), %rdx
	movl	0(%rdx), %ebx
	movl	%ebx, -44(%rbp)
	mov	-40(%rbp), %edx
	mov	-28(%rbp), %eax
	add	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -28(%rbp)
	mov	-44(%rbp), %edx
	mov	-28(%rbp), %eax
	imul	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -28(%rbp)
	movq	-36(%rbp), %rax
	movq	%rax, %rdi
	movl	-28(%rbp), %eax
	movl	%eax, %esi
	call	realloc
	movq	-24(%rbp), %rdx
	movq	%rax, %rbx
	movq	%rbx, 4(%rdx)
	movl	-28(%rbp), %eax
	leave
	ret
pub_Array_forEach:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$80, %rsp
	movq	%rdi, -24(%rbp)
	movq	%rsi, -32(%rbp)
	movq	%rdx, -40(%rbp)
	movq	-24(%rbp), %rdx
	movl	12(%rdx), %ebx
	movl	%ebx, -44(%rbp)
	movq	-24(%rbp), %rdx
	movl	0(%rdx), %ebx
	movl	%ebx, -56(%rbp)
	movl	$0, -68(%rbp)
	jmp	.LforEach15
.LforEach14:
	mov	-56(%rbp), %edx
	mov	-68(%rbp), %eax
	imul	%edx, %eax
	movl	%eax, -72(%rbp)
	movq	-24(%rbp), %rdx
	movq	4(%rdx), %rbx
	movq	%rbx, -64(%rbp)
	mov	-72(%rbp), %edx
	mov	-64(%rbp), %rax
	add	%rdx, %rax
	movl	%eax, %ebx
	movl	%ebx, -64(%rbp)
	movq	-32(%rbp), %rcx
	movq	-64(%rbp), %rax
	movq	%rax, %rdi
	movq	-40(%rbp), %rax
	movq	%rax, %rsi
	call	*%rcx
	mov	$1, %edx
	mov	-68(%rbp), %eax
	add	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -68(%rbp)
.LforEach15:
	movl	-68(%rbp), %eax
	movl	-44(%rbp), %ecx
	cmpl	%ecx, %eax
	jl	.LforEach14
	movl	$0, %eax
	leave
	ret


.data



.bss

