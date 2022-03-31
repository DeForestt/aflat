.global	pub_Array_extend
.global	pub_LinkedList_reverse
.global	pub_LinkedList_erace
.global	pub_Array_delete
.global	newArray
.global	pub_Array_at
.global	pub_Array_init
.global	whereArray
.global	pub_LinkedList_find
.global	forEachArray
.global	pub_LinkedList_copy
.global	pub_LinkedList_map
.global	pub_LinkedList_sort
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
	pushq	%rbx
	subq	$16, %rsp
	pushq	%rdx
	pushq	%rdi
	movl	$24, %eax
	movl	%eax, %edi
	call	malloc
	popq	%rdi
	popq	%rdx
	movq	%rax, %rbx
	movq	%rbx, -8(%rbp)
	pushq	%rdx
	lea	-8(%rbp), %rax
	pushq	%rdi
	movq	(%rax), %rax
	movq	%rax, %rdi
	call	pub_LinkedList_init
	popq	%rdi
	popq	%rdx
	movq	-8(%rbp), %rax
	leave
	ret
forEachLinkedList:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$48, %rsp
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	movq	%rdx, -24(%rbp)
	movq	-8(%rbp), %r14
	movq	16(%r14), %rbx
	movq	%rbx, -32(%rbp)
	movl	$0, %ebx
	movl	%ebx, -36(%rbp)
	jmp	.LforEachLinkedList1
.LforEachLinkedList0:
	movq	-32(%rbp), %r14
	movq	8(%r14), %rbx
	movq	%rbx, -44(%rbp)
	pushq	%rdx
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
	popq	%rdx
	movl	%eax, %ebx
	movl	%ebx, -48(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movl	-48(%rbp), %edi
	cmpl	%edx, %edi
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.LforEachLinkedList2
	movl	$1, %eax
	leave
	ret
.LforEachLinkedList2:
	movq	-32(%rbp), %r14
	movq	0(%r14), %rbx
	movq	%rbx, -32(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movl	-36(%rbp), %edi
	add	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -36(%rbp)
.LforEachLinkedList1:
	pushq	%rdi
	pushq	%rdx
	movq	$0, %rdx
	movq	-32(%rbp), %rdi
	cmpq	%rdx, %rdi
	setne	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$1, %al
	je	.LforEachLinkedList0
	movl	$0, %eax
	leave
	ret
whereLinkedList:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$64, %rsp
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	movq	%rdx, -24(%rbp)
	pushq	%rdx
	pushq	%rdi
	movl	$24, %eax
	movl	%eax, %edi
	call	malloc
	popq	%rdi
	popq	%rdx
	pushq	%rdi
	movq	%rax, %rdi
	pushq	%rdx
	call	pub_LinkedList_init
	popq	%rdi
	popq	%rdx
	movq	%rax, %rbx
	movq	%rbx, -32(%rbp)
	movq	-8(%rbp), %r14
	movq	16(%r14), %rbx
	movq	%rbx, -40(%rbp)
	movl	$0, %ebx
	movl	%ebx, -44(%rbp)
	jmp	.LwhereLinkedList4
.LwhereLinkedList3:
	movq	-40(%rbp), %r14
	movq	8(%r14), %rbx
	movq	%rbx, -52(%rbp)
	pushq	%rdx
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
	popq	%rdx
	movb	%al, %bl
	movb	%bl, -53(%rbp)
	pushq	%rdi
	pushq	%rdx
	movb	$1, %dl
	movb	-53(%rbp), %dil
	cmpb	%dl, %dil
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.LwhereLinkedList5
	pushq	%rdx
	lea	-32(%rbp), %rax
	pushq	%rdi
	movq	(%rax), %rax
	movq	%rax, %rdi
	pushq	%rsi
	movq	-52(%rbp), %rax
	movq	%rax, %rsi
	call	pub_LinkedList_append
	popq	%rsi
	popq	%rdi
	popq	%rdx
.LwhereLinkedList5:
	movq	-40(%rbp), %r14
	movq	0(%r14), %rbx
	movq	%rbx, -40(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movl	-44(%rbp), %edi
	add	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -44(%rbp)
.LwhereLinkedList4:
	pushq	%rdi
	pushq	%rdx
	movq	$0, %rdx
	movq	-40(%rbp), %rdi
	cmpq	%rdx, %rdi
	setne	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$1, %al
	je	.LwhereLinkedList3
	movq	-32(%rbp), %rax
	leave
	ret
pub_LinkedList_init:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
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
newNode:
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
	movq	-8(%rbp), %rdx
	movq	$0, %rbx
	movq	%rbx, 8(%rdx)
	movq	-8(%rbp), %rax
	leave
	ret
pub_LinkedList_delete:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$32, %rsp
	movq	%rdi, -8(%rbp)
	movl	%esi, -12(%rbp)
	pushq	%rdi
	pushq	%rdx
	pushq	%rdi
	pushq	%rdx
	pushq	%rdx
	lea	-8(%rbp), %rax
	pushq	%rdi
	movq	(%rax), %rax
	movq	%rax, %rdi
	call	pub_LinkedList_size
	popq	%rdi
	popq	%rdx
	movl	%eax, %edx
	movl	-12(%rbp), %edi
	cmpl	%edx, %edi
	setg	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %dl
	pushq	%rdi
	pushq	%rdx
	movl	$0, %edx
	movl	-12(%rbp), %edi
	cmpl	%edx, %edi
	setl	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %dil
	orb	%dl, %dil
	movb	%dil, %al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.Ldelete6
	movl	$-1, %eax
	leave
	ret
.Ldelete6:
	movq	-8(%rbp), %r14
	movq	16(%r14), %rbx
	movq	%rbx, -20(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	$0, %edx
	movl	-12(%rbp), %edi
	cmpl	%edx, %edi
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.Ldelete7
	movq	-20(%rbp), %r14
	movq	-8(%rbp), %rdx
	movq	0(%r14), %rbx
	movq	%rbx, 16(%rdx)
	movl	$0, %eax
	leave
	ret
.Ldelete7:
	movl	$0, %ebx
	movl	%ebx, -24(%rbp)
	jmp	.Ldelete9
.Ldelete8:
	pushq	%rdi
	pushq	%rdx
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movl	-12(%rbp), %edi
	sub	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %edx
	movl	-24(%rbp), %edi
	cmpl	%edx, %edi
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.Ldelete10
	movq	-20(%rbp), %r14
	movq	0(%r14), %rbx
	movq	%rbx, -32(%rbp)
	movq	-32(%rbp), %r14
	movq	-20(%rbp), %rdx
	movq	0(%r14), %rbx
	movq	%rbx, 0(%rdx)
	movl	$0, %eax
	leave
	ret
.Ldelete10:
	movq	-20(%rbp), %r14
	movq	0(%r14), %rbx
	movq	%rbx, -20(%rbp)
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
.Ldelete9:
	pushq	%rdi
	pushq	%rdx
	movq	$0, %rdx
	movq	-20(%rbp), %rdi
	cmpq	%rdx, %rdi
	setne	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$1, %al
	je	.Ldelete8
	movl	$-1, %eax
	leave
	ret
pub_LinkedList_stitch:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$32, %rsp
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	movq	-8(%rbp), %r14
	movq	16(%r14), %rbx
	movq	%rbx, -24(%rbp)
	jmp	.Lstitch12
.Lstitch11:
	movq	-24(%rbp), %r14
	movq	0(%r14), %rbx
	movq	%rbx, -24(%rbp)
.Lstitch12:
	pushq	%rdi
	pushq	%rdx
	movq	$0, %rdx
	movq	-24(%rbp), %r14
	movq	0(%r14), %rdi
	cmpq	%rdx, %rdi
	setne	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$1, %al
	je	.Lstitch11
	movq	-16(%rbp), %r14
	movq	-24(%rbp), %rdx
	movq	16(%r14), %rbx
	movq	%rbx, 0(%rdx)
	movl	$0, %eax
	leave
	ret
pub_LinkedList_get:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$32, %rsp
	movq	%rdi, -8(%rbp)
	movl	%esi, -12(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	$0, %edx
	movl	-12(%rbp), %edi
	cmpl	%edx, %edi
	setl	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.Lget13
	movq	$0, %rax
	leave
	ret
.Lget13:
	movq	-8(%rbp), %r14
	movq	16(%r14), %rbx
	movq	%rbx, -20(%rbp)
	movq	-20(%rbp), %rbx
	movq	%rbx, -28(%rbp)
	movl	$0, %ebx
	movl	%ebx, -32(%rbp)
	jmp	.Lget15
.Lget14:
	movq	-28(%rbp), %r14
	movq	0(%r14), %rbx
	movq	%rbx, -28(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movl	-32(%rbp), %edi
	add	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -32(%rbp)
.Lget15:
	pushq	%rdi
	pushq	%rdx
	movl	-12(%rbp), %edx
	movl	-32(%rbp), %edi
	cmpl	%edx, %edi
	setl	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$1, %al
	je	.Lget14
	movq	-28(%rbp), %r14
	movq	8(%r14), %rax
	leave
	ret
pub_LinkedList_append:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$48, %rsp
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	pushq	%rdx
	pushq	%rdi
	movl	$16, %eax
	movl	%eax, %edi
	call	malloc
	popq	%rdi
	popq	%rdx
	movq	%rax, %rbx
	movq	%rbx, -24(%rbp)
	movq	-24(%rbp), %rdx
	movq	-16(%rbp), %rbx
	movq	%rbx, 8(%rdx)
	movq	-24(%rbp), %rdx
	movq	$0, %rbx
	movq	%rbx, 0(%rdx)
	movq	-8(%rbp), %r14
	movq	16(%r14), %rbx
	movq	%rbx, -32(%rbp)
	pushq	%rdi
	pushq	%rdx
	movq	$0, %rdx
	movq	-32(%rbp), %rdi
	cmpq	%rdx, %rdi
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.Lappend16
	movq	-8(%rbp), %rdx
	movq	-24(%rbp), %rbx
	movq	%rbx, 16(%rdx)
	movl	$0, %eax
	leave
	ret
.Lappend16:
	movq	-8(%rbp), %r14
	movq	16(%r14), %rbx
	movq	%rbx, -40(%rbp)
	movq	-40(%rbp), %r14
	movq	0(%r14), %rbx
	movq	%rbx, -48(%rbp)
	jmp	.Lappend18
.Lappend17:
	movq	-48(%rbp), %rbx
	movq	%rbx, -40(%rbp)
	movq	-40(%rbp), %r14
	movq	0(%r14), %rbx
	movq	%rbx, -48(%rbp)
.Lappend18:
	pushq	%rdi
	pushq	%rdx
	movq	$0, %rdx
	movq	-48(%rbp), %rdi
	cmpq	%rdx, %rdi
	setne	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$1, %al
	je	.Lappend17
	movq	-40(%rbp), %rdx
	movq	-24(%rbp), %rbx
	movq	%rbx, 0(%rdx)
	movl	$0, %eax
	leave
	ret
pub_LinkedList_insert:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$48, %rsp
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	movl	%edx, -20(%rbp)
	pushq	%rdi
	pushq	%rdx
	pushq	%rdi
	pushq	%rdx
	pushq	%rdx
	lea	-8(%rbp), %rax
	pushq	%rdi
	movq	(%rax), %rax
	movq	%rax, %rdi
	call	pub_LinkedList_size
	popq	%rdi
	popq	%rdx
	movl	%eax, %edx
	movl	-20(%rbp), %edi
	cmpl	%edx, %edi
	setg	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %dl
	pushq	%rdi
	pushq	%rdx
	movl	$0, %edx
	movl	-20(%rbp), %edi
	cmpl	%edx, %edi
	setl	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %dil
	orb	%dl, %dil
	movb	%dil, %al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.Linsert19
	movl	$-1, %eax
	leave
	ret
.Linsert19:
	pushq	%rdx
	pushq	%rdi
	movl	$16, %eax
	movl	%eax, %edi
	call	malloc
	popq	%rdi
	popq	%rdx
	movq	%rax, %rbx
	movq	%rbx, -28(%rbp)
	movq	-28(%rbp), %rdx
	movq	-16(%rbp), %rbx
	movq	%rbx, 8(%rdx)
	movq	-28(%rbp), %rdx
	movq	$0, %rbx
	movq	%rbx, 0(%rdx)
	pushq	%rdi
	pushq	%rdx
	movl	$0, %edx
	movl	-20(%rbp), %edi
	cmpl	%edx, %edi
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.Linsert20
	movq	-8(%rbp), %r14
	movq	-28(%rbp), %rdx
	movq	16(%r14), %rbx
	movq	%rbx, 0(%rdx)
	movq	-8(%rbp), %rdx
	movq	-28(%rbp), %rbx
	movq	%rbx, 16(%rdx)
	movl	$0, %eax
	leave
	ret
.Linsert20:
	movq	-8(%rbp), %r14
	movq	16(%r14), %rbx
	movq	%rbx, -36(%rbp)
	movl	$0, %ebx
	movl	%ebx, -40(%rbp)
	jmp	.Linsert22
.Linsert21:
	movq	-36(%rbp), %r14
	movq	0(%r14), %rbx
	movq	%rbx, -36(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movl	-40(%rbp), %edi
	add	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -40(%rbp)
.Linsert22:
	pushq	%rdi
	pushq	%rdx
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movl	-20(%rbp), %edi
	sub	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %edx
	movl	-40(%rbp), %edi
	cmpl	%edx, %edi
	setl	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$1, %al
	je	.Linsert21
	movq	-36(%rbp), %r14
	movq	0(%r14), %rbx
	movq	%rbx, -44(%rbp)
	movq	-36(%rbp), %rdx
	movq	-28(%rbp), %rbx
	movq	%rbx, 0(%rdx)
	movq	-28(%rbp), %rdx
	movq	-44(%rbp), %rbx
	movq	%rbx, 0(%rdx)
	movl	$0, %eax
	leave
	ret
pub_LinkedList_size:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$32, %rsp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %r14
	movq	16(%r14), %rbx
	movq	%rbx, -16(%rbp)
	movl	$0, %ebx
	movl	%ebx, -20(%rbp)
	jmp	.Lsize24
.Lsize23:
	movq	-16(%rbp), %r14
	movq	0(%r14), %rbx
	movq	%rbx, -16(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movl	-20(%rbp), %edi
	add	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -20(%rbp)
.Lsize24:
	pushq	%rdi
	pushq	%rdx
	movq	$0, %rdx
	movq	-16(%rbp), %rdi
	cmpq	%rdx, %rdi
	setne	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$1, %al
	je	.Lsize23
	movl	-20(%rbp), %eax
	leave
	ret
pub_LinkedList_sort:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$64, %rsp
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	movq	%rdx, -24(%rbp)
	pushq	%rdx
	pushq	%rdi
	movl	$24, %eax
	movl	%eax, %edi
	call	malloc
	popq	%rdi
	popq	%rdx
	pushq	%rdi
	movq	%rax, %rdi
	pushq	%rdx
	call	pub_LinkedList_init
	popq	%rdi
	popq	%rdx
	movq	%rax, %rbx
	movq	%rbx, -32(%rbp)
	movq	-8(%rbp), %r14
	movq	16(%r14), %rbx
	movq	%rbx, -40(%rbp)
	jmp	.Lsort26
.Lsort25:
	movq	-40(%rbp), %r14
	movq	8(%r14), %rbx
	movq	%rbx, -48(%rbp)
	movl	$0, %ebx
	movl	%ebx, -52(%rbp)
	movb	$1, %bl
	movb	%bl, -53(%rbp)
	jmp	.Lsort28
.Lsort27:
	pushq	%rdx
	lea	-32(%rbp), %rax
	pushq	%rdi
	movq	(%rax), %rax
	movq	%rax, %rdi
	pushq	%rsi
	movl	-52(%rbp), %eax
	movl	%eax, %esi
	call	pub_LinkedList_get
	popq	%rsi
	popq	%rdi
	popq	%rdx
	movq	%rax, %rbx
	movq	%rbx, -61(%rbp)
	pushq	%rdx
	movq	-16(%rbp), %r15
	pushq	%rdi
	movq	-48(%rbp), %rax
	movq	%rax, %rdi
	pushq	%rsi
	movq	-61(%rbp), %rax
	movq	%rax, %rsi
	pushq	%rdx
	movq	-24(%rbp), %rax
	movq	%rax, %rdx
	call	*%r15
	popq	%rdx
	popq	%rsi
	popq	%rdi
	popq	%rdx
	movb	%al, %bl
	movb	%bl, -62(%rbp)
	pushq	%rdi
	pushq	%rdx
	movb	$1, %dl
	movb	-62(%rbp), %dil
	cmpb	%dl, %dil
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.Lsort29
	pushq	%rdx
	lea	-32(%rbp), %rax
	pushq	%rdi
	movq	(%rax), %rax
	movq	%rax, %rdi
	pushq	%rsi
	movq	-48(%rbp), %rax
	movq	%rax, %rsi
	pushq	%rdx
	movl	-52(%rbp), %eax
	movl	%eax, %edx
	call	pub_LinkedList_insert
	popq	%rdx
	popq	%rsi
	popq	%rdi
	popq	%rdx
	movb	$0, %bl
	movb	%bl, -53(%rbp)
.Lsort29:
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movl	-52(%rbp), %edi
	add	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -52(%rbp)
.Lsort28:
	pushq	%rdi
	pushq	%rdx
	movb	-53(%rbp), %dl
	pushq	%rdi
	pushq	%rdx
	pushq	%rdx
	lea	-32(%rbp), %rax
	pushq	%rdi
	movq	(%rax), %rax
	movq	%rax, %rdi
	call	pub_LinkedList_size
	popq	%rdi
	popq	%rdx
	movl	%eax, %edx
	movl	-52(%rbp), %edi
	cmpl	%edx, %edi
	setl	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %dil
	andb	%dl, %dil
	movb	%dil, %al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$1, %al
	je	.Lsort27
	pushq	%rdi
	pushq	%rdx
	pushq	%rdx
	lea	-32(%rbp), %rax
	pushq	%rdi
	movq	(%rax), %rax
	movq	%rax, %rdi
	call	pub_LinkedList_size
	popq	%rdi
	popq	%rdx
	movl	%eax, %edx
	movl	-52(%rbp), %edi
	cmpl	%edx, %edi
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.Lsort30
	pushq	%rdx
	lea	-32(%rbp), %rax
	pushq	%rdi
	movq	(%rax), %rax
	movq	%rax, %rdi
	pushq	%rsi
	movq	-48(%rbp), %rax
	movq	%rax, %rsi
	call	pub_LinkedList_append
	popq	%rsi
	popq	%rdi
	popq	%rdx
.Lsort30:
	movq	-40(%rbp), %r14
	movq	0(%r14), %rbx
	movq	%rbx, -40(%rbp)
.Lsort26:
	pushq	%rdi
	pushq	%rdx
	movq	$0, %rdx
	movq	-40(%rbp), %rdi
	cmpq	%rdx, %rdi
	setne	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$1, %al
	je	.Lsort25
	movq	-32(%rbp), %rax
	leave
	ret
pub_LinkedList_map:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$64, %rsp
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	movq	%rdx, -24(%rbp)
	pushq	%rdx
	pushq	%rdi
	movl	$24, %eax
	movl	%eax, %edi
	call	malloc
	popq	%rdi
	popq	%rdx
	pushq	%rdi
	movq	%rax, %rdi
	pushq	%rdx
	call	pub_LinkedList_init
	popq	%rdi
	popq	%rdx
	movq	%rax, %rbx
	movq	%rbx, -32(%rbp)
	movq	-8(%rbp), %r14
	movq	16(%r14), %rbx
	movq	%rbx, -40(%rbp)
	movl	$0, %ebx
	movl	%ebx, -44(%rbp)
	jmp	.Lmap32
.Lmap31:
	movq	-40(%rbp), %r14
	movq	8(%r14), %rbx
	movq	%rbx, -52(%rbp)
	pushq	%rdx
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
	popq	%rdx
	movq	%rax, %rbx
	movq	%rbx, -60(%rbp)
	pushq	%rdx
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
	popq	%rdx
	movq	-40(%rbp), %r14
	movq	0(%r14), %rbx
	movq	%rbx, -40(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movl	-44(%rbp), %edi
	add	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -44(%rbp)
.Lmap32:
	pushq	%rdi
	pushq	%rdx
	movq	$0, %rdx
	movq	-40(%rbp), %rdi
	cmpq	%rdx, %rdi
	setne	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$1, %al
	je	.Lmap31
	movq	-32(%rbp), %rax
	leave
	ret
pub_LinkedList_copy:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$16, %rsp
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	pushq	%rdx
	lea	-8(%rbp), %rax
	pushq	%rdi
	movq	(%rax), %rax
	movq	%rax, %rdi
	call	pub_LinkedList_erace
	popq	%rdi
	popq	%rdx
	movq	-16(%rbp), %rax
	leave
	ret
forEachArray:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$64, %rsp
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	movq	%rdx, -24(%rbp)
	movq	-8(%rbp), %r14
	movl	28(%r14), %ebx
	movl	%ebx, -28(%rbp)
	movq	-8(%rbp), %r14
	movl	16(%r14), %ebx
	movl	%ebx, -40(%rbp)
	movl	$0, %ebx
	movl	%ebx, -52(%rbp)
	jmp	.LforEachArray34
.LforEachArray33:
	pushq	%rdi
	pushq	%rdx
	movl	-40(%rbp), %edx
	movl	-52(%rbp), %edi
	imul	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -56(%rbp)
	movq	-8(%rbp), %r14
	movq	20(%r14), %rbx
	movq	%rbx, -48(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	-56(%rbp), %edx
	movq	-48(%rbp), %rdi
	add	%rdx, %rdi
	movq	%rdi, %rax
	popq	%rdx
	popq	%rdi
	movq	%rax, %rbx
	movq	%rbx, -48(%rbp)
	pushq	%rdx
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
	popq	%rdx
	movl	%eax, %ebx
	movl	%ebx, -60(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movl	-60(%rbp), %edi
	cmpl	%edx, %edi
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.LforEachArray35
	movl	$0, %eax
	leave
	ret
.LforEachArray35:
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movl	-52(%rbp), %edi
	add	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -52(%rbp)
.LforEachArray34:
	pushq	%rdi
	pushq	%rdx
	movl	-28(%rbp), %edx
	movl	-52(%rbp), %edi
	cmpl	%edx, %edi
	setl	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$1, %al
	je	.LforEachArray33
	movl	$0, %eax
	leave
	ret
pub_LinkedList_find:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$48, %rsp
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	movq	%rdx, -24(%rbp)
	movq	-8(%rbp), %r14
	movq	16(%r14), %rbx
	movq	%rbx, -32(%rbp)
	movl	$0, %ebx
	movl	%ebx, -36(%rbp)
	jmp	.Lfind37
.Lfind36:
	movq	-32(%rbp), %r14
	movq	8(%r14), %rbx
	movq	%rbx, -44(%rbp)
	pushq	%rdx
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
	popq	%rdx
	movb	%al, %bl
	movb	%bl, -45(%rbp)
	pushq	%rdi
	pushq	%rdx
	movb	$1, %dl
	movb	-45(%rbp), %dil
	cmpb	%dl, %dil
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.Lfind38
	movl	-36(%rbp), %eax
	leave
	ret
.Lfind38:
	movq	-32(%rbp), %r14
	movq	0(%r14), %rbx
	movq	%rbx, -32(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movl	-36(%rbp), %edi
	add	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -36(%rbp)
.Lfind37:
	pushq	%rdi
	pushq	%rdx
	movq	$0, %rdx
	movq	-32(%rbp), %rdi
	cmpq	%rdx, %rdi
	setne	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$1, %al
	je	.Lfind36
	movl	$-1, %eax
	leave
	ret
whereArray:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$80, %rsp
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	movq	%rdx, -24(%rbp)
	pushq	%rdx
	pushq	%rdi
	movl	$32, %eax
	movl	%eax, %edi
	call	malloc
	popq	%rdi
	popq	%rdx
	pushq	%rdi
	movq	%rax, %rdi
	pushq	%rdx
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
	popq	%rdx
	movq	%rax, %rbx
	movq	%rbx, -32(%rbp)
	movq	-8(%rbp), %r14
	movl	16(%r14), %ebx
	movl	%ebx, -40(%rbp)
	movq	-8(%rbp), %r14
	movl	28(%r14), %ebx
	movl	%ebx, -36(%rbp)
	movl	$0, %ebx
	movl	%ebx, -44(%rbp)
	jmp	.LwhereArray40
.LwhereArray39:
	pushq	%rdi
	pushq	%rdx
	movl	-40(%rbp), %edx
	movl	-44(%rbp), %edi
	imul	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -48(%rbp)
	movq	-8(%rbp), %r14
	movq	20(%r14), %rbx
	movq	%rbx, -56(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	-48(%rbp), %edx
	movq	-56(%rbp), %rdi
	add	%rdx, %rdi
	movq	%rdi, %rax
	popq	%rdx
	popq	%rdi
	movq	%rax, %rbx
	movq	%rbx, -56(%rbp)
	movq	-56(%rbp), %rbx
	movq	%rbx, -64(%rbp)
	pushq	%rdx
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
	popq	%rdx
	movb	%al, %bl
	movb	%bl, -65(%rbp)
	pushq	%rdi
	pushq	%rdx
	movb	$1, %dl
	movb	-65(%rbp), %dil
	cmpb	%dl, %dil
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.LwhereArray41
	pushq	%rdx
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
	popq	%rdx
	movq	-32(%rbp), %r14
	movl	28(%r14), %ebx
	movl	%ebx, -69(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movl	-69(%rbp), %edi
	sub	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -69(%rbp)
	pushq	%rdx
	lea	-32(%rbp), %rax
	pushq	%rdi
	movq	(%rax), %rax
	movq	%rax, %rdi
	pushq	%rsi
	movl	-69(%rbp), %eax
	movl	%eax, %esi
	call	pub_Array_at
	popq	%rsi
	popq	%rdi
	popq	%rdx
	movq	%rax, %rbx
	movq	%rbx, -77(%rbp)
	movq	-77(%rbp), %rax
	movq	-64(%rbp), %rbx
	movq	%rbx, (%rax)
.LwhereArray41:
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movl	-44(%rbp), %edi
	add	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -44(%rbp)
.LwhereArray40:
	pushq	%rdi
	pushq	%rdx
	movl	-36(%rbp), %edx
	movl	-44(%rbp), %edi
	cmpl	%edx, %edi
	setl	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$1, %al
	je	.LwhereArray39
	movq	-32(%rbp), %rax
	leave
	ret
pub_Array_init:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
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
	pushq	%rdi
	pushq	%rdx
	movl	-12(%rbp), %edx
	movl	-16(%rbp), %edi
	imul	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -16(%rbp)
	pushq	%rdx
	pushq	%rdi
	movl	-16(%rbp), %eax
	movl	%eax, %edi
	call	malloc
	popq	%rdi
	popq	%rdx
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
pub_Array_at:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$32, %rsp
	movq	%rdi, -8(%rbp)
	movl	%esi, -12(%rbp)
	movq	-8(%rbp), %r14
	movl	16(%r14), %ebx
	movl	%ebx, -16(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	-16(%rbp), %edx
	movl	-12(%rbp), %edi
	imul	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -20(%rbp)
	movq	-8(%rbp), %r14
	movq	20(%r14), %rbx
	movq	%rbx, -28(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	-20(%rbp), %edx
	movq	-28(%rbp), %rdi
	add	%rdx, %rdi
	movq	%rdi, %rax
	popq	%rdx
	popq	%rdi
	movq	%rax, %rbx
	movq	%rbx, -28(%rbp)
	movq	-28(%rbp), %rax
	leave
	ret
newArray:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$16, %rsp
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	pushq	%rdx
	pushq	%rdi
	movl	$32, %eax
	movl	%eax, %edi
	call	malloc
	popq	%rdi
	popq	%rdx
	movq	%rax, %rbx
	movq	%rbx, -16(%rbp)
	pushq	%rdx
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
	popq	%rdx
	movq	-16(%rbp), %rax
	leave
	ret
pub_Array_delete:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$16, %rsp
	movq	%rdi, -8(%rbp)
	pushq	%rdx
	movq	-8(%rbp), %r14
	pushq	%rdi
	movq	20(%r14), %rax
	movq	%rax, %rdi
	call	free
	popq	%rdi
	popq	%rdx
	pushq	%rdx
	pushq	%rdi
	movq	-8(%rbp), %rax
	movq	%rax, %rdi
	call	free
	popq	%rdi
	popq	%rdx
	movl	$0, %eax
	leave
	ret
pub_LinkedList_erace:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$32, %rsp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %r14
	movq	16(%r14), %rbx
	movq	%rbx, -16(%rbp)
	jmp	.Lerace43
.Lerace42:
	movq	-16(%rbp), %r14
	movq	0(%r14), %rbx
	movq	%rbx, -24(%rbp)
	pushq	%rdx
	movq	-16(%rbp), %r14
	pushq	%rdi
	movq	8(%r14), %rax
	movq	%rax, %rdi
	call	free
	popq	%rdi
	popq	%rdx
	pushq	%rdx
	pushq	%rdi
	movq	-16(%rbp), %rax
	movq	%rax, %rdi
	call	free
	popq	%rdi
	popq	%rdx
	movq	-24(%rbp), %rbx
	movq	%rbx, -16(%rbp)
.Lerace43:
	pushq	%rdi
	pushq	%rdx
	movq	$0, %rdx
	movq	-16(%rbp), %rdi
	cmpq	%rdx, %rdi
	setne	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$1, %al
	je	.Lerace42
	pushq	%rdx
	pushq	%rdi
	movq	-8(%rbp), %rax
	movq	%rax, %rdi
	call	free
	popq	%rdi
	popq	%rdx
	movl	$0, %eax
	leave
	ret
pub_LinkedList_reverse:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$32, %rsp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %r14
	movq	16(%r14), %rbx
	movq	%rbx, -16(%rbp)
	movq	$0, %rbx
	movq	%rbx, -24(%rbp)
	jmp	.Lreverse45
.Lreverse44:
	movq	-16(%rbp), %r14
	movq	0(%r14), %rbx
	movq	%rbx, -32(%rbp)
	movq	-16(%rbp), %rdx
	movq	-24(%rbp), %rbx
	movq	%rbx, 0(%rdx)
	movq	-16(%rbp), %rbx
	movq	%rbx, -24(%rbp)
	movq	-32(%rbp), %rbx
	movq	%rbx, -16(%rbp)
.Lreverse45:
	pushq	%rdi
	pushq	%rdx
	movq	$0, %rdx
	movq	-16(%rbp), %rdi
	cmpq	%rdx, %rdi
	setne	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$1, %al
	je	.Lreverse44
	movq	-8(%rbp), %rdx
	movq	-24(%rbp), %rbx
	movq	%rbx, 16(%rdx)
	movq	-8(%rbp), %rax
	leave
	ret
pub_Array_extend:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$32, %rsp
	movq	%rdi, -8(%rbp)
	movl	%esi, -12(%rbp)
	movq	-8(%rbp), %r14
	movq	20(%r14), %rbx
	movq	%rbx, -20(%rbp)
	movq	-8(%rbp), %r14
	movl	28(%r14), %ebx
	movl	%ebx, -24(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	-12(%rbp), %edx
	movl	-24(%rbp), %edi
	add	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movq	-8(%rbp), %rdx
	movl	%eax, %ebx
	movl	%ebx, 28(%rdx)
	movq	-8(%rbp), %r14
	movl	16(%r14), %ebx
	movl	%ebx, -28(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	-24(%rbp), %edx
	movl	-12(%rbp), %edi
	add	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -12(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	-28(%rbp), %edx
	movl	-12(%rbp), %edi
	imul	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -12(%rbp)
	pushq	%rdx
	pushq	%rdi
	movq	-20(%rbp), %rax
	movq	%rax, %rdi
	pushq	%rsi
	movl	-12(%rbp), %eax
	movl	%eax, %esi
	call	realloc
	popq	%rsi
	popq	%rdi
	popq	%rdx
	movq	-8(%rbp), %rdx
	movq	%rax, %rbx
	movq	%rbx, 20(%rdx)
	pushq	%rdi
	pushq	%rdx
	movq	$0, %rdx
	movq	-8(%rbp), %r14
	movq	20(%r14), %rdi
	cmpq	%rdx, %rdi
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.Lextend46
	pushq	%rdx
	pushq	%rdi
	movq	$.strArray.extend47, %rax
	movq	%rax, %rdi
	call	print
	popq	%rdi
	popq	%rdx
.Lextend46:
	movl	-12(%rbp), %eax
	leave
	ret


.data

.strArray.extend47:
	.asciz	 "badrealloc"


.bss

