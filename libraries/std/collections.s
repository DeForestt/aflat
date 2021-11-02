.global	pub_LinkedList_size
.global	pub_LinkedList_append
.global	pub_LinkedList_get
.global	pub_LinkedList_delete
.global	newNode
.global	newList


.text

newList:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$16, %rsp
	movl	$12, %eax
	movl	%eax, %edi
	call	malloc
	movq	%rax, -8(%rbp)
	movq	-8(%rbp), %rdx
	movq	$0, %rbx
	movq	%rbx, 0(%rdx)
	movq	-8(%rbp), %rax
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
	subq	$48, %rsp
	movq	%rdi, -20(%rbp)
	movl	%esi, -24(%rbp)
	movq	-20(%rbp), %rdx
	movq	0(%rdx), %rbx
	movq	%rbx, -32(%rbp)
	movl	$0, -36(%rbp)
	jmp	.Ldelete1
.Ldelete0:
	movq	-32(%rbp), %rdx
	movq	0(%rdx), %rbx
	movq	%rbx, -32(%rbp)
	mov	$1, %edx
	mov	-36(%rbp), %eax
	add	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -36(%rbp)
.Ldelete1:
	movl	-36(%rbp), %eax
	movl	-24(%rbp), %ecx
	cmpl	%ecx, %eax
	jl	.Ldelete0
	movq	-32(%rbp), %rdx
	movq	0(%rdx), %rbx
	movq	%rbx, -32(%rbp)
	movl	$0, %eax
	leave
	ret
pub_LinkedList_get:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$48, %rsp
	movq	%rdi, -20(%rbp)
	movl	%esi, -24(%rbp)
	movl	-24(%rbp), %eax
	movl	$0, %ecx
	cmpl	%ecx, %eax
	jge	.Lget2
	movq	$0, %rax
	leave
	ret
.Lget2:
	movq	-20(%rbp), %rdx
	movq	0(%rdx), %rbx
	movq	%rbx, -32(%rbp)
	movq	-32(%rbp), %rbx
	movq	%rbx, -40(%rbp)
	movl	$0, -44(%rbp)
	jmp	.Lget4
.Lget3:
	movq	-40(%rbp), %rdx
	movq	0(%rdx), %rbx
	movq	%rbx, -40(%rbp)
	mov	$1, %edx
	mov	-44(%rbp), %eax
	add	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -44(%rbp)
.Lget4:
	movl	-44(%rbp), %eax
	movl	-24(%rbp), %ecx
	cmpl	%ecx, %eax
	jl	.Lget3
	movq	-40(%rbp), %rdx
	movq	8(%rdx), %rax
	leave
	ret
pub_LinkedList_append:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$48, %rsp
	movq	%rdi, -20(%rbp)
	movq	%rsi, -28(%rbp)
	call	newNode
	movq	%rax, -36(%rbp)
	movq	-36(%rbp), %rdx
	movq	-28(%rbp), %rbx
	movq	%rbx, 8(%rdx)
	movq	-20(%rbp), %rdx
	movq	0(%rdx), %rax
	movq	$0, %rcx
	cmpq	%rcx, %rax
	jne	.Lappend5
	movq	-20(%rbp), %rdx
	movq	-36(%rbp), %rbx
	movq	%rbx, 0(%rdx)
	movl	$0, %eax
	leave
	ret
.Lappend5:
	movq	-20(%rbp), %rdx
	movq	0(%rdx), %rbx
	movq	%rbx, -44(%rbp)
	jmp	.Lappend7
.Lappend6:
	movq	-44(%rbp), %rdx
	movq	0(%rdx), %rbx
	movq	%rbx, -44(%rbp)
.Lappend7:
	movq	-44(%rbp), %rdx
	movq	0(%rdx), %rax
	movq	$0, %rcx
	cmpq	%rcx, %rax
	jne	.Lappend6
	movq	-44(%rbp), %rdx
	movq	-36(%rbp), %rbx
	movq	%rbx, 0(%rdx)
	movl	$0, %eax
	leave
	ret
pub_LinkedList_size:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$32, %rsp
	movq	%rdi, -20(%rbp)
	movq	-20(%rbp), %rdx
	movq	0(%rdx), %rbx
	movq	%rbx, -28(%rbp)
	movl	$0, -32(%rbp)
	jmp	.Lsize9
.Lsize8:
	movq	-28(%rbp), %rdx
	movq	0(%rdx), %rbx
	movq	%rbx, -28(%rbp)
	mov	$1, %edx
	mov	-32(%rbp), %eax
	add	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -32(%rbp)
.Lsize9:
	movq	-28(%rbp), %rax
	movq	$0, %rcx
	cmpq	%rcx, %rax
	jne	.Lsize8
	movl	-32(%rbp), %eax
	leave
	ret


.data



.bss

