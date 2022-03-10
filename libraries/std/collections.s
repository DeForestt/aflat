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
	movl	$1, %edx
	movl	-48(%rbp), %eax
	cmpl	%edx, %eax
	sete	%al
	movb	%al, %al
	cmpb	$0, %al
	je	.LforEachLinkedList2
	movl	$1, %eax
	leave
	ret
.LforEachLinkedList2:
	movq	-32(%rbp), %rdx
	movq	0(%rdx), %rbx
	movq	%rbx, -32(%rbp)
	movl	$1, %edx
	movl	-36(%rbp), %eax
	add	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -36(%rbp)
.LforEachLinkedList1:
	movq	$0, %rdx
	movq	-32(%rbp), %rax
	cmpq	%rdx, %rax
	setne	%al
	movb	%al, %al
	cmpb	$1, %al
	je	.LforEachLinkedList0
	movl	$0, %eax
	leave
	ret
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
	movq	-8(%rbp), %rdx
	movq	16(%rdx), %rbx
	movq	%rbx, -40(%rbp)
	movl	$0, %ebx
	movl	%ebx, -44(%rbp)
	jmp	.LwhereLinkedList4
.LwhereLinkedList3:
	movq	-40(%rbp), %rdx
	movq	8(%rdx), %rbx
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
	movb	$1, %dl
	movb	-53(%rbp), %al
	cmpb	%dl, %al
	sete	%al
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
	movq	-40(%rbp), %rdx
	movq	0(%rdx), %rbx
	movq	%rbx, -40(%rbp)
	movl	$1, %edx
	movl	-44(%rbp), %eax
	add	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -44(%rbp)
.LwhereLinkedList4:
	movq	$0, %rdx
	movq	-40(%rbp), %rax
	cmpq	%rdx, %rax
	setne	%al
	movb	%al, %al
	cmpb	$1, %al
	je	.LwhereLinkedList3
	movq	-32(%rbp), %rax
	leave
	ret
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
	popq	%rbx
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
	leave
	ret
pub_LinkedList_delete:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$32, %rsp
	movq	%rdi, -8(%rbp)
	movl	%esi, -12(%rbp)
	movq	-8(%rbp), %rdx
	movq	16(%rdx), %rbx
	movq	%rbx, -20(%rbp)
	movl	$0, %ebx
	movl	%ebx, -24(%rbp)
	jmp	.Ldelete7
.Ldelete6:
	movq	-20(%rbp), %rdx
	movq	0(%rdx), %rbx
	movq	%rbx, -20(%rbp)
	movl	$1, %edx
	movl	-24(%rbp), %eax
	add	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -24(%rbp)
.Ldelete7:
	movl	-12(%rbp), %edx
	movl	-24(%rbp), %eax
	cmpl	%edx, %eax
	setl	%al
	movb	%al, %al
	cmpb	$1, %al
	je	.Ldelete6
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
	pushq	%rbx
	subq	$32, %rsp
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	movq	-8(%rbp), %rdx
	movq	16(%rdx), %rbx
	movq	%rbx, -24(%rbp)
	jmp	.Lstitch9
.Lstitch8:
	movq	-24(%rbp), %rdx
	movq	0(%rdx), %rbx
	movq	%rbx, -24(%rbp)
.Lstitch9:
	movq	$0, %rdx
	movq	-24(%rbp), %rdx
	movq	0(%rdx), %rax
	cmpq	%rdx, %rax
	setne	%al
	movb	%al, %al
	cmpb	$1, %al
	je	.Lstitch8
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
	pushq	%rbx
	subq	$32, %rsp
	movq	%rdi, -8(%rbp)
	movl	%esi, -12(%rbp)
	movl	$0, %edx
	movl	-12(%rbp), %eax
	cmpl	%edx, %eax
	setl	%al
	movb	%al, %al
	cmpb	$0, %al
	je	.Lget10
	movq	$0, %rax
	leave
	ret
.Lget10:
	movq	-8(%rbp), %rdx
	movq	16(%rdx), %rbx
	movq	%rbx, -20(%rbp)
	movq	-20(%rbp), %rbx
	movq	%rbx, -28(%rbp)
	movl	$0, %ebx
	movl	%ebx, -32(%rbp)
	jmp	.Lget12
.Lget11:
	movq	-28(%rbp), %rdx
	movq	0(%rdx), %rbx
	movq	%rbx, -28(%rbp)
	movl	$1, %edx
	movl	-32(%rbp), %eax
	add	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -32(%rbp)
.Lget12:
	movl	-12(%rbp), %edx
	movl	-32(%rbp), %eax
	cmpl	%edx, %eax
	setl	%al
	movb	%al, %al
	cmpb	$1, %al
	je	.Lget11
	movq	-28(%rbp), %rdx
	movq	8(%rdx), %rax
	leave
	ret
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
	movq	-8(%rbp), %rdx
	movq	16(%rdx), %rbx
	movq	%rbx, -32(%rbp)
	movq	$0, %rdx
	movq	-32(%rbp), %rax
	cmpq	%rdx, %rax
	sete	%al
	movb	%al, %al
	cmpb	$0, %al
	je	.Lappend13
	movq	-8(%rbp), %rdx
	movq	-24(%rbp), %rbx
	movq	%rbx, 16(%rdx)
	movl	$0, %eax
	leave
	ret
.Lappend13:
	movq	-8(%rbp), %rdx
	movq	16(%rdx), %rbx
	movq	%rbx, -40(%rbp)
	movq	-40(%rbp), %rdx
	movq	0(%rdx), %rbx
	movq	%rbx, -48(%rbp)
	jmp	.Lappend15
.Lappend14:
	movq	-48(%rbp), %rbx
	movq	%rbx, -40(%rbp)
	movq	-40(%rbp), %rdx
	movq	0(%rdx), %rbx
	movq	%rbx, -48(%rbp)
.Lappend15:
	movq	$0, %rdx
	movq	-48(%rbp), %rax
	cmpq	%rdx, %rax
	setne	%al
	movb	%al, %al
	cmpb	$1, %al
	je	.Lappend14
	movq	-40(%rbp), %rdx
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
	pushq	%rbx
	subq	$48, %rsp
	movq	%rdi, -8(%rbp)
	movl	%esi, -12(%rbp)
	movq	%rdx, -20(%rbp)
	movl	$0, %edx
	movl	-12(%rbp), %eax
	cmpl	%edx, %eax
	setl	%al
	movb	%al, %al
	cmpb	$0, %al
	je	.Linsert16
	movl	$-1, %eax
	leave
	ret
.Linsert16:
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
	movq	-20(%rbp), %rbx
	movq	%rbx, 8(%rdx)
	movq	-8(%rbp), %rdx
	movq	16(%rdx), %rbx
	movq	%rbx, -36(%rbp)
	movl	$0, %ebx
	movl	%ebx, -40(%rbp)
	jmp	.Linsert18
.Linsert17:
	movq	-36(%rbp), %rdx
	movq	0(%rdx), %rbx
	movq	%rbx, -36(%rbp)
	movl	$1, %edx
	movl	-40(%rbp), %eax
	add	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -40(%rbp)
.Linsert18:
	movl	-12(%rbp), %edx
	movl	-40(%rbp), %eax
	cmpl	%edx, %eax
	setl	%al
	movb	%al, %al
	cmpb	$1, %al
	je	.Linsert17
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
	pushq	%rbx
	subq	$32, %rsp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rdx
	movq	16(%rdx), %rbx
	movq	%rbx, -16(%rbp)
	movl	$0, %ebx
	movl	%ebx, -20(%rbp)
	jmp	.Lsize20
.Lsize19:
	movq	-16(%rbp), %rdx
	movq	0(%rdx), %rbx
	movq	%rbx, -16(%rbp)
	movl	$1, %edx
	movl	-20(%rbp), %eax
	add	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -20(%rbp)
.Lsize20:
	movq	$0, %rdx
	movq	-16(%rbp), %rax
	cmpq	%rdx, %rax
	setne	%al
	movb	%al, %al
	cmpb	$1, %al
	je	.Lsize19
	movl	-20(%rbp), %eax
	leave
	ret
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
	movq	-8(%rbp), %rdx
	movq	16(%rdx), %rbx
	movq	%rbx, -40(%rbp)
	movl	$0, %ebx
	movl	%ebx, -44(%rbp)
	jmp	.Lmap22
.Lmap21:
	movq	-40(%rbp), %rdx
	movq	8(%rdx), %rbx
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
	movq	-40(%rbp), %rdx
	movq	0(%rdx), %rbx
	movq	%rbx, -40(%rbp)
	movl	$1, %edx
	movl	-44(%rbp), %eax
	add	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -44(%rbp)
.Lmap22:
	movq	$0, %rdx
	movq	-40(%rbp), %rax
	cmpq	%rdx, %rax
	setne	%al
	movb	%al, %al
	cmpb	$1, %al
	je	.Lmap21
	movq	-32(%rbp), %rax
	leave
	ret
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
	movq	-8(%rbp), %rdx
	movl	28(%rdx), %ebx
	movl	%ebx, -28(%rbp)
	movq	-8(%rbp), %rdx
	movl	16(%rdx), %ebx
	movl	%ebx, -40(%rbp)
	movl	$0, %ebx
	movl	%ebx, -52(%rbp)
	jmp	.LforEachArray24
.LforEachArray23:
	movl	-40(%rbp), %edx
	movl	-52(%rbp), %eax
	imul	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -56(%rbp)
	movq	-8(%rbp), %rdx
	movq	20(%rdx), %rbx
	movq	%rbx, -48(%rbp)
	movl	-56(%rbp), %edx
	movq	-48(%rbp), %rax
	add	%rdx, %rax
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
	movl	$1, %edx
	movl	-60(%rbp), %eax
	cmpl	%edx, %eax
	sete	%al
	movb	%al, %al
	cmpb	$0, %al
	je	.LforEachArray25
	movl	$0, %eax
	leave
	ret
.LforEachArray25:
	movl	$1, %edx
	movl	-52(%rbp), %eax
	add	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -52(%rbp)
.LforEachArray24:
	movl	-28(%rbp), %edx
	movl	-52(%rbp), %eax
	cmpl	%edx, %eax
	setl	%al
	movb	%al, %al
	cmpb	$1, %al
	je	.LforEachArray23
	movl	$0, %eax
	leave
	ret
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
	movq	-8(%rbp), %rdx
	movq	16(%rdx), %rbx
	movq	%rbx, -32(%rbp)
	movl	$0, %ebx
	movl	%ebx, -36(%rbp)
	jmp	.Lfind27
.Lfind26:
	movq	-32(%rbp), %rdx
	movq	8(%rdx), %rbx
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
	movb	$1, %dl
	movb	-45(%rbp), %al
	cmpb	%dl, %al
	sete	%al
	movb	%al, %al
	cmpb	$0, %al
	je	.Lfind28
	movl	-36(%rbp), %eax
	leave
	ret
.Lfind28:
	movq	-32(%rbp), %rdx
	movq	0(%rdx), %rbx
	movq	%rbx, -32(%rbp)
	movl	$1, %edx
	movl	-36(%rbp), %eax
	add	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -36(%rbp)
.Lfind27:
	movq	$0, %rdx
	movq	-32(%rbp), %rax
	cmpq	%rdx, %rax
	setne	%al
	movb	%al, %al
	cmpb	$1, %al
	je	.Lfind26
	movl	$-1, %eax
	leave
	ret
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
	movq	-8(%rbp), %rdx
	movl	16(%rdx), %ebx
	movl	%ebx, -40(%rbp)
	movq	-8(%rbp), %rdx
	movl	28(%rdx), %ebx
	movl	%ebx, -36(%rbp)
	movl	$0, %ebx
	movl	%ebx, -44(%rbp)
	jmp	.LwhereArray30
.LwhereArray29:
	movl	-40(%rbp), %edx
	movl	-44(%rbp), %eax
	imul	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -48(%rbp)
	movq	-8(%rbp), %rdx
	movq	20(%rdx), %rbx
	movq	%rbx, -56(%rbp)
	movl	-48(%rbp), %edx
	movq	-56(%rbp), %rax
	add	%rdx, %rax
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
	movb	$1, %dl
	movb	-65(%rbp), %al
	cmpb	%dl, %al
	sete	%al
	movb	%al, %al
	cmpb	$0, %al
	je	.LwhereArray31
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
	movq	-32(%rbp), %rdx
	movl	28(%rdx), %ebx
	movl	%ebx, -69(%rbp)
	movl	$1, %edx
	movl	-69(%rbp), %eax
	sub	%edx, %eax
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
.LwhereArray31:
	movl	$1, %edx
	movl	-44(%rbp), %eax
	add	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -44(%rbp)
.LwhereArray30:
	movl	-36(%rbp), %edx
	movl	-44(%rbp), %eax
	cmpl	%edx, %eax
	setl	%al
	movb	%al, %al
	cmpb	$1, %al
	je	.LwhereArray29
	movq	-32(%rbp), %rax
	leave
	ret
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
	movl	-12(%rbp), %edx
	movl	-16(%rbp), %eax
	imul	%edx, %eax
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
	popq	%rbx
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
	movq	-8(%rbp), %rdx
	movl	16(%rdx), %ebx
	movl	%ebx, -16(%rbp)
	movl	-16(%rbp), %edx
	movl	-12(%rbp), %eax
	imul	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -20(%rbp)
	movq	-8(%rbp), %rdx
	movq	20(%rdx), %rbx
	movq	%rbx, -28(%rbp)
	movl	-20(%rbp), %edx
	movq	-28(%rbp), %rax
	add	%rdx, %rax
	movq	%rax, %rbx
	movq	%rbx, -28(%rbp)
	movq	-28(%rbp), %rax
	leave
	ret
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
	leave
	ret
pub_Array_delete:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$16, %rsp
	movq	%rdi, -8(%rbp)
	pushq	%rdx
	movq	-8(%rbp), %rdx
	pushq	%rdi
	movq	20(%rdx), %rax
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
	leave
	ret
pub_LinkedList_erace:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$32, %rsp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rdx
	movq	16(%rdx), %rbx
	movq	%rbx, -16(%rbp)
	jmp	.Lerace33
.Lerace32:
	movq	-16(%rbp), %rdx
	movq	0(%rdx), %rbx
	movq	%rbx, -24(%rbp)
	pushq	%rdx
	movq	-16(%rbp), %rdx
	pushq	%rdi
	movq	8(%rdx), %rax
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
.Lerace33:
	movq	$0, %rdx
	movq	-16(%rbp), %rax
	cmpq	%rdx, %rax
	setne	%al
	movb	%al, %al
	cmpb	$1, %al
	je	.Lerace32
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
	leave
	ret
pub_LinkedList_reverse:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$32, %rsp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rdx
	movq	16(%rdx), %rbx
	movq	%rbx, -16(%rbp)
	movq	$0, %rbx
	movq	%rbx, -24(%rbp)
	jmp	.Lreverse35
.Lreverse34:
	movq	-16(%rbp), %rdx
	movq	0(%rdx), %rbx
	movq	%rbx, -32(%rbp)
	movq	-16(%rbp), %rdx
	movq	-24(%rbp), %rbx
	movq	%rbx, 0(%rdx)
	movq	-16(%rbp), %rbx
	movq	%rbx, -24(%rbp)
	movq	-32(%rbp), %rbx
	movq	%rbx, -16(%rbp)
.Lreverse35:
	movq	$0, %rdx
	movq	-16(%rbp), %rax
	cmpq	%rdx, %rax
	setne	%al
	movb	%al, %al
	cmpb	$1, %al
	je	.Lreverse34
	movq	-8(%rbp), %rdx
	movq	-24(%rbp), %rbx
	movq	%rbx, 16(%rdx)
	movq	-8(%rbp), %rax
	leave
	ret
	leave
	ret
pub_Array_extend:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$32, %rsp
	movq	%rdi, -8(%rbp)
	movl	%esi, -12(%rbp)
	movq	-8(%rbp), %rdx
	movq	20(%rdx), %rbx
	movq	%rbx, -20(%rbp)
	movq	-8(%rbp), %rdx
	movl	28(%rdx), %ebx
	movl	%ebx, -24(%rbp)
	movl	-12(%rbp), %edx
	movl	-24(%rbp), %eax
	add	%edx, %eax
	movq	-8(%rbp), %rdx
	movl	%eax, %ebx
	movl	%ebx, 28(%rdx)
	movq	-8(%rbp), %rdx
	movl	16(%rdx), %ebx
	movl	%ebx, -28(%rbp)
	movl	-24(%rbp), %edx
	movl	-12(%rbp), %eax
	add	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -12(%rbp)
	movl	-28(%rbp), %edx
	movl	-12(%rbp), %eax
	imul	%edx, %eax
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
	movq	$0, %rdx
	movq	-8(%rbp), %rdx
	movq	20(%rdx), %rax
	cmpq	%rdx, %rax
	sete	%al
	movb	%al, %al
	cmpb	$0, %al
	je	.Lextend36
	pushq	%rdx
	pushq	%rdi
	movq	$.strArray.extend37, %rax
	movq	%rax, %rdi
	call	print
	popq	%rdi
	popq	%rdx
.Lextend36:
	movl	-12(%rbp), %eax
	leave
	ret
	leave
	ret


.data

.strArray.extend37:
	.asciz	 "badrealloc"


.bss

