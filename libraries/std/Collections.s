.global	pub_Array_extend
.global	pub_LinkedList_reverse
.global	pub_LinkedList_erace
.global	pub_Array_del
.global	pub_Array_at
.global	pub_Array_init
.global	pub_LinkedList_find
.global	pub_LinkedList_copy
.global	pub_LinkedList_map
.global	pub_LinkedList_sort
.global	pub_LinkedList_size
.global	pub_LinkedList_insert
.global	pub_LinkedList_append
.global	pub_LinkedList_get
.global	pub_LinkedList_stitch
.global	pub_LinkedList_del
.global	pub_LinkedList_init
.global	pub_LinkedList_where
.global	pub_LinkedList_forEach
.global	pub_LinkedList_reduce


.text

pub_ICollection_init:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$16, %rsp
	movq	%rdi, -8(%rbp)
	pushq	%r14
	movq	-8(%rbp), %r14
	movq	$lambda_1650419093816_0, %rbx
	movq	%rbx, 0(%r14)
	popq	%r14
	pushq	%r14
	movq	-8(%rbp), %r14
	movq	$lambda_1650419093816_2, %rbx
	movq	%rbx, 8(%r14)
	popq	%r14
	movq	-8(%rbp), %r15
	movq	%r15, %rax
	leave
	ret
pub_LinkedList_reduce:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$48, %rsp
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	movq	%rdx, -24(%rbp)
	pushq	%r14
	movq	-8(%rbp), %r14
	movq	0(%r14), %r14
	movq	8(%r14), %r15
	popq	%r14
	movq	%r15, %rbx
	movq	%rbx, -32(%rbp)
	pushq	%r14
	movq	-8(%rbp), %r14
	movq	0(%r14), %r14
	movq	0(%r14), %r15
	popq	%r14
	movq	%r15, %rbx
	movq	%rbx, -40(%rbp)
	jmp	.Lreduce5
.Lreduce4:
	pushq	%rdx
	movq	-16(%rbp), %r15
	movq	%r15, %r11
	movq	-32(%rbp), %r15
	pushq	%rdi
	movq	%r15, %rax
	movq	%rax, %rdi
	pushq	%r14
	movq	-40(%rbp), %r14
	movq	8(%r14), %r15
	popq	%r14
	pushq	%rsi
	movq	%r15, %rax
	movq	%rax, %rsi
	movq	-24(%rbp), %r15
	pushq	%rdx
	movq	%r15, %rax
	movq	%rax, %rdx
	call	*%r11
	popq	%rdx
	popq	%rsi
	popq	%rdi
	popq	%rdx
	movq	%rax, %rbx
	movq	%rbx, -32(%rbp)
	pushq	%r14
	movq	-40(%rbp), %r14
	movq	0(%r14), %r15
	popq	%r14
	movq	%r15, %rbx
	movq	%rbx, -40(%rbp)
.Lreduce5:
	pushq	%rdi
	pushq	%rdx
	movq	$0, %rdx
	movq	-40(%rbp), %r15
	movq	%r15, %rdi
	cmpq	%rdx, %rdi
	setne	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$1, %al
	je	.Lreduce4
	movq	-32(%rbp), %r15
	movq	%r15, %rax
	leave
	ret
pub_LinkedList_forEach:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$48, %rsp
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	movq	%rdx, -24(%rbp)
	pushq	%r14
	movq	-8(%rbp), %r14
	movq	0(%r14), %r15
	popq	%r14
	movq	%r15, %rbx
	movq	%rbx, -32(%rbp)
	movl	$0, %ebx
	movl	%ebx, -36(%rbp)
	jmp	.LforEach7
.LforEach6:
	pushq	%r14
	movq	-32(%rbp), %r14
	movq	8(%r14), %r15
	popq	%r14
	movq	%r15, %rbx
	movq	%rbx, -44(%rbp)
	pushq	%rdx
	movq	-16(%rbp), %r15
	movq	%r15, %r11
	movq	-44(%rbp), %r15
	pushq	%rdi
	movq	%r15, %rax
	movq	%rax, %rdi
	movl	-36(%rbp), %r15d
	pushq	%rsi
	movl	%r15d, %eax
	movl	%eax, %esi
	movq	-24(%rbp), %r15
	pushq	%rdx
	movq	%r15, %rax
	movq	%rax, %rdx
	call	*%r11
	popq	%rdx
	popq	%rsi
	popq	%rdi
	popq	%rdx
	movl	%eax, %ebx
	movl	%ebx, -48(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movl	-48(%rbp), %r15d
	movl	%r15d, %edi
	cmpl	%edx, %edi
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.LforEach8
	movl	$1, %eax
	leave
	ret
.LforEach8:
	pushq	%r14
	movq	-32(%rbp), %r14
	movq	0(%r14), %r15
	popq	%r14
	movq	%r15, %rbx
	movq	%rbx, -32(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movl	-36(%rbp), %r15d
	movl	%r15d, %edi
	add	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -36(%rbp)
.LforEach7:
	pushq	%rdi
	pushq	%rdx
	movq	$0, %rdx
	movq	-32(%rbp), %r15
	movq	%r15, %rdi
	cmpq	%rdx, %rdi
	setne	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$1, %al
	je	.LforEach6
	movl	$0, %eax
	leave
	ret
pub_LinkedList_where:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$64, %rsp
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	movq	%rdx, -24(%rbp)
	pushq	%rdx
	pushq	%rdi
	movl	$16, %eax
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
	pushq	%r14
	movq	-8(%rbp), %r14
	movq	0(%r14), %r15
	popq	%r14
	movq	%r15, %rbx
	movq	%rbx, -40(%rbp)
	movl	$0, %ebx
	movl	%ebx, -44(%rbp)
	jmp	.Lwhere10
.Lwhere9:
	pushq	%r14
	movq	-40(%rbp), %r14
	movq	8(%r14), %r15
	popq	%r14
	movq	%r15, %rbx
	movq	%rbx, -52(%rbp)
	pushq	%rdx
	movq	-16(%rbp), %r15
	movq	%r15, %r11
	movq	-52(%rbp), %r15
	pushq	%rdi
	movq	%r15, %rax
	movq	%rax, %rdi
	movl	-44(%rbp), %r15d
	pushq	%rsi
	movl	%r15d, %eax
	movl	%eax, %esi
	movq	-24(%rbp), %r15
	pushq	%rdx
	movq	%r15, %rax
	movq	%rax, %rdx
	call	*%r11
	popq	%rdx
	popq	%rsi
	popq	%rdi
	popq	%rdx
	movb	%al, %bl
	movb	%bl, -53(%rbp)
	pushq	%rdi
	pushq	%rdx
	movb	$1, %dl
	movb	-53(%rbp), %r15b
	movb	%r15b, %dil
	cmpb	%dl, %dil
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.Lwhere11
	pushq	%rdx
	lea	-32(%rbp), %rax
	pushq	%rdi
	movq	(%rax), %rax
	movq	%rax, %rdi
	movq	-52(%rbp), %r15
	pushq	%rsi
	movq	%r15, %rax
	movq	%rax, %rsi
	call	pub_LinkedList_append
	popq	%rsi
	popq	%rdi
	popq	%rdx
.Lwhere11:
	pushq	%r14
	movq	-40(%rbp), %r14
	movq	0(%r14), %r15
	popq	%r14
	movq	%r15, %rbx
	movq	%rbx, -40(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movl	-44(%rbp), %r15d
	movl	%r15d, %edi
	add	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -44(%rbp)
.Lwhere10:
	pushq	%rdi
	pushq	%rdx
	movq	$0, %rdx
	movq	-40(%rbp), %r15
	movq	%r15, %rdi
	cmpq	%rdx, %rdi
	setne	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$1, %al
	je	.Lwhere9
	movq	-32(%rbp), %r15
	movq	%r15, %rax
	leave
	ret
pub_LinkedList_init:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$16, %rsp
	movq	%rdi, -8(%rbp)
	pushq	%r14
	movq	-8(%rbp), %r14
	pushq	%r14
	movq	-8(%rbp), %r14
	movq	0(%r14), %r15
	popq	%r14
	movq	%r15, %rbx
	movq	%rbx, 8(%r14)
	popq	%r14
	pushq	%r14
	movq	-8(%rbp), %r14
	movq	$0, %rbx
	movq	%rbx, 0(%r14)
	popq	%r14
	movq	-8(%rbp), %r15
	movq	%r15, %rax
	leave
	ret
pub_LinkedList_del:
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
	movl	-12(%rbp), %r15d
	movl	%r15d, %edi
	cmpl	%edx, %edi
	setg	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %dl
	pushq	%rdi
	pushq	%rdx
	movl	$0, %edx
	movl	-12(%rbp), %r15d
	movl	%r15d, %edi
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
	je	.Ldel12
	movl	$-1, %eax
	leave
	ret
.Ldel12:
	pushq	%r14
	movq	-8(%rbp), %r14
	movq	0(%r14), %r15
	popq	%r14
	movq	%r15, %rbx
	movq	%rbx, -20(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	$0, %edx
	movl	-12(%rbp), %r15d
	movl	%r15d, %edi
	cmpl	%edx, %edi
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.Ldel13
	pushq	%r14
	movq	-8(%rbp), %r14
	pushq	%r14
	movq	-20(%rbp), %r14
	movq	0(%r14), %r15
	popq	%r14
	movq	%r15, %rbx
	movq	%rbx, 0(%r14)
	popq	%r14
	movl	$0, %eax
	leave
	ret
.Ldel13:
	movl	$0, %ebx
	movl	%ebx, -24(%rbp)
	jmp	.Ldel15
.Ldel14:
	pushq	%rdi
	pushq	%rdx
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movl	-12(%rbp), %r15d
	movl	%r15d, %edi
	sub	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %edx
	movl	-24(%rbp), %r15d
	movl	%r15d, %edi
	cmpl	%edx, %edi
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.Ldel16
	pushq	%r14
	movq	-20(%rbp), %r14
	movq	0(%r14), %r15
	popq	%r14
	movq	%r15, %rbx
	movq	%rbx, -32(%rbp)
	pushq	%r14
	movq	-20(%rbp), %r14
	pushq	%r14
	movq	-32(%rbp), %r14
	movq	0(%r14), %r15
	popq	%r14
	movq	%r15, %rbx
	movq	%rbx, 0(%r14)
	popq	%r14
	movl	$0, %eax
	leave
	ret
.Ldel16:
	pushq	%r14
	movq	-20(%rbp), %r14
	movq	0(%r14), %r15
	popq	%r14
	movq	%r15, %rbx
	movq	%rbx, -20(%rbp)
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
.Ldel15:
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
	je	.Ldel14
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
	pushq	%r14
	movq	-8(%rbp), %r14
	movq	0(%r14), %r15
	popq	%r14
	movq	%r15, %rbx
	movq	%rbx, -24(%rbp)
	jmp	.Lstitch18
.Lstitch17:
	pushq	%r14
	movq	-24(%rbp), %r14
	movq	0(%r14), %r15
	popq	%r14
	movq	%r15, %rbx
	movq	%rbx, -24(%rbp)
.Lstitch18:
	pushq	%rdi
	pushq	%rdx
	movq	$0, %rdx
	pushq	%r14
	movq	-24(%rbp), %r14
	movq	0(%r14), %r15
	popq	%r14
	movq	%r15, %rdi
	cmpq	%rdx, %rdi
	setne	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$1, %al
	je	.Lstitch17
	pushq	%r14
	movq	-24(%rbp), %r14
	pushq	%r14
	movq	-16(%rbp), %r14
	movq	0(%r14), %r15
	popq	%r14
	movq	%r15, %rbx
	movq	%rbx, 0(%r14)
	popq	%r14
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
	movl	-12(%rbp), %r15d
	movl	%r15d, %edi
	cmpl	%edx, %edi
	setl	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.Lget19
	movq	$0, %rax
	leave
	ret
.Lget19:
	pushq	%r14
	movq	-8(%rbp), %r14
	movq	0(%r14), %r15
	popq	%r14
	movq	%r15, %rbx
	movq	%rbx, -20(%rbp)
	movq	-20(%rbp), %r15
	movq	%r15, %rbx
	movq	%rbx, -28(%rbp)
	movl	$0, %ebx
	movl	%ebx, -32(%rbp)
	jmp	.Lget21
.Lget20:
	pushq	%r14
	movq	-28(%rbp), %r14
	movq	0(%r14), %r15
	popq	%r14
	movq	%r15, %rbx
	movq	%rbx, -28(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movl	-32(%rbp), %r15d
	movl	%r15d, %edi
	add	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -32(%rbp)
.Lget21:
	pushq	%rdi
	pushq	%rdx
	movl	-12(%rbp), %r15d
	movl	%r15d, %edx
	movl	-32(%rbp), %r15d
	movl	%r15d, %edi
	cmpl	%edx, %edi
	setl	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$1, %al
	je	.Lget20
	pushq	%r14
	movq	-28(%rbp), %r14
	movq	8(%r14), %r15
	popq	%r14
	movq	%r15, %rax
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
	pushq	%r14
	movq	-24(%rbp), %r14
	movq	-16(%rbp), %r15
	movq	%r15, %rbx
	movq	%rbx, 8(%r14)
	popq	%r14
	pushq	%r14
	movq	-24(%rbp), %r14
	movq	$0, %rbx
	movq	%rbx, 0(%r14)
	popq	%r14
	pushq	%r14
	movq	-8(%rbp), %r14
	movq	0(%r14), %r15
	popq	%r14
	movq	%r15, %rbx
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
	je	.Lappend22
	pushq	%r14
	movq	-8(%rbp), %r14
	movq	-24(%rbp), %r15
	movq	%r15, %rbx
	movq	%rbx, 0(%r14)
	popq	%r14
	movl	$0, %eax
	leave
	ret
.Lappend22:
	pushq	%r14
	movq	-8(%rbp), %r14
	movq	0(%r14), %r15
	popq	%r14
	movq	%r15, %rbx
	movq	%rbx, -40(%rbp)
	pushq	%r14
	movq	-40(%rbp), %r14
	movq	0(%r14), %r15
	popq	%r14
	movq	%r15, %rbx
	movq	%rbx, -48(%rbp)
	jmp	.Lappend24
.Lappend23:
	movq	-48(%rbp), %r15
	movq	%r15, %rbx
	movq	%rbx, -40(%rbp)
	pushq	%r14
	movq	-40(%rbp), %r14
	movq	0(%r14), %r15
	popq	%r14
	movq	%r15, %rbx
	movq	%rbx, -48(%rbp)
.Lappend24:
	pushq	%rdi
	pushq	%rdx
	movq	$0, %rdx
	movq	-48(%rbp), %r15
	movq	%r15, %rdi
	cmpq	%rdx, %rdi
	setne	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$1, %al
	je	.Lappend23
	pushq	%r14
	movq	-40(%rbp), %r14
	movq	-24(%rbp), %r15
	movq	%r15, %rbx
	movq	%rbx, 0(%r14)
	popq	%r14
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
	movl	-20(%rbp), %r15d
	movl	%r15d, %edi
	cmpl	%edx, %edi
	setg	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %dl
	pushq	%rdi
	pushq	%rdx
	movl	$0, %edx
	movl	-20(%rbp), %r15d
	movl	%r15d, %edi
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
	je	.Linsert25
	movl	$-1, %eax
	leave
	ret
.Linsert25:
	pushq	%rdx
	pushq	%rdi
	movl	$16, %eax
	movl	%eax, %edi
	call	malloc
	popq	%rdi
	popq	%rdx
	movq	%rax, %rbx
	movq	%rbx, -28(%rbp)
	pushq	%r14
	movq	-28(%rbp), %r14
	movq	-16(%rbp), %r15
	movq	%r15, %rbx
	movq	%rbx, 8(%r14)
	popq	%r14
	pushq	%r14
	movq	-28(%rbp), %r14
	movq	$0, %rbx
	movq	%rbx, 0(%r14)
	popq	%r14
	pushq	%rdi
	pushq	%rdx
	movl	$0, %edx
	movl	-20(%rbp), %r15d
	movl	%r15d, %edi
	cmpl	%edx, %edi
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.Linsert26
	pushq	%r14
	movq	-28(%rbp), %r14
	pushq	%r14
	movq	-8(%rbp), %r14
	movq	0(%r14), %r15
	popq	%r14
	movq	%r15, %rbx
	movq	%rbx, 0(%r14)
	popq	%r14
	pushq	%r14
	movq	-8(%rbp), %r14
	movq	-28(%rbp), %r15
	movq	%r15, %rbx
	movq	%rbx, 0(%r14)
	popq	%r14
	movl	$0, %eax
	leave
	ret
.Linsert26:
	pushq	%r14
	movq	-8(%rbp), %r14
	movq	0(%r14), %r15
	popq	%r14
	movq	%r15, %rbx
	movq	%rbx, -36(%rbp)
	movl	$0, %ebx
	movl	%ebx, -40(%rbp)
	jmp	.Linsert28
.Linsert27:
	pushq	%r14
	movq	-36(%rbp), %r14
	movq	0(%r14), %r15
	popq	%r14
	movq	%r15, %rbx
	movq	%rbx, -36(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movl	-40(%rbp), %r15d
	movl	%r15d, %edi
	add	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -40(%rbp)
.Linsert28:
	pushq	%rdi
	pushq	%rdx
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movl	-20(%rbp), %r15d
	movl	%r15d, %edi
	sub	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %edx
	movl	-40(%rbp), %r15d
	movl	%r15d, %edi
	cmpl	%edx, %edi
	setl	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$1, %al
	je	.Linsert27
	pushq	%r14
	movq	-36(%rbp), %r14
	movq	0(%r14), %r15
	popq	%r14
	movq	%r15, %rbx
	movq	%rbx, -44(%rbp)
	pushq	%r14
	movq	-36(%rbp), %r14
	movq	-28(%rbp), %r15
	movq	%r15, %rbx
	movq	%rbx, 0(%r14)
	popq	%r14
	pushq	%r14
	movq	-28(%rbp), %r14
	movq	-44(%rbp), %r15
	movq	%r15, %rbx
	movq	%rbx, 0(%r14)
	popq	%r14
	movl	$0, %eax
	leave
	ret
pub_LinkedList_size:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$32, %rsp
	movq	%rdi, -8(%rbp)
	pushq	%r14
	movq	-8(%rbp), %r14
	movq	0(%r14), %r15
	popq	%r14
	movq	%r15, %rbx
	movq	%rbx, -16(%rbp)
	movl	$0, %ebx
	movl	%ebx, -20(%rbp)
	jmp	.Lsize30
.Lsize29:
	pushq	%r14
	movq	-16(%rbp), %r14
	movq	0(%r14), %r15
	popq	%r14
	movq	%r15, %rbx
	movq	%rbx, -16(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movl	-20(%rbp), %r15d
	movl	%r15d, %edi
	add	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -20(%rbp)
.Lsize30:
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
	je	.Lsize29
	movl	-20(%rbp), %r15d
	movl	%r15d, %eax
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
	movl	$16, %eax
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
	pushq	%r14
	movq	-8(%rbp), %r14
	movq	0(%r14), %r15
	popq	%r14
	movq	%r15, %rbx
	movq	%rbx, -40(%rbp)
	jmp	.Lsort32
.Lsort31:
	pushq	%r14
	movq	-40(%rbp), %r14
	movq	8(%r14), %r15
	popq	%r14
	movq	%r15, %rbx
	movq	%rbx, -48(%rbp)
	movl	$0, %ebx
	movl	%ebx, -52(%rbp)
	movb	$1, %bl
	movb	%bl, -53(%rbp)
	jmp	.Lsort34
.Lsort33:
	pushq	%rdx
	lea	-32(%rbp), %rax
	pushq	%rdi
	movq	(%rax), %rax
	movq	%rax, %rdi
	movl	-52(%rbp), %r15d
	pushq	%rsi
	movl	%r15d, %eax
	movl	%eax, %esi
	call	pub_LinkedList_get
	popq	%rsi
	popq	%rdi
	popq	%rdx
	movq	%rax, %rbx
	movq	%rbx, -61(%rbp)
	pushq	%rdx
	movq	-16(%rbp), %r15
	movq	%r15, %r11
	movq	-48(%rbp), %r15
	pushq	%rdi
	movq	%r15, %rax
	movq	%rax, %rdi
	movq	-61(%rbp), %r15
	pushq	%rsi
	movq	%r15, %rax
	movq	%rax, %rsi
	movq	-24(%rbp), %r15
	pushq	%rdx
	movq	%r15, %rax
	movq	%rax, %rdx
	call	*%r11
	popq	%rdx
	popq	%rsi
	popq	%rdi
	popq	%rdx
	movb	%al, %bl
	movb	%bl, -62(%rbp)
	pushq	%rdi
	pushq	%rdx
	movb	$1, %dl
	movb	-62(%rbp), %r15b
	movb	%r15b, %dil
	cmpb	%dl, %dil
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.Lsort35
	pushq	%rdx
	lea	-32(%rbp), %rax
	pushq	%rdi
	movq	(%rax), %rax
	movq	%rax, %rdi
	movq	-48(%rbp), %r15
	pushq	%rsi
	movq	%r15, %rax
	movq	%rax, %rsi
	movl	-52(%rbp), %r15d
	pushq	%rdx
	movl	%r15d, %eax
	movl	%eax, %edx
	call	pub_LinkedList_insert
	popq	%rdx
	popq	%rsi
	popq	%rdi
	popq	%rdx
	movb	$0, %bl
	movb	%bl, -53(%rbp)
.Lsort35:
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movl	-52(%rbp), %r15d
	movl	%r15d, %edi
	add	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -52(%rbp)
.Lsort34:
	pushq	%rdi
	pushq	%rdx
	movb	-53(%rbp), %r15b
	movb	%r15b, %dl
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
	movl	-52(%rbp), %r15d
	movl	%r15d, %edi
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
	je	.Lsort33
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
	movl	-52(%rbp), %r15d
	movl	%r15d, %edi
	cmpl	%edx, %edi
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.Lsort36
	pushq	%rdx
	lea	-32(%rbp), %rax
	pushq	%rdi
	movq	(%rax), %rax
	movq	%rax, %rdi
	movq	-48(%rbp), %r15
	pushq	%rsi
	movq	%r15, %rax
	movq	%rax, %rsi
	call	pub_LinkedList_append
	popq	%rsi
	popq	%rdi
	popq	%rdx
.Lsort36:
	pushq	%r14
	movq	-40(%rbp), %r14
	movq	0(%r14), %r15
	popq	%r14
	movq	%r15, %rbx
	movq	%rbx, -40(%rbp)
.Lsort32:
	pushq	%rdi
	pushq	%rdx
	movq	$0, %rdx
	movq	-40(%rbp), %r15
	movq	%r15, %rdi
	cmpq	%rdx, %rdi
	setne	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$1, %al
	je	.Lsort31
	movq	-32(%rbp), %r15
	movq	%r15, %rax
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
	movl	$16, %eax
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
	pushq	%r14
	movq	-8(%rbp), %r14
	movq	0(%r14), %r15
	popq	%r14
	movq	%r15, %rbx
	movq	%rbx, -40(%rbp)
	movl	$0, %ebx
	movl	%ebx, -44(%rbp)
	jmp	.Lmap38
.Lmap37:
	pushq	%r14
	movq	-40(%rbp), %r14
	movq	8(%r14), %r15
	popq	%r14
	movq	%r15, %rbx
	movq	%rbx, -52(%rbp)
	pushq	%rdx
	movq	-16(%rbp), %r15
	movq	%r15, %r11
	movq	-52(%rbp), %r15
	pushq	%rdi
	movq	%r15, %rax
	movq	%rax, %rdi
	movl	-44(%rbp), %r15d
	pushq	%rsi
	movl	%r15d, %eax
	movl	%eax, %esi
	movq	-24(%rbp), %r15
	pushq	%rdx
	movq	%r15, %rax
	movq	%rax, %rdx
	call	*%r11
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
	movq	-60(%rbp), %r15
	pushq	%rsi
	movq	%r15, %rax
	movq	%rax, %rsi
	call	pub_LinkedList_append
	popq	%rsi
	popq	%rdi
	popq	%rdx
	pushq	%r14
	movq	-40(%rbp), %r14
	movq	0(%r14), %r15
	popq	%r14
	movq	%r15, %rbx
	movq	%rbx, -40(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movl	-44(%rbp), %r15d
	movl	%r15d, %edi
	add	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -44(%rbp)
.Lmap38:
	pushq	%rdi
	pushq	%rdx
	movq	$0, %rdx
	movq	-40(%rbp), %r15
	movq	%r15, %rdi
	cmpq	%rdx, %rdi
	setne	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$1, %al
	je	.Lmap37
	movq	-32(%rbp), %r15
	movq	%r15, %rax
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
	movq	-16(%rbp), %r15
	movq	%r15, %rax
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
	pushq	%r14
	movq	-8(%rbp), %r14
	movl	28(%r14), %r15d
	popq	%r14
	movl	%r15d, %ebx
	movl	%ebx, -28(%rbp)
	pushq	%r14
	movq	-8(%rbp), %r14
	movl	16(%r14), %r15d
	popq	%r14
	movl	%r15d, %ebx
	movl	%ebx, -40(%rbp)
	movl	$0, %ebx
	movl	%ebx, -52(%rbp)
	jmp	.LforEachArray40
.LforEachArray39:
	pushq	%rdi
	pushq	%rdx
	movl	-40(%rbp), %r15d
	movl	%r15d, %edx
	movl	-52(%rbp), %r15d
	movl	%r15d, %edi
	imul	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -56(%rbp)
	pushq	%r14
	movq	-8(%rbp), %r14
	movq	20(%r14), %r15
	popq	%r14
	movq	%r15, %rbx
	movq	%rbx, -48(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	-56(%rbp), %r15d
	movl	%r15d, %edx
	movq	-48(%rbp), %r15
	movq	%r15, %rdi
	add	%rdx, %rdi
	movq	%rdi, %rax
	popq	%rdx
	popq	%rdi
	movq	%rax, %rbx
	movq	%rbx, -48(%rbp)
	pushq	%rdx
	movq	-16(%rbp), %r15
	movq	%r15, %r11
	movq	-48(%rbp), %r15
	pushq	%rdi
	movq	%r15, %rax
	movq	%rax, %rdi
	movl	-52(%rbp), %r15d
	pushq	%rsi
	movl	%r15d, %eax
	movl	%eax, %esi
	movq	-24(%rbp), %r15
	pushq	%rdx
	movq	%r15, %rax
	movq	%rax, %rdx
	call	*%r11
	popq	%rdx
	popq	%rsi
	popq	%rdi
	popq	%rdx
	movl	%eax, %ebx
	movl	%ebx, -60(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movl	-60(%rbp), %r15d
	movl	%r15d, %edi
	cmpl	%edx, %edi
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.LforEachArray41
	movl	$0, %eax
	leave
	ret
.LforEachArray41:
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movl	-52(%rbp), %r15d
	movl	%r15d, %edi
	add	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -52(%rbp)
.LforEachArray40:
	pushq	%rdi
	pushq	%rdx
	movl	-28(%rbp), %r15d
	movl	%r15d, %edx
	movl	-52(%rbp), %r15d
	movl	%r15d, %edi
	cmpl	%edx, %edi
	setl	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$1, %al
	je	.LforEachArray39
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
	pushq	%r14
	movq	-8(%rbp), %r14
	movq	0(%r14), %r15
	popq	%r14
	movq	%r15, %rbx
	movq	%rbx, -32(%rbp)
	movl	$0, %ebx
	movl	%ebx, -36(%rbp)
	jmp	.Lfind43
.Lfind42:
	pushq	%r14
	movq	-32(%rbp), %r14
	movq	8(%r14), %r15
	popq	%r14
	movq	%r15, %rbx
	movq	%rbx, -44(%rbp)
	pushq	%rdx
	movq	-16(%rbp), %r15
	movq	%r15, %r11
	movq	-44(%rbp), %r15
	pushq	%rdi
	movq	%r15, %rax
	movq	%rax, %rdi
	movl	-36(%rbp), %r15d
	pushq	%rsi
	movl	%r15d, %eax
	movl	%eax, %esi
	movq	-24(%rbp), %r15
	pushq	%rdx
	movq	%r15, %rax
	movq	%rax, %rdx
	call	*%r11
	popq	%rdx
	popq	%rsi
	popq	%rdi
	popq	%rdx
	movb	%al, %bl
	movb	%bl, -45(%rbp)
	pushq	%rdi
	pushq	%rdx
	movb	$1, %dl
	movb	-45(%rbp), %r15b
	movb	%r15b, %dil
	cmpb	%dl, %dil
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.Lfind44
	movl	-36(%rbp), %r15d
	movl	%r15d, %eax
	leave
	ret
.Lfind44:
	pushq	%r14
	movq	-32(%rbp), %r14
	movq	0(%r14), %r15
	popq	%r14
	movq	%r15, %rbx
	movq	%rbx, -32(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movl	-36(%rbp), %r15d
	movl	%r15d, %edi
	add	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -36(%rbp)
.Lfind43:
	pushq	%rdi
	pushq	%rdx
	movq	$0, %rdx
	movq	-32(%rbp), %r15
	movq	%r15, %rdi
	cmpq	%rdx, %rdi
	setne	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$1, %al
	je	.Lfind42
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
	pushq	%r14
	movq	-8(%rbp), %r14
	movl	16(%r14), %r15d
	popq	%r14
	movl	%r15d, %ebx
	movl	%ebx, -40(%rbp)
	pushq	%r14
	movq	-8(%rbp), %r14
	movl	28(%r14), %r15d
	popq	%r14
	movl	%r15d, %ebx
	movl	%ebx, -36(%rbp)
	movl	$0, %ebx
	movl	%ebx, -44(%rbp)
	jmp	.LwhereArray46
.LwhereArray45:
	pushq	%rdi
	pushq	%rdx
	movl	-40(%rbp), %r15d
	movl	%r15d, %edx
	movl	-44(%rbp), %r15d
	movl	%r15d, %edi
	imul	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -48(%rbp)
	pushq	%r14
	movq	-8(%rbp), %r14
	movq	20(%r14), %r15
	popq	%r14
	movq	%r15, %rbx
	movq	%rbx, -56(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	-48(%rbp), %r15d
	movl	%r15d, %edx
	movq	-56(%rbp), %r15
	movq	%r15, %rdi
	add	%rdx, %rdi
	movq	%rdi, %rax
	popq	%rdx
	popq	%rdi
	movq	%rax, %rbx
	movq	%rbx, -56(%rbp)
	movq	-56(%rbp), %r15
	movq	%r15, %rbx
	movq	%rbx, -64(%rbp)
	pushq	%rdx
	movq	-16(%rbp), %r15
	movq	%r15, %r11
	movq	-56(%rbp), %r15
	pushq	%rdi
	movq	%r15, %rax
	movq	%rax, %rdi
	movl	-44(%rbp), %r15d
	pushq	%rsi
	movl	%r15d, %eax
	movl	%eax, %esi
	movq	-24(%rbp), %r15
	pushq	%rdx
	movq	%r15, %rax
	movq	%rax, %rdx
	call	*%r11
	popq	%rdx
	popq	%rsi
	popq	%rdi
	popq	%rdx
	movb	%al, %bl
	movb	%bl, -65(%rbp)
	pushq	%rdi
	pushq	%rdx
	movb	$1, %dl
	movb	-65(%rbp), %r15b
	movb	%r15b, %dil
	cmpb	%dl, %dil
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.LwhereArray47
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
	pushq	%r14
	movq	-32(%rbp), %r14
	movl	28(%r14), %r15d
	popq	%r14
	movl	%r15d, %ebx
	movl	%ebx, -69(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movl	-69(%rbp), %r15d
	movl	%r15d, %edi
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
	movl	-69(%rbp), %r15d
	pushq	%rsi
	movl	%r15d, %eax
	movl	%eax, %esi
	call	pub_Array_at
	popq	%rsi
	popq	%rdi
	popq	%rdx
	movq	%rax, %rbx
	movq	%rbx, -77(%rbp)
	movq	-64(%rbp), %r15
	movq	-77(%rbp), %rax
	movq	%r15, %rbx
	movq	%rbx, (%rax)
.LwhereArray47:
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movl	-44(%rbp), %r15d
	movl	%r15d, %edi
	add	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -44(%rbp)
.LwhereArray46:
	pushq	%rdi
	pushq	%rdx
	movl	-36(%rbp), %r15d
	movl	%r15d, %edx
	movl	-44(%rbp), %r15d
	movl	%r15d, %edi
	cmpl	%edx, %edi
	setl	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$1, %al
	je	.LwhereArray45
	movq	-32(%rbp), %r15
	movq	%r15, %rax
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
	pushq	%r14
	movq	-8(%rbp), %r14
	movq	$lambda_1650419093825_48, %rbx
	movq	%rbx, 0(%r14)
	popq	%r14
	pushq	%r14
	movq	-8(%rbp), %r14
	movq	$lambda_1650419093825_50, %rbx
	movq	%rbx, 8(%r14)
	popq	%r14
	pushq	%r14
	movq	-8(%rbp), %r14
	movl	-16(%rbp), %r15d
	movl	%r15d, %ebx
	movl	%ebx, 28(%r14)
	popq	%r14
	pushq	%r14
	movq	-8(%rbp), %r14
	movl	-12(%rbp), %r15d
	movl	%r15d, %ebx
	movl	%ebx, 16(%r14)
	popq	%r14
	pushq	%rdi
	pushq	%rdx
	movl	-12(%rbp), %r15d
	movl	%r15d, %edx
	movl	-16(%rbp), %r15d
	movl	%r15d, %edi
	imul	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -16(%rbp)
	pushq	%r14
	movq	-8(%rbp), %r14
	pushq	%rdx
	movl	-16(%rbp), %r15d
	pushq	%rdi
	movl	%r15d, %eax
	movl	%eax, %edi
	call	malloc
	popq	%rdi
	popq	%rdx
	movq	%rax, %rbx
	movq	%rbx, 20(%r14)
	popq	%r14
	pushq	%r14
	movq	-8(%rbp), %r14
	movq	$forEachArray, %rbx
	movq	%rbx, 0(%r14)
	popq	%r14
	pushq	%r14
	movq	-8(%rbp), %r14
	movq	$whereArray, %rbx
	movq	%rbx, 8(%r14)
	popq	%r14
	movq	-8(%rbp), %r15
	movq	%r15, %rax
	leave
	ret
pub_Array_at:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$32, %rsp
	movq	%rdi, -8(%rbp)
	movl	%esi, -12(%rbp)
	pushq	%r14
	movq	-8(%rbp), %r14
	movl	16(%r14), %r15d
	popq	%r14
	movl	%r15d, %ebx
	movl	%ebx, -16(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	-16(%rbp), %r15d
	movl	%r15d, %edx
	movl	-12(%rbp), %r15d
	movl	%r15d, %edi
	imul	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -20(%rbp)
	pushq	%r14
	movq	-8(%rbp), %r14
	movq	20(%r14), %r15
	popq	%r14
	movq	%r15, %rbx
	movq	%rbx, -28(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	-20(%rbp), %r15d
	movl	%r15d, %edx
	movq	-28(%rbp), %r15
	movq	%r15, %rdi
	add	%rdx, %rdi
	movq	%rdi, %rax
	popq	%rdx
	popq	%rdi
	movq	%rax, %rbx
	movq	%rbx, -28(%rbp)
	movq	-28(%rbp), %r15
	movq	%r15, %rax
	leave
	ret
pub_Array_del:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$16, %rsp
	movq	%rdi, -8(%rbp)
	pushq	%rdx
	pushq	%r14
	movq	-8(%rbp), %r14
	movq	20(%r14), %r15
	popq	%r14
	pushq	%rdi
	movq	%r15, %rax
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
	pushq	%r14
	movq	-8(%rbp), %r14
	movq	0(%r14), %r15
	popq	%r14
	movq	%r15, %rbx
	movq	%rbx, -16(%rbp)
	jmp	.Lerace53
.Lerace52:
	pushq	%r14
	movq	-16(%rbp), %r14
	movq	0(%r14), %r15
	popq	%r14
	movq	%r15, %rbx
	movq	%rbx, -24(%rbp)
	pushq	%rdx
	pushq	%r14
	movq	-16(%rbp), %r14
	movq	8(%r14), %r15
	popq	%r14
	pushq	%rdi
	movq	%r15, %rax
	movq	%rax, %rdi
	call	free
	popq	%rdi
	popq	%rdx
	pushq	%rdx
	movq	-16(%rbp), %r15
	pushq	%rdi
	movq	%r15, %rax
	movq	%rax, %rdi
	call	free
	popq	%rdi
	popq	%rdx
	movq	-24(%rbp), %r15
	movq	%r15, %rbx
	movq	%rbx, -16(%rbp)
.Lerace53:
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
	je	.Lerace52
	pushq	%rdx
	movq	-8(%rbp), %r15
	pushq	%rdi
	movq	%r15, %rax
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
	pushq	%r14
	movq	-8(%rbp), %r14
	movq	0(%r14), %r15
	popq	%r14
	movq	%r15, %rbx
	movq	%rbx, -16(%rbp)
	movq	$0, %rbx
	movq	%rbx, -24(%rbp)
	jmp	.Lreverse55
.Lreverse54:
	pushq	%r14
	movq	-16(%rbp), %r14
	movq	0(%r14), %r15
	popq	%r14
	movq	%r15, %rbx
	movq	%rbx, -32(%rbp)
	pushq	%r14
	movq	-16(%rbp), %r14
	movq	-24(%rbp), %r15
	movq	%r15, %rbx
	movq	%rbx, 0(%r14)
	popq	%r14
	movq	-16(%rbp), %r15
	movq	%r15, %rbx
	movq	%rbx, -24(%rbp)
	movq	-32(%rbp), %r15
	movq	%r15, %rbx
	movq	%rbx, -16(%rbp)
.Lreverse55:
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
	je	.Lreverse54
	pushq	%r14
	movq	-8(%rbp), %r14
	movq	-24(%rbp), %r15
	movq	%r15, %rbx
	movq	%rbx, 0(%r14)
	popq	%r14
	movq	-8(%rbp), %r15
	movq	%r15, %rax
	leave
	ret
pub_Array_extend:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$32, %rsp
	movq	%rdi, -8(%rbp)
	movl	%esi, -12(%rbp)
	pushq	%r14
	movq	-8(%rbp), %r14
	movq	20(%r14), %r15
	popq	%r14
	movq	%r15, %rbx
	movq	%rbx, -20(%rbp)
	pushq	%r14
	movq	-8(%rbp), %r14
	movl	28(%r14), %r15d
	popq	%r14
	movl	%r15d, %ebx
	movl	%ebx, -24(%rbp)
	pushq	%r14
	movq	-8(%rbp), %r14
	pushq	%rdi
	pushq	%rdx
	movl	-12(%rbp), %r15d
	movl	%r15d, %edx
	movl	-24(%rbp), %r15d
	movl	%r15d, %edi
	add	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, 28(%r14)
	popq	%r14
	pushq	%r14
	movq	-8(%rbp), %r14
	movl	16(%r14), %r15d
	popq	%r14
	movl	%r15d, %ebx
	movl	%ebx, -28(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	-24(%rbp), %r15d
	movl	%r15d, %edx
	movl	-12(%rbp), %r15d
	movl	%r15d, %edi
	add	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -12(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	-28(%rbp), %r15d
	movl	%r15d, %edx
	movl	-12(%rbp), %r15d
	movl	%r15d, %edi
	imul	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -12(%rbp)
	pushq	%r14
	movq	-8(%rbp), %r14
	pushq	%rdx
	movq	-20(%rbp), %r15
	pushq	%rdi
	movq	%r15, %rax
	movq	%rax, %rdi
	movl	-12(%rbp), %r15d
	pushq	%rsi
	movl	%r15d, %eax
	movl	%eax, %esi
	call	realloc
	popq	%rsi
	popq	%rdi
	popq	%rdx
	movq	%rax, %rbx
	movq	%rbx, 20(%r14)
	popq	%r14
	pushq	%rdi
	pushq	%rdx
	movq	$0, %rdx
	pushq	%r14
	movq	-8(%rbp), %r14
	movq	20(%r14), %r15
	popq	%r14
	movq	%r15, %rdi
	cmpq	%rdx, %rdi
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.Lextend56
	pushq	%rdx
	pushq	%rdi
	movq	$.strArray.lambda_1650419093825_5057, %rax
	movq	%rax, %rdi
	call	panic
	popq	%rdi
	popq	%rdx
.Lextend56:
	movl	-12(%rbp), %r15d
	movl	%r15d, %eax
	leave
	ret
lambda_1650419093816_0:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$16, %rsp
	pushq	%rdx
	pushq	%rdi
	movq	$.strICollection.lambda_1650419093816_01, %rax
	movq	%rax, %rdi
	call	panic
	popq	%rdi
	popq	%rdx
	leave
	ret
lambda_1650419093816_2:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$16, %rsp
	pushq	%rdx
	pushq	%rdi
	movq	$.strICollection.lambda_1650419093816_23, %rax
	movq	%rax, %rdi
	call	panic
	popq	%rdi
	popq	%rdx
	leave
	ret
lambda_1650419093825_48:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$16, %rsp
	pushq	%rdx
	pushq	%rdi
	movq	$.strArray.lambda_1650419093825_4849, %rax
	movq	%rax, %rdi
	call	panic
	popq	%rdi
	popq	%rdx
	leave
	ret
lambda_1650419093825_50:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$16, %rsp
	pushq	%rdx
	pushq	%rdi
	movq	$.strArray.lambda_1650419093825_5051, %rax
	movq	%rax, %rdi
	call	panic
	popq	%rdi
	popq	%rdx
	leave
	ret


.data

.strArray.lambda_1650419093825_5051:
	.asciz	 "Function (where) not implemented on this instance"
.strArray.lambda_1650419093825_4849:
	.asciz	 "Function (forEach) not implemented on this instance"
.strICollection.lambda_1650419093816_23:
	.asciz	 "Function (where) not implemented on this instance"
.strICollection.lambda_1650419093816_01:
	.asciz	 "Function (forEach) not implemented on this instance"
.strArray.lambda_1650419093825_5057:
	.asciz	 "bad realloc\n"


.bss

