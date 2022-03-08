.global	pub_Array_extend
.global	pub_LinkedList_free
.global	pub_Array_delete
.global	newArray
.global	pub_Array_at
.global	pub_Array_init
.global	whereArray
.global	forEachArray
.global	pub_LinkedList_copy
.global	pub_LinkedList_map
.global	pub_LinkedList_size
.global	pub_LinkedList_insert
.global	pub_LinkedList_append
.global	pub_LinkedList_get
.global	pub_LinkedList_stitch
.global	pub_LinkedList_delete
.global	newNode
.global	pub_LinkedList_init
.global	whereLinkedList
.global	forEachLinkedList
.global	newList


.text

newList:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$16, %rsp
	pushq	%rdi
	movl	$24, %eax
	movl	%eax, %edi
	call	malloc
	popq	%rdi
	movq	%rax, %rbx
	movq	%rbx, -8(%rbp)
	lea	-8(%rbp), %rax
	pushq	%rdi
	movq	(%rax), %rax
	movq	%rax, %rdi
	call	pub_LinkedList_init
	popq	%rdi
	movq	-8(%rbp), %rax
	leave
	ret
	leave
	ret
forEachLinkedList:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$48, %rsp
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	movq	%rdx, -24(%rbp)
	movq	-8(%rbp), %rdx
	movq	16(%rdx), %rbx
	movq	%rbx, -32(%rbp)
	movl	$0, %ebx
	movl	%ebx, -36(%rbp)
	jmp	.LforEachLinkedList1
.LforEachLinkedList0:
	movq	-32(%rbp), %rdx
	movq	8(%rdx), %rbx
	movq	%rbx, -44(%rbp)
	movq	-16(%rbp), %r15
	pushq	%rdi
	movq	-44(%rbp), %rax
	movq	%rax, %rdi
	pushq	%rsi
	movl	-36(%rbp), %eax
	movl	%eax, %esi
	pushq	%rdx
	movq	-24(%rbp), %rax
	movq	%rax, %rdx
	call	*%r15
	popq	%rdx
	popq	%rsi
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -48(%rbp)
	movl	-48(%rbp), %eax
	movl	$1, %ecx
	cmpl	%ecx, %eax
	jne	.LforEachLinkedList2
	movl	$1, %eax
	leave
	ret
.LforEachLinkedList2:
	movq	-32(%rbp), %rdx
	movq	0(%rdx), %rbx
	movq	%rbx, -32(%rbp)
	mov	$1, %edx
	mov	-36(%rbp), %eax
	add	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -36(%rbp)
.LforEachLinkedList1:
	movq	-32(%rbp), %rax
	movq	$0, %rcx
	cmpq	%rcx, %rax
	jne	.LforEachLinkedList0
	movl	$0, %eax
	leave
	ret
	leave
	ret
whereLinkedList:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$80, %rsp
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	movq	%rdx, -24(%rbp)
	pushq	%rdi
	movl	$32, %eax
	movl	%eax, %edi
	call	malloc
	popq	%rdi
	pushq	%rdi
	movq	%rax, %rdi
	pushq	%rsi
	movl	$8, %eax
	movl	%eax, %esi
	pushq	%rdx
	movl	$0, %eax
	movl	%eax, %edx
	call	pub_Array_init
	popq	%rdx
	popq	%rsi
	popq	%rdi
	movq	%rax, %rbx
	movq	%rbx, -32(%rbp)
	movq	-8(%rbp), %rdx
	movq	16(%rdx), %rbx
	movq	%rbx, -40(%rbp)
	movl	$0, %ebx
	movl	%ebx, -44(%rbp)
	jmp	.LwhereLinkedList6
.LwhereLinkedList5:
	movq	-40(%rbp), %rdx
	movq	8(%rdx), %rbx
	movq	%rbx, -52(%rbp)
	movq	-16(%rbp), %r15
	pushq	%rdi
	movq	-52(%rbp), %rax
	movq	%rax, %rdi
	pushq	%rsi
	movl	-44(%rbp), %eax
	movl	%eax, %esi
	pushq	%rdx
	movq	-24(%rbp), %rax
	movq	%rax, %rdx
	call	*%r15
	popq	%rdx
	popq	%rsi
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -56(%rbp)
	movl	-56(%rbp), %eax
	movl	$1, %ecx
	cmpl	%ecx, %eax
	jne	.LwhereLinkedList7
	lea	-32(%rbp), %rax
	pushq	%rdi
	movq	(%rax), %rax
	movq	%rax, %rdi
	pushq	%rsi
	movl	$1, %eax
	movl	%eax, %esi
	call	pub_Array_extend
	popq	%rsi
	popq	%rdi
	movq	-32(%rbp), %rdx
	movl	28(%rdx), %ebx
	movl	%ebx, -60(%rbp)
	mov	$1, %edx
	mov	-60(%rbp), %eax
	sub	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -60(%rbp)
	lea	-32(%rbp), %rax
	pushq	%rdi
	movq	(%rax), %rax
	movq	%rax, %rdi
	pushq	%rsi
	movl	-60(%rbp), %eax
	movl	%eax, %esi
	call	pub_Array_at
	popq	%rsi
	popq	%rdi
	movq	%rax, %rbx
	movq	%rbx, -68(%rbp)
	movq	-68(%rbp), %rax
	movq	-52(%rbp), %rbx
	movq	%rbx, (%rax)
.LwhereLinkedList7:
	movq	-40(%rbp), %rdx
	movq	0(%rdx), %rbx
	movq	%rbx, -40(%rbp)
	mov	$1, %edx
	mov	-44(%rbp), %eax
	add	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -44(%rbp)
.LwhereLinkedList6:
	movq	-40(%rbp), %rax
	movq	$0, %rcx
	cmpq	%rcx, %rax
	jne	.LwhereLinkedList5
	movq	-32(%rbp), %rax
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
	movq	%rbx, 16(%rdx)
	movq	-8(%rbp), %rdx
	movq	$forEachLinkedList, %rbx
	movq	%rbx, 0(%rdx)
	movq	-8(%rbp), %rdx
	movq	$whereLinkedList, %rbx
	movq	%rbx, 8(%rdx)
	movq	-8(%rbp), %rax
	leave
	ret
	movq	-8(%rbp), %rax
	leave
	ret
newNode:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$16, %rsp
	pushq	%rdi
	movl	$16, %eax
	movl	%eax, %edi
	call	malloc
	popq	%rdi
	movq	%rax, %rbx
	movq	%rbx, -8(%rbp)
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
	movq	16(%rdx), %rbx
	movq	%rbx, -20(%rbp)
	movl	$0, %ebx
	movl	%ebx, -24(%rbp)
	jmp	.Ldelete11
.Ldelete10:
	movq	-20(%rbp), %rdx
	movq	0(%rdx), %rbx
	movq	%rbx, -20(%rbp)
	mov	$1, %edx
	mov	-24(%rbp), %eax
	add	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -24(%rbp)
.Ldelete11:
	movl	-24(%rbp), %eax
	movl	-12(%rbp), %ecx
	cmpl	%ecx, %eax
	jl	.Ldelete10
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
	movq	16(%rdx), %rbx
	movq	%rbx, -24(%rbp)
	jmp	.Lstitch13
.Lstitch12:
	movq	-24(%rbp), %rdx
	movq	0(%rdx), %rbx
	movq	%rbx, -24(%rbp)
.Lstitch13:
	movq	-24(%rbp), %rdx
	movq	0(%rdx), %rax
	movq	$0, %rcx
	cmpq	%rcx, %rax
	jne	.Lstitch12
	movq	-16(%rbp), %rdx
	movq	-24(%rbp), %rdx
	movq	16(%rdx), %rbx
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
	jge	.Lget14
	movq	$0, %rax
	leave
	ret
.Lget14:
	movq	-8(%rbp), %rdx
	movq	16(%rdx), %rbx
	movq	%rbx, -20(%rbp)
	movq	-20(%rbp), %rbx
	movq	%rbx, -28(%rbp)
	movl	$0, %ebx
	movl	%ebx, -32(%rbp)
	jmp	.Lget18
.Lget17:
	movq	-28(%rbp), %rdx
	movq	0(%rdx), %rbx
	movq	%rbx, -28(%rbp)
	mov	$1, %edx
	mov	-32(%rbp), %eax
	add	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -32(%rbp)
.Lget18:
	movl	-32(%rbp), %eax
	movl	-12(%rbp), %ecx
	cmpl	%ecx, %eax
	jl	.Lget17
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
	pushq	%rdi
	movl	$16, %eax
	movl	%eax, %edi
	call	malloc
	popq	%rdi
	movq	%rax, %rbx
	movq	%rbx, -24(%rbp)
	movq	-24(%rbp), %rdx
	movq	-16(%rbp), %rbx
	movq	%rbx, 8(%rdx)
	movq	-8(%rbp), %rdx
	movq	16(%rdx), %rax
	movq	$0, %rcx
	cmpq	%rcx, %rax
	jne	.Lappend19
	movq	-8(%rbp), %rdx
	movq	-24(%rbp), %rbx
	movq	%rbx, 16(%rdx)
	movl	$0, %eax
	leave
	ret
.Lappend19:
	movq	-8(%rbp), %rdx
	movq	16(%rdx), %rbx
	movq	%rbx, -32(%rbp)
	jmp	.Lappend23
.Lappend22:
	movq	-32(%rbp), %rdx
	movq	0(%rdx), %rbx
	movq	%rbx, -32(%rbp)
.Lappend23:
	movq	-32(%rbp), %rdx
	movq	0(%rdx), %rax
	movq	$0, %rcx
	cmpq	%rcx, %rax
	jne	.Lappend22
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
	jge	.Linsert24
	movl	$-1, %eax
	leave
	ret
.Linsert24:
	pushq	%rdi
	movl	$16, %eax
	movl	%eax, %edi
	call	malloc
	popq	%rdi
	movq	%rax, %rbx
	movq	%rbx, -28(%rbp)
	movq	-28(%rbp), %rdx
	movq	-20(%rbp), %rbx
	movq	%rbx, 8(%rdx)
	movq	-8(%rbp), %rdx
	movq	16(%rdx), %rbx
	movq	%rbx, -36(%rbp)
	movl	$0, %ebx
	movl	%ebx, -40(%rbp)
	jmp	.Linsert28
.Linsert27:
	movq	-36(%rbp), %rdx
	movq	0(%rdx), %rbx
	movq	%rbx, -36(%rbp)
	mov	$1, %edx
	mov	-40(%rbp), %eax
	add	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -40(%rbp)
.Linsert28:
	movl	-40(%rbp), %eax
	movl	-12(%rbp), %ecx
	cmpl	%ecx, %eax
	jl	.Linsert27
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
	movq	16(%rdx), %rbx
	movq	%rbx, -16(%rbp)
	movl	$0, %ebx
	movl	%ebx, -20(%rbp)
	jmp	.Lsize30
.Lsize29:
	movq	-16(%rbp), %rdx
	movq	0(%rdx), %rbx
	movq	%rbx, -16(%rbp)
	mov	$1, %edx
	mov	-20(%rbp), %eax
	add	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -20(%rbp)
.Lsize30:
	movq	-16(%rbp), %rax
	movq	$0, %rcx
	cmpq	%rcx, %rax
	jne	.Lsize29
	movl	-20(%rbp), %eax
	leave
	ret
	leave
	ret
pub_LinkedList_map:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$64, %rsp
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	movq	%rdx, -24(%rbp)
	pushq	%rdi
	movl	$24, %eax
	movl	%eax, %edi
	call	malloc
	popq	%rdi
	pushq	%rdi
	movq	%rax, %rdi
	call	pub_LinkedList_init
	popq	%rdi
	movq	%rax, %rbx
	movq	%rbx, -32(%rbp)
	movq	-8(%rbp), %rdx
	movq	16(%rdx), %rbx
	movq	%rbx, -40(%rbp)
	movl	$0, %ebx
	movl	%ebx, -44(%rbp)
	jmp	.Lmap32
.Lmap31:
	movq	-40(%rbp), %rdx
	movq	8(%rdx), %rbx
	movq	%rbx, -52(%rbp)
	movq	-16(%rbp), %r15
	pushq	%rdi
	movq	-52(%rbp), %rax
	movq	%rax, %rdi
	pushq	%rsi
	movl	-44(%rbp), %eax
	movl	%eax, %esi
	pushq	%rdx
	movq	-24(%rbp), %rax
	movq	%rax, %rdx
	call	*%r15
	popq	%rdx
	popq	%rsi
	popq	%rdi
	movq	%rax, %rbx
	movq	%rbx, -60(%rbp)
	lea	-32(%rbp), %rax
	pushq	%rdi
	movq	(%rax), %rax
	movq	%rax, %rdi
	pushq	%rsi
	movq	-60(%rbp), %rax
	movq	%rax, %rsi
	call	pub_LinkedList_append
	popq	%rsi
	popq	%rdi
	movq	-40(%rbp), %rdx
	movq	0(%rdx), %rbx
	movq	%rbx, -40(%rbp)
	mov	$1, %edx
	mov	-44(%rbp), %eax
	add	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -44(%rbp)
.Lmap32:
	movq	-40(%rbp), %rax
	movq	$0, %rcx
	cmpq	%rcx, %rax
	jne	.Lmap31
	movq	-32(%rbp), %rax
	leave
	ret
	leave
	ret
pub_LinkedList_copy:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$16, %rsp
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	lea	-8(%rbp), %rax
	pushq	%rdi
	movq	(%rax), %rax
	movq	%rax, %rdi
	call	pub_LinkedList_free
	popq	%rdi
	movq	-16(%rbp), %rax
	leave
	ret
	leave
	ret
forEachArray:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$64, %rsp
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	movq	%rdx, -24(%rbp)
	movq	-8(%rbp), %rdx
	movl	28(%rdx), %ebx
	movl	%ebx, -28(%rbp)
	movq	-8(%rbp), %rdx
	movl	16(%rdx), %ebx
	movl	%ebx, -40(%rbp)
	movl	$0, %ebx
	movl	%ebx, -52(%rbp)
	jmp	.LforEachArray34
.LforEachArray33:
	mov	-40(%rbp), %edx
	mov	-52(%rbp), %eax
	imul	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -56(%rbp)
	movq	-8(%rbp), %rdx
	movq	20(%rdx), %rbx
	movq	%rbx, -48(%rbp)
	mov	-56(%rbp), %edx
	mov	-48(%rbp), %rax
	add	%rdx, %rax
	movl	%eax, %ebx
	movl	%ebx, -48(%rbp)
	movq	-16(%rbp), %r15
	pushq	%rdi
	movq	-48(%rbp), %rax
	movq	%rax, %rdi
	pushq	%rsi
	movl	-52(%rbp), %eax
	movl	%eax, %esi
	pushq	%rdx
	movq	-24(%rbp), %rax
	movq	%rax, %rdx
	call	*%r15
	popq	%rdx
	popq	%rsi
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -60(%rbp)
	movl	-60(%rbp), %eax
	movl	$1, %ecx
	cmpl	%ecx, %eax
	jne	.LforEachArray35
	movl	$0, %eax
	leave
	ret
.LforEachArray35:
	mov	$1, %edx
	mov	-52(%rbp), %eax
	add	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -52(%rbp)
.LforEachArray34:
	movl	-52(%rbp), %eax
	movl	-28(%rbp), %ecx
	cmpl	%ecx, %eax
	jl	.LforEachArray33
	movl	$0, %eax
	leave
	ret
	leave
	ret
whereArray:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$80, %rsp
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	movq	%rdx, -24(%rbp)
	pushq	%rdi
	movl	$32, %eax
	movl	%eax, %edi
	call	malloc
	popq	%rdi
	pushq	%rdi
	movq	%rax, %rdi
	pushq	%rsi
	movl	$8, %eax
	movl	%eax, %esi
	pushq	%rdx
	movl	$0, %eax
	movl	%eax, %edx
	call	pub_Array_init
	popq	%rdx
	popq	%rsi
	popq	%rdi
	movq	%rax, %rbx
	movq	%rbx, -32(%rbp)
	movq	-8(%rbp), %rdx
	movl	16(%rdx), %ebx
	movl	%ebx, -40(%rbp)
	movq	-8(%rbp), %rdx
	movl	28(%rdx), %ebx
	movl	%ebx, -36(%rbp)
	movl	$0, %ebx
	movl	%ebx, -44(%rbp)
	jmp	.LwhereArray39
.LwhereArray38:
	mov	-40(%rbp), %edx
	mov	-44(%rbp), %eax
	imul	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -48(%rbp)
	movq	-8(%rbp), %rdx
	movq	20(%rdx), %rbx
	movq	%rbx, -56(%rbp)
	mov	-48(%rbp), %edx
	mov	-56(%rbp), %rax
	add	%rdx, %rax
	movl	%eax, %ebx
	movl	%ebx, -56(%rbp)
	movq	-56(%rbp), %rbx
	movq	%rbx, -64(%rbp)
	movq	-16(%rbp), %r15
	pushq	%rdi
	movq	-56(%rbp), %rax
	movq	%rax, %rdi
	pushq	%rsi
	movl	-44(%rbp), %eax
	movl	%eax, %esi
	pushq	%rdx
	movq	-24(%rbp), %rax
	movq	%rax, %rdx
	call	*%r15
	popq	%rdx
	popq	%rsi
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -68(%rbp)
	movl	-68(%rbp), %eax
	movl	$1, %ecx
	cmpl	%ecx, %eax
	jne	.LwhereArray40
	lea	-32(%rbp), %rax
	pushq	%rdi
	movq	(%rax), %rax
	movq	%rax, %rdi
	pushq	%rsi
	movl	$1, %eax
	movl	%eax, %esi
	call	pub_Array_extend
	popq	%rsi
	popq	%rdi
	movq	-32(%rbp), %rdx
	movl	28(%rdx), %ebx
	movl	%ebx, -72(%rbp)
	mov	$1, %edx
	mov	-72(%rbp), %eax
	sub	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -72(%rbp)
	lea	-32(%rbp), %rax
	pushq	%rdi
	movq	(%rax), %rax
	movq	%rax, %rdi
	pushq	%rsi
	movl	-72(%rbp), %eax
	movl	%eax, %esi
	call	pub_Array_at
	popq	%rsi
	popq	%rdi
	movq	%rax, %rbx
	movq	%rbx, -80(%rbp)
	movq	-80(%rbp), %rax
	movq	-64(%rbp), %rbx
	movq	%rbx, (%rax)
.LwhereArray40:
	mov	$1, %edx
	mov	-44(%rbp), %eax
	add	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -44(%rbp)
.LwhereArray39:
	movl	-44(%rbp), %eax
	movl	-36(%rbp), %ecx
	cmpl	%ecx, %eax
	jl	.LwhereArray38
	movq	-32(%rbp), %rax
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
	movl	%ebx, 28(%rdx)
	movq	-8(%rbp), %rdx
	movl	-12(%rbp), %ebx
	movl	%ebx, 16(%rdx)
	mov	-12(%rbp), %edx
	mov	-16(%rbp), %eax
	imul	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -16(%rbp)
	pushq	%rdi
	movl	-16(%rbp), %eax
	movl	%eax, %edi
	call	malloc
	popq	%rdi
	movq	-8(%rbp), %rdx
	movq	%rax, %rbx
	movq	%rbx, 20(%rdx)
	movq	-8(%rbp), %rdx
	movq	$forEachArray, %rbx
	movq	%rbx, 0(%rdx)
	movq	-8(%rbp), %rdx
	movq	$whereArray, %rbx
	movq	%rbx, 8(%rdx)
	movq	-8(%rbp), %rax
	leave
	ret
	movq	-8(%rbp), %rax
	leave
	ret
pub_Array_at:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$32, %rsp
	movq	%rdi, -8(%rbp)
	movl	%esi, -12(%rbp)
	movq	-8(%rbp), %rdx
	movl	16(%rdx), %ebx
	movl	%ebx, -16(%rbp)
	mov	-16(%rbp), %edx
	mov	-12(%rbp), %eax
	imul	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -20(%rbp)
	movq	-8(%rbp), %rdx
	movq	20(%rdx), %rbx
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
	pushq	%rdi
	movl	$32, %eax
	movl	%eax, %edi
	call	malloc
	popq	%rdi
	movq	%rax, %rbx
	movq	%rbx, -16(%rbp)
	lea	-16(%rbp), %rax
	pushq	%rdi
	movq	(%rax), %rax
	movq	%rax, %rdi
	pushq	%rsi
	movl	-4(%rbp), %eax
	movl	%eax, %esi
	pushq	%rdx
	movl	-8(%rbp), %eax
	movl	%eax, %edx
	call	pub_Array_init
	popq	%rdx
	popq	%rsi
	popq	%rdi
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
	pushq	%rdi
	movq	20(%rdx), %rax
	movq	%rax, %rdi
	call	free
	popq	%rdi
	pushq	%rdi
	movq	-8(%rbp), %rax
	movq	%rax, %rdi
	call	free
	popq	%rdi
	movl	$0, %eax
	leave
	ret
	leave
	ret
pub_LinkedList_free:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$32, %rsp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rdx
	movq	16(%rdx), %rbx
	movq	%rbx, -16(%rbp)
	jmp	.Lfree44
.Lfree43:
	movq	-16(%rbp), %rdx
	movq	0(%rdx), %rbx
	movq	%rbx, -24(%rbp)
	movq	-16(%rbp), %rdx
	pushq	%rdi
	movq	8(%rdx), %rax
	movq	%rax, %rdi
	call	free
	popq	%rdi
	pushq	%rdi
	movq	-16(%rbp), %rax
	movq	%rax, %rdi
	call	free
	popq	%rdi
	movq	-24(%rbp), %rbx
	movq	%rbx, -16(%rbp)
.Lfree44:
	movq	-16(%rbp), %rax
	movq	$0, %rcx
	cmpq	%rcx, %rax
	jne	.Lfree43
	pushq	%rdi
	movq	-8(%rbp), %rax
	movq	%rax, %rdi
	call	free
	popq	%rdi
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
	movq	20(%rdx), %rbx
	movq	%rbx, -20(%rbp)
	movq	-8(%rbp), %rdx
	movl	28(%rdx), %ebx
	movl	%ebx, -24(%rbp)
	mov	-12(%rbp), %edx
	mov	-24(%rbp), %eax
	add	%edx, %eax
	movq	-8(%rbp), %rdx
	movl	%eax, %ebx
	movl	%ebx, 28(%rdx)
	movq	-8(%rbp), %rdx
	movl	16(%rdx), %ebx
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
	pushq	%rdi
	movq	-20(%rbp), %rax
	movq	%rax, %rdi
	pushq	%rsi
	movl	-12(%rbp), %eax
	movl	%eax, %esi
	call	realloc
	popq	%rsi
	popq	%rdi
	movq	-8(%rbp), %rdx
	movq	%rax, %rbx
	movq	%rbx, 20(%rdx)
	movq	-8(%rbp), %rdx
	movq	20(%rdx), %rax
	movq	$0, %rcx
	cmpq	%rcx, %rax
	jne	.Lextend45
	pushq	%rdi
	movq	$.strArray.extend48, %rax
	movq	%rax, %rdi
	call	print
	popq	%rdi
.Lextend45:
	movl	-12(%rbp), %eax
	leave
	ret
	leave
	ret


.data

.strArray.extend48:
	.asciz	 "badrealloc"


.bss

