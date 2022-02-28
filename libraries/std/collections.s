.global	pub_Array_forEach
.global	pub_Array_extend
.global	pub_Array_delete
.global	newArray
.global	pub_Array_at
.global	pub_Array_init
.global	pub_LinkedList_forEach
.global	pub_LinkedList_size
.global	pub_LinkedList_insert
.global	pub_LinkedList_append
.global	pub_LinkedList_get
.global	pub_LinkedList_stitch
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
	leave
	ret
pub_LinkedList_init:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$16, %rsp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rdx
	movq	$0, %rbx
	movq	%rbx, 0(%rdx)
	movl	$0, %eax
	leave
	ret
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
	leave
	ret
pub_LinkedList_delete:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$32, %rsp
	movq	%rdi, -8(%rbp)
	movl	%esi, -12(%rbp)
	movq	-8(%rbp), %rdx
	movq	0(%rdx), %rbx
	movq	%rbx, -20(%rbp)
	movl	$0, -24(%rbp)
	jmp	.Ldelete1
.Ldelete0:
	movq	-20(%rbp), %rdx
	movq	0(%rdx), %rbx
	movq	%rbx, -20(%rbp)
	mov	$1, %edx
	mov	-24(%rbp), %eax
	add	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -24(%rbp)
.Ldelete1:
	movl	-24(%rbp), %eax
	movl	-12(%rbp), %ecx
	cmpl	%ecx, %eax
	jl	.Ldelete0
	movq	-20(%rbp), %rdx
	movq	0(%rdx), %rbx
	movq	%rbx, -20(%rbp)
	movl	$0, %eax
	leave
	ret
	leave
	ret
pub_LinkedList_stitch:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$32, %rsp
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	movq	-8(%rbp), %rdx
	movq	0(%rdx), %rbx
	movq	%rbx, -24(%rbp)
	jmp	.Lstitch3
.Lstitch2:
	movq	-24(%rbp), %rdx
	movq	0(%rdx), %rbx
	movq	%rbx, -24(%rbp)
.Lstitch3:
	movq	-24(%rbp), %rdx
	movq	0(%rdx), %rax
	movq	$0, %rcx
	cmpq	%rcx, %rax
	jne	.Lstitch2
	movq	-16(%rbp), %rdx
	movq	-24(%rbp), %rdx
	movq	0(%rdx), %rbx
	movq	%rbx, 0(%rdx)
	movl	$0, %eax
	leave
	ret
	leave
	ret
pub_LinkedList_get:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$32, %rsp
	movq	%rdi, -8(%rbp)
	movl	%esi, -12(%rbp)
	movl	-12(%rbp), %eax
	movl	$0, %ecx
	cmpl	%ecx, %eax
	jge	.Lget4
	movq	$0, %rax
	leave
	ret
.Lget4:
	movq	-8(%rbp), %rdx
	movq	0(%rdx), %rbx
	movq	%rbx, -20(%rbp)
	movq	-20(%rbp), %rbx
	movq	%rbx, -28(%rbp)
	movl	$0, -32(%rbp)
	jmp	.Lget8
.Lget7:
	movq	-28(%rbp), %rdx
	movq	0(%rdx), %rbx
	movq	%rbx, -28(%rbp)
	mov	$1, %edx
	mov	-32(%rbp), %eax
	add	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -32(%rbp)
.Lget8:
	movl	-32(%rbp), %eax
	movl	-12(%rbp), %ecx
	cmpl	%ecx, %eax
	jl	.Lget7
	movq	-28(%rbp), %rdx
	movq	8(%rdx), %rax
	leave
	ret
	leave
	ret
pub_LinkedList_append:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$32, %rsp
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	call	newNode
	movq	%rax, -24(%rbp)
	movq	-24(%rbp), %rdx
	movq	-16(%rbp), %rbx
	movq	%rbx, 8(%rdx)
	movq	-8(%rbp), %rdx
	movq	0(%rdx), %rax
	movq	$0, %rcx
	cmpq	%rcx, %rax
	jne	.Lappend9
	movq	-8(%rbp), %rdx
	movq	-24(%rbp), %rbx
	movq	%rbx, 0(%rdx)
	movl	$0, %eax
	leave
	ret
.Lappend9:
	movq	-8(%rbp), %rdx
	movq	0(%rdx), %rbx
	movq	%rbx, -32(%rbp)
	jmp	.Lappend13
.Lappend12:
	movq	-32(%rbp), %rdx
	movq	0(%rdx), %rbx
	movq	%rbx, -32(%rbp)
.Lappend13:
	movq	-32(%rbp), %rdx
	movq	0(%rdx), %rax
	movq	$0, %rcx
	cmpq	%rcx, %rax
	jne	.Lappend12
	movq	-32(%rbp), %rdx
	movq	-24(%rbp), %rbx
	movq	%rbx, 0(%rdx)
	movl	$0, %eax
	leave
	ret
	leave
	ret
pub_LinkedList_insert:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$48, %rsp
	movq	%rdi, -8(%rbp)
	movl	%esi, -12(%rbp)
	movq	%rdx, -20(%rbp)
	movl	-12(%rbp), %eax
	movl	$0, %ecx
	cmpl	%ecx, %eax
	jge	.Linsert14
	movl	$-1, %eax
	leave
	ret
.Linsert14:
	call	newNode
	movq	%rax, -28(%rbp)
	movq	-28(%rbp), %rdx
	movq	-20(%rbp), %rbx
	movq	%rbx, 8(%rdx)
	movq	-8(%rbp), %rdx
	movq	0(%rdx), %rbx
	movq	%rbx, -36(%rbp)
	movl	$0, -40(%rbp)
	jmp	.Linsert18
.Linsert17:
	movq	-36(%rbp), %rdx
	movq	0(%rdx), %rbx
	movq	%rbx, -36(%rbp)
	mov	$1, %edx
	mov	-40(%rbp), %eax
	add	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -40(%rbp)
.Linsert18:
	movl	-40(%rbp), %eax
	movl	-12(%rbp), %ecx
	cmpl	%ecx, %eax
	jl	.Linsert17
	movq	-36(%rbp), %rdx
	movq	0(%rdx), %rbx
	movq	%rbx, -44(%rbp)
	movq	-28(%rbp), %rdx
	movq	-44(%rbp), %rbx
	movq	%rbx, 0(%rdx)
	movq	-36(%rbp), %rdx
	movq	-28(%rbp), %rbx
	movq	%rbx, 0(%rdx)
	movl	$0, %eax
	leave
	ret
	leave
	ret
pub_LinkedList_size:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$32, %rsp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rdx
	movq	0(%rdx), %rbx
	movq	%rbx, -16(%rbp)
	movl	$0, -20(%rbp)
	jmp	.Lsize20
.Lsize19:
	movq	-16(%rbp), %rdx
	movq	0(%rdx), %rbx
	movq	%rbx, -16(%rbp)
	mov	$1, %edx
	mov	-20(%rbp), %eax
	add	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -20(%rbp)
.Lsize20:
	movq	-16(%rbp), %rax
	movq	$0, %rcx
	cmpq	%rcx, %rax
	jne	.Lsize19
	movl	-20(%rbp), %eax
	leave
	ret
	leave
	ret
pub_LinkedList_forEach:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$48, %rsp
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	movq	%rdx, -24(%rbp)
	movq	-8(%rbp), %rdx
	movq	0(%rdx), %rbx
	movq	%rbx, -32(%rbp)
	movl	$0, -36(%rbp)
	jmp	.LforEach22
.LforEach21:
	movq	-32(%rbp), %rdx
	movq	8(%rdx), %rbx
	movq	%rbx, -44(%rbp)
	movq	-16(%rbp), %rcx
	movq	-44(%rbp), %rax
	movq	%rax, %rdi
	movl	-36(%rbp), %eax
	movl	%eax, %esi
	movq	-24(%rbp), %rax
	movq	%rax, %rdx
	call	*%rcx
	movl	%eax, -48(%rbp)
	movl	-48(%rbp), %eax
	movl	$1, %ecx
	cmpl	%ecx, %eax
	jne	.LforEach23
	movl	$1, %eax
	leave
	ret
.LforEach23:
	movq	-32(%rbp), %rdx
	movq	0(%rdx), %rbx
	movq	%rbx, -32(%rbp)
	mov	$1, %edx
	mov	-36(%rbp), %eax
	add	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -36(%rbp)
.LforEach22:
	movq	-32(%rbp), %rax
	movq	$0, %rcx
	cmpq	%rcx, %rax
	jne	.LforEach21
	movl	$0, %eax
	leave
	ret
	leave
	ret
pub_Array_init:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$16, %rsp
	movq	%rdi, -8(%rbp)
	movl	%esi, -12(%rbp)
	movl	%edx, -16(%rbp)
	movq	-8(%rbp), %rdx
	movl	-16(%rbp), %ebx
	movl	%ebx, 12(%rdx)
	movq	-8(%rbp), %rdx
	movl	-12(%rbp), %ebx
	movl	%ebx, 0(%rdx)
	mov	-12(%rbp), %edx
	mov	-16(%rbp), %eax
	imul	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -16(%rbp)
	movl	-16(%rbp), %eax
	movl	%eax, %edi
	call	malloc
	movq	-8(%rbp), %rdx
	movq	%rax, %rbx
	movq	%rbx, 4(%rdx)
	movl	$0, %eax
	leave
	ret
	leave
	ret
pub_Array_at:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$32, %rsp
	movq	%rdi, -8(%rbp)
	movl	%esi, -12(%rbp)
	movq	-8(%rbp), %rdx
	movl	0(%rdx), %ebx
	movl	%ebx, -16(%rbp)
	mov	-16(%rbp), %edx
	mov	-12(%rbp), %eax
	imul	%edx, %eax
	movl	%eax, -20(%rbp)
	movq	-8(%rbp), %rdx
	movq	4(%rdx), %rbx
	movq	%rbx, -28(%rbp)
	mov	-20(%rbp), %edx
	mov	-28(%rbp), %rax
	add	%rdx, %rax
	movl	%eax, %ebx
	movl	%ebx, -28(%rbp)
	movq	-28(%rbp), %rax
	leave
	ret
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
	leave
	ret
pub_Array_delete:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$16, %rsp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rdx
	movq	4(%rdx), %rax
	movq	%rax, %rdi
	call	free
	movq	-8(%rbp), %rax
	movq	%rax, %rdi
	call	free
	movl	$0, %eax
	leave
	ret
	leave
	ret
pub_Array_extend:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$32, %rsp
	movq	%rdi, -8(%rbp)
	movl	%esi, -12(%rbp)
	movq	-8(%rbp), %rdx
	movq	4(%rdx), %rbx
	movq	%rbx, -20(%rbp)
	movq	-8(%rbp), %rdx
	movl	12(%rdx), %ebx
	movl	%ebx, -24(%rbp)
	mov	-12(%rbp), %edx
	mov	-24(%rbp), %eax
	add	%edx, %eax
	movq	-8(%rbp), %rdx
	movl	%eax, %ebx
	movl	%ebx, 12(%rdx)
	movq	-8(%rbp), %rdx
	movl	0(%rdx), %ebx
	movl	%ebx, -28(%rbp)
	mov	-24(%rbp), %edx
	mov	-12(%rbp), %eax
	add	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -12(%rbp)
	mov	-28(%rbp), %edx
	mov	-12(%rbp), %eax
	imul	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -12(%rbp)
	movq	-20(%rbp), %rax
	movq	%rax, %rdi
	movl	-12(%rbp), %eax
	movl	%eax, %esi
	call	realloc
	movq	-8(%rbp), %rdx
	movq	%rax, %rbx
	movq	%rbx, 4(%rdx)
	movl	-12(%rbp), %eax
	leave
	ret
	leave
	ret
pub_Array_forEach:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$48, %rsp
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	movq	%rdx, -24(%rbp)
	movq	-8(%rbp), %rdx
	movl	12(%rdx), %ebx
	movl	%ebx, -28(%rbp)
	movq	-8(%rbp), %rdx
	movl	0(%rdx), %ebx
	movl	%ebx, -40(%rbp)
	movl	$0, -52(%rbp)
	jmp	.LforEach27
.LforEach26:
	mov	-40(%rbp), %edx
	mov	-52(%rbp), %eax
	imul	%edx, %eax
	movl	%eax, -56(%rbp)
	movq	-8(%rbp), %rdx
	movq	4(%rdx), %rbx
	movq	%rbx, -48(%rbp)
	mov	-56(%rbp), %edx
	mov	-48(%rbp), %rax
	add	%rdx, %rax
	movl	%eax, %ebx
	movl	%ebx, -48(%rbp)
	movq	-16(%rbp), %rcx
	movq	-48(%rbp), %rax
	movq	%rax, %rdi
	movl	-52(%rbp), %eax
	movl	%eax, %esi
	movq	-24(%rbp), %rax
	movq	%rax, %rdx
	call	*%rcx
	movl	%eax, -60(%rbp)
	movl	-60(%rbp), %eax
	movl	$1, %ecx
	cmpl	%ecx, %eax
	jne	.LforEach28
	movl	$0, %eax
	leave
	ret
.LforEach28:
	mov	$1, %edx
	mov	-52(%rbp), %eax
	add	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -52(%rbp)
.LforEach27:
	movl	-52(%rbp), %eax
	movl	-28(%rbp), %ecx
	cmpl	%ecx, %eax
	jl	.LforEach26
	movl	$0, %eax
	leave
	ret
	leave
	ret


.data



.bss

