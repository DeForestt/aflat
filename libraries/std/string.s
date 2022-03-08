.global	upper
.global	lower
.global	pub_String_len
.global	pub_String_toUpper
.global	pub_String_toLower
.global	pub_String_replace
.global	pub_String_trim
.global	pub_String_copy
.global	getString
.global	printString
.global	pub_String_delete
.global	pub_String_indexOf
.global	pub_String_at
.global	pub_String_equal
.global	pub_String_compare
.global	pub_String_subString
.global	pub_String_stitch
.global	pub_String_concat
.global	pub_String_init


.text

pub_String_init:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$32, %rsp
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	pushq	%rdi
	movq	-16(%rbp), %rax
	movq	%rax, %rdi
	call	len
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -20(%rbp)
	mov	$1, %edx
	mov	-20(%rbp), %eax
	add	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -20(%rbp)
	pushq	%rdi
	movl	-20(%rbp), %eax
	movl	%eax, %edi
	call	malloc
	popq	%rdi
	movq	-8(%rbp), %rdx
	movq	%rax, %rbx
	movq	%rbx, 0(%rdx)
	movq	-8(%rbp), %rdx
	movq	0(%rdx), %rbx
	movq	%rbx, -28(%rbp)
	pushq	%rdi
	movq	-16(%rbp), %rax
	movq	%rax, %rdi
	pushq	%rsi
	movq	-28(%rbp), %rax
	movq	%rax, %rsi
	pushq	%rdx
	movl	-20(%rbp), %eax
	movl	%eax, %edx
	call	memcopy
	popq	%rdx
	popq	%rsi
	popq	%rdi
	movq	-8(%rbp), %rax
	leave
	ret
pub_String_concat:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$64, %rsp
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	movq	-8(%rbp), %rdx
	pushq	%rdi
	movq	0(%rdx), %rax
	movq	%rax, %rdi
	call	len
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -20(%rbp)
	movq	-16(%rbp), %rdx
	pushq	%rdi
	movq	0(%rdx), %rax
	movq	%rax, %rdi
	call	len
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -24(%rbp)
	mov	$1, %edx
	mov	-24(%rbp), %eax
	add	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -24(%rbp)
	mov	-24(%rbp), %edx
	mov	-20(%rbp), %eax
	add	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -28(%rbp)
	pushq	%rdi
	movl	-28(%rbp), %eax
	movl	%eax, %edi
	call	malloc
	popq	%rdi
	movq	%rax, %rbx
	movq	%rbx, -36(%rbp)
	movq	-36(%rbp), %rbx
	movq	%rbx, -44(%rbp)
	movq	-8(%rbp), %rdx
	pushq	%rdi
	movq	0(%rdx), %rax
	movq	%rax, %rdi
	pushq	%rsi
	movq	-36(%rbp), %rax
	movq	%rax, %rsi
	pushq	%rdx
	movl	-20(%rbp), %eax
	movl	%eax, %edx
	call	memcopy
	popq	%rdx
	popq	%rsi
	popq	%rdi
	mov	-20(%rbp), %edx
	mov	-36(%rbp), %rax
	add	%rdx, %rax
	movl	%eax, %ebx
	movl	%ebx, -36(%rbp)
	movq	-16(%rbp), %rdx
	pushq	%rdi
	movq	0(%rdx), %rax
	movq	%rax, %rdi
	pushq	%rsi
	movq	-36(%rbp), %rax
	movq	%rax, %rsi
	pushq	%rdx
	movl	-24(%rbp), %eax
	movl	%eax, %edx
	call	memcopy
	popq	%rdx
	popq	%rsi
	popq	%rdi
	pushq	%rdi
	movl	$8, %eax
	movl	%eax, %edi
	call	malloc
	popq	%rdi
	pushq	%rdi
	movq	%rax, %rdi
	pushq	%rsi
	movq	-44(%rbp), %rax
	movq	%rax, %rsi
	call	pub_String_init
	popq	%rsi
	popq	%rdi
	movq	%rax, %rbx
	movq	%rbx, -52(%rbp)
	pushq	%rdi
	movq	-44(%rbp), %rax
	movq	%rax, %rdi
	call	free
	popq	%rdi
	movq	-52(%rbp), %rax
	leave
	ret
	leave
	ret
pub_String_stitch:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$64, %rsp
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	movq	-8(%rbp), %rdx
	pushq	%rdi
	movq	0(%rdx), %rax
	movq	%rax, %rdi
	call	len
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -20(%rbp)
	pushq	%rdi
	movq	-16(%rbp), %rax
	movq	%rax, %rdi
	call	len
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -24(%rbp)
	mov	$1, %edx
	mov	-24(%rbp), %eax
	add	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -24(%rbp)
	mov	-24(%rbp), %edx
	mov	-20(%rbp), %eax
	add	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -28(%rbp)
	pushq	%rdi
	movl	-28(%rbp), %eax
	movl	%eax, %edi
	call	malloc
	popq	%rdi
	movq	%rax, %rbx
	movq	%rbx, -36(%rbp)
	movq	-36(%rbp), %rbx
	movq	%rbx, -44(%rbp)
	movq	-8(%rbp), %rdx
	pushq	%rdi
	movq	0(%rdx), %rax
	movq	%rax, %rdi
	pushq	%rsi
	movq	-36(%rbp), %rax
	movq	%rax, %rsi
	pushq	%rdx
	movl	-20(%rbp), %eax
	movl	%eax, %edx
	call	memcopy
	popq	%rdx
	popq	%rsi
	popq	%rdi
	mov	-20(%rbp), %edx
	mov	-36(%rbp), %rax
	add	%rdx, %rax
	movl	%eax, %ebx
	movl	%ebx, -36(%rbp)
	pushq	%rdi
	movq	-16(%rbp), %rax
	movq	%rax, %rdi
	pushq	%rsi
	movq	-36(%rbp), %rax
	movq	%rax, %rsi
	pushq	%rdx
	movl	-24(%rbp), %eax
	movl	%eax, %edx
	call	memcopy
	popq	%rdx
	popq	%rsi
	popq	%rdi
	pushq	%rdi
	movl	$8, %eax
	movl	%eax, %edi
	call	malloc
	popq	%rdi
	pushq	%rdi
	movq	%rax, %rdi
	pushq	%rsi
	movq	-44(%rbp), %rax
	movq	%rax, %rsi
	call	pub_String_init
	popq	%rsi
	popq	%rdi
	movq	%rax, %rbx
	movq	%rbx, -52(%rbp)
	pushq	%rdi
	movq	-44(%rbp), %rax
	movq	%rax, %rdi
	call	free
	popq	%rdi
	movq	-52(%rbp), %rax
	leave
	ret
	leave
	ret
pub_String_subString:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$64, %rsp
	movq	%rdi, -8(%rbp)
	movl	%esi, -12(%rbp)
	movl	%edx, -16(%rbp)
	movq	-8(%rbp), %rdx
	movq	0(%rdx), %rbx
	movq	%rbx, -24(%rbp)
	pushq	%rdi
	movq	-24(%rbp), %rax
	movq	%rax, %rdi
	call	len
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -28(%rbp)
	mov	-12(%rbp), %edx
	mov	-16(%rbp), %eax
	sub	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -32(%rbp)
	pushq	%rdi
	movl	-32(%rbp), %eax
	movl	%eax, %edi
	call	malloc
	popq	%rdi
	movq	%rax, %rbx
	movq	%rbx, -40(%rbp)
	movq	-40(%rbp), %rbx
	movq	%rbx, -48(%rbp)
	mov	-12(%rbp), %edx
	mov	-24(%rbp), %rax
	add	%rdx, %rax
	movl	%eax, %ebx
	movl	%ebx, -24(%rbp)
	pushq	%rdi
	movq	-24(%rbp), %rax
	movq	%rax, %rdi
	pushq	%rsi
	movq	-40(%rbp), %rax
	movq	%rax, %rsi
	pushq	%rdx
	movl	-32(%rbp), %eax
	movl	%eax, %edx
	call	memcopy
	popq	%rdx
	popq	%rsi
	popq	%rdi
	pushq	%rdi
	movl	$8, %eax
	movl	%eax, %edi
	call	malloc
	popq	%rdi
	pushq	%rdi
	movq	%rax, %rdi
	pushq	%rsi
	movq	-48(%rbp), %rax
	movq	%rax, %rsi
	call	pub_String_init
	popq	%rsi
	popq	%rdi
	movq	%rax, %rbx
	movq	%rbx, -56(%rbp)
	pushq	%rdi
	movq	-48(%rbp), %rax
	movq	%rax, %rdi
	call	free
	popq	%rdi
	movq	-56(%rbp), %rax
	leave
	ret
	leave
	ret
pub_String_compare:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$48, %rsp
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	movq	-16(%rbp), %rdx
	movq	0(%rdx), %rbx
	movq	%rbx, -24(%rbp)
	movq	-8(%rbp), %rdx
	movq	0(%rdx), %rbx
	movq	%rbx, -32(%rbp)
	pushq	%rdi
	movq	-24(%rbp), %rax
	movq	%rax, %rdi
	pushq	%rsi
	movq	-32(%rbp), %rax
	movq	%rax, %rsi
	call	str_comp
	popq	%rsi
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -36(%rbp)
	movl	-36(%rbp), %eax
	leave
	ret
	leave
	ret
pub_String_equal:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$32, %rsp
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	movq	-8(%rbp), %rdx
	pushq	%rdi
	movq	0(%rdx), %rax
	movq	%rax, %rdi
	pushq	%rsi
	movq	-16(%rbp), %rax
	movq	%rax, %rsi
	call	str_comp
	popq	%rsi
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -20(%rbp)
	movl	-20(%rbp), %eax
	leave
	ret
	leave
	ret
pub_String_at:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$16, %rsp
	movq	%rdi, -8(%rbp)
	movl	%esi, -12(%rbp)
	movq	-8(%rbp), %rdx
	pushq	%rdi
	movq	0(%rdx), %rax
	movq	%rax, %rdi
	pushq	%rsi
	movl	-12(%rbp), %eax
	movl	%eax, %esi
	call	str_at
	popq	%rsi
	popq	%rdi
	movb	%al, %bl
	movb	%bl, -13(%rbp)
	movb	-13(%rbp), %al
	leave
	ret
	leave
	ret
pub_String_indexOf:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$16, %rsp
	movq	%rdi, -8(%rbp)
	movb	%sil, -9(%rbp)
	movq	-8(%rbp), %rdx
	pushq	%rdi
	movq	0(%rdx), %rax
	movq	%rax, %rdi
	pushq	%rsi
	movb	-9(%rbp), %al
	movb	%al, %sil
	call	str_find
	popq	%rsi
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -13(%rbp)
	movl	-13(%rbp), %eax
	leave
	ret
	leave
	ret
pub_String_delete:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$16, %rsp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rdx
	movq	0(%rdx), %rbx
	movq	%rbx, -16(%rbp)
	pushq	%rdi
	movq	-16(%rbp), %rax
	movq	%rax, %rdi
	call	free
	popq	%rdi
	pushq	%rdi
	movq	-8(%rbp), %rax
	movq	%rax, %rdi
	call	free
	popq	%rdi
	leave
	ret
printString:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$16, %rsp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rdx
	movq	0(%rdx), %rbx
	movq	%rbx, -16(%rbp)
	pushq	%rdi
	movq	-16(%rbp), %rax
	movq	%rax, %rdi
	call	print
	popq	%rdi
	movq	-8(%rbp), %rax
	leave
	ret
	leave
	ret
getString:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$48, %rsp
	pushq	%rdi
	movl	$1, %eax
	movl	%eax, %edi
	call	malloc
	popq	%rdi
	movq	%rax, %rbx
	movq	%rbx, -8(%rbp)
	movq	-8(%rbp), %rbx
	movq	%rbx, -16(%rbp)
	movl	$1, %ebx
	movl	%ebx, -20(%rbp)
	movb	$32, %bl
	movb	%bl, -29(%rbp)
	movb	$10, %bl
	movb	%bl, -30(%rbp)
	movb	$0, %bl
	movb	%bl, -31(%rbp)
	movl	$0, %ebx
	movl	%ebx, -35(%rbp)
	jmp	.LgetString1
.LgetString0:
	lea	-29(%rbp), %rax
	movq	$1, %rdi
	movq	%rax, %rsi
	movq	$1, %rdx
	movq	$0, %rax
	syscall	
	mov	-35(%rbp), %edx
	mov	-8(%rbp), %rax
	add	%rdx, %rax
	movl	%eax, %ebx
	movl	%ebx, -8(%rbp)
	movq	-8(%rbp), %rax
	movb	-29(%rbp), %bl
	movb	%bl, (%rax)
	movb	-29(%rbp), %al
	movb	-30(%rbp), %cl
	cmpb	%cl, %al
	jne	.LgetString2
	movq	-8(%rbp), %rax
	movb	-31(%rbp), %bl
	movb	%bl, (%rax)
.LgetString2:
	mov	$1, %edx
	mov	-20(%rbp), %eax
	add	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -20(%rbp)
	pushq	%rdi
	movq	-16(%rbp), %rax
	movq	%rax, %rdi
	pushq	%rsi
	movl	-20(%rbp), %eax
	movl	%eax, %esi
	call	realloc
	popq	%rsi
	popq	%rdi
	movq	%rax, %rbx
	movq	%rbx, -16(%rbp)
	movq	-16(%rbp), %rbx
	movq	%rbx, -8(%rbp)
	mov	$1, %edx
	mov	-35(%rbp), %eax
	add	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -35(%rbp)
.LgetString1:
	movb	-29(%rbp), %al
	movb	-30(%rbp), %cl
	cmpb	%cl, %al
	jne	.LgetString0
	pushq	%rdi
	movl	$8, %eax
	movl	%eax, %edi
	call	malloc
	popq	%rdi
	pushq	%rdi
	movq	%rax, %rdi
	pushq	%rsi
	movq	-16(%rbp), %rax
	movq	%rax, %rsi
	call	pub_String_init
	popq	%rsi
	popq	%rdi
	movq	%rax, %rbx
	movq	%rbx, -43(%rbp)
	pushq	%rdi
	movq	-16(%rbp), %rax
	movq	%rax, %rdi
	call	free
	popq	%rdi
	movq	-43(%rbp), %rax
	leave
	ret
	leave
	ret
pub_String_copy:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$16, %rsp
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	lea	-8(%rbp), %rax
	pushq	%rdi
	movq	(%rax), %rax
	movq	%rax, %rdi
	call	pub_String_delete
	popq	%rdi
	movq	-16(%rbp), %rax
	leave
	ret
	leave
	ret
pub_String_trim:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$32, %rsp
	movq	%rdi, -8(%rbp)
	movb	%sil, -9(%rbp)
	lea	-8(%rbp), %rax
	pushq	%rdi
	movq	(%rax), %rax
	movq	%rax, %rdi
	call	pub_String_len
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -13(%rbp)
	movl	$0, %ebx
	movl	%ebx, -17(%rbp)
	lea	-8(%rbp), %rax
	pushq	%rdi
	movq	(%rax), %rax
	movq	%rax, %rdi
	pushq	%rsi
	movl	-17(%rbp), %eax
	movl	%eax, %esi
	call	pub_String_at
	popq	%rsi
	popq	%rdi
	movb	%al, %bl
	movb	%bl, -18(%rbp)
	jmp	.Ltrim6
.Ltrim5:
	mov	$1, %edx
	mov	-17(%rbp), %eax
	add	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -17(%rbp)
	lea	-8(%rbp), %rax
	pushq	%rdi
	movq	(%rax), %rax
	movq	%rax, %rdi
	pushq	%rsi
	movl	-17(%rbp), %eax
	movl	%eax, %esi
	call	pub_String_at
	popq	%rsi
	popq	%rdi
	movb	%al, %bl
	movb	%bl, -18(%rbp)
.Ltrim6:
	movb	-18(%rbp), %al
	movb	-9(%rbp), %cl
	cmpb	%cl, %al
	je	.Ltrim5
	mov	$1, %edx
	mov	-13(%rbp), %eax
	sub	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -22(%rbp)
	lea	-8(%rbp), %rax
	pushq	%rdi
	movq	(%rax), %rax
	movq	%rax, %rdi
	pushq	%rsi
	movl	-22(%rbp), %eax
	movl	%eax, %esi
	call	pub_String_at
	popq	%rsi
	popq	%rdi
	movb	%al, %bl
	movb	%bl, -18(%rbp)
	jmp	.Ltrim8
.Ltrim7:
	mov	$1, %edx
	mov	-22(%rbp), %eax
	sub	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -22(%rbp)
	lea	-8(%rbp), %rax
	pushq	%rdi
	movq	(%rax), %rax
	movq	%rax, %rdi
	pushq	%rsi
	movl	-22(%rbp), %eax
	movl	%eax, %esi
	call	pub_String_at
	popq	%rsi
	popq	%rdi
	movb	%al, %bl
	movb	%bl, -18(%rbp)
.Ltrim8:
	movb	-18(%rbp), %al
	movb	-9(%rbp), %cl
	cmpb	%cl, %al
	je	.Ltrim7
	mov	$1, %edx
	mov	-22(%rbp), %eax
	add	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -22(%rbp)
	lea	-8(%rbp), %rax
	pushq	%rdi
	movq	(%rax), %rax
	movq	%rax, %rdi
	pushq	%rsi
	movl	-17(%rbp), %eax
	movl	%eax, %esi
	pushq	%rdx
	movl	-22(%rbp), %eax
	movl	%eax, %edx
	call	pub_String_subString
	popq	%rdx
	popq	%rsi
	popq	%rdi
	movq	%rax, %rbx
	movq	%rbx, -30(%rbp)
	movq	-30(%rbp), %rax
	leave
	ret
	leave
	ret
pub_String_replace:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$64, %rsp
	movq	%rdi, -8(%rbp)
	movb	%sil, -9(%rbp)
	movb	%dl, -10(%rbp)
	lea	-8(%rbp), %rax
	pushq	%rdi
	movq	(%rax), %rax
	movq	%rax, %rdi
	call	pub_String_len
	popq	%rdi
	pushq	%rdi
	movl	%eax, %eax
	movl	%eax, %edi
	call	malloc
	popq	%rdi
	movq	%rax, %rbx
	movq	%rbx, -18(%rbp)
	movq	-18(%rbp), %rbx
	movq	%rbx, -26(%rbp)
	lea	-8(%rbp), %rax
	pushq	%rdi
	movq	(%rax), %rax
	movq	%rax, %rdi
	call	pub_String_len
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -30(%rbp)
	mov	$1, %edx
	mov	-30(%rbp), %eax
	add	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -30(%rbp)
	movq	-8(%rbp), %rdx
	movq	0(%rdx), %rbx
	movq	%rbx, -38(%rbp)
	pushq	%rdi
	movq	-38(%rbp), %rax
	movq	%rax, %rdi
	pushq	%rsi
	movq	-18(%rbp), %rax
	movq	%rax, %rsi
	pushq	%rdx
	movl	-30(%rbp), %eax
	movl	%eax, %edx
	call	memcopy
	popq	%rdx
	popq	%rsi
	popq	%rdi
	movl	$0, %ebx
	movl	%ebx, -42(%rbp)
	jmp	.Lreplace10
.Lreplace9:
	lea	-8(%rbp), %rax
	pushq	%rdi
	movq	(%rax), %rax
	movq	%rax, %rdi
	pushq	%rsi
	movl	-42(%rbp), %eax
	movl	%eax, %esi
	call	pub_String_at
	popq	%rsi
	popq	%rdi
	movb	%al, %al
	movb	-9(%rbp), %cl
	cmpb	%cl, %al
	jne	.Lreplace11
	mov	-42(%rbp), %edx
	mov	-18(%rbp), %rax
	add	%rdx, %rax
	movl	%eax, %ebx
	movl	%ebx, -18(%rbp)
	movq	-18(%rbp), %rax
	movb	-10(%rbp), %bl
	movb	%bl, (%rax)
.Lreplace11:
	mov	$1, %edx
	mov	-42(%rbp), %eax
	add	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -42(%rbp)
.Lreplace10:
	movl	-42(%rbp), %eax
	movl	-30(%rbp), %ecx
	cmpl	%ecx, %eax
	jl	.Lreplace9
	pushq	%rdi
	movl	$8, %eax
	movl	%eax, %edi
	call	malloc
	popq	%rdi
	pushq	%rdi
	movq	%rax, %rdi
	pushq	%rsi
	movq	-26(%rbp), %rax
	movq	%rax, %rsi
	call	pub_String_init
	popq	%rsi
	popq	%rdi
	movq	%rax, %rbx
	movq	%rbx, -50(%rbp)
	pushq	%rdi
	movq	-26(%rbp), %rax
	movq	%rax, %rdi
	call	free
	popq	%rdi
	movq	-50(%rbp), %rax
	leave
	ret
	leave
	ret
pub_String_toLower:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$48, %rsp
	movq	%rdi, -8(%rbp)
	lea	-8(%rbp), %rax
	pushq	%rdi
	movq	(%rax), %rax
	movq	%rax, %rdi
	call	pub_String_len
	popq	%rdi
	pushq	%rdi
	movl	%eax, %eax
	movl	%eax, %edi
	call	malloc
	popq	%rdi
	movq	%rax, %rbx
	movq	%rbx, -16(%rbp)
	movq	-16(%rbp), %rbx
	movq	%rbx, -24(%rbp)
	lea	-8(%rbp), %rax
	pushq	%rdi
	movq	(%rax), %rax
	movq	%rax, %rdi
	call	pub_String_len
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -28(%rbp)
	mov	$1, %edx
	mov	-28(%rbp), %eax
	add	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -28(%rbp)
	movq	-8(%rbp), %rdx
	movq	0(%rdx), %rbx
	movq	%rbx, -36(%rbp)
	movl	$0, %ebx
	movl	%ebx, -40(%rbp)
	jmp	.LtoLower15
.LtoLower14:
	lea	-8(%rbp), %rax
	pushq	%rdi
	movq	(%rax), %rax
	movq	%rax, %rdi
	pushq	%rsi
	movl	-40(%rbp), %eax
	movl	%eax, %esi
	call	pub_String_at
	popq	%rsi
	popq	%rdi
	movb	%al, %bl
	movb	%bl, -41(%rbp)
	pushq	%rdi
	movb	-41(%rbp), %al
	movb	%al, %dil
	call	lower
	popq	%rdi
	movb	%al, %bl
	movb	%bl, -41(%rbp)
	movq	-16(%rbp), %rax
	movb	-41(%rbp), %bl
	movb	%bl, (%rax)
	mov	$1, %edx
	mov	-16(%rbp), %rax
	add	%rdx, %rax
	movl	%eax, %ebx
	movl	%ebx, -16(%rbp)
	mov	$1, %edx
	mov	-40(%rbp), %eax
	add	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -40(%rbp)
.LtoLower15:
	movl	-40(%rbp), %eax
	movl	-28(%rbp), %ecx
	cmpl	%ecx, %eax
	jl	.LtoLower14
	pushq	%rdi
	movl	$8, %eax
	movl	%eax, %edi
	call	malloc
	popq	%rdi
	pushq	%rdi
	movq	%rax, %rdi
	pushq	%rsi
	movq	-24(%rbp), %rax
	movq	%rax, %rsi
	call	pub_String_init
	popq	%rsi
	popq	%rdi
	movq	%rax, %rbx
	movq	%rbx, -48(%rbp)
	pushq	%rdi
	movq	-24(%rbp), %rax
	movq	%rax, %rdi
	call	free
	popq	%rdi
	movq	-48(%rbp), %rax
	leave
	ret
	leave
	ret
pub_String_toUpper:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$48, %rsp
	movq	%rdi, -8(%rbp)
	lea	-8(%rbp), %rax
	pushq	%rdi
	movq	(%rax), %rax
	movq	%rax, %rdi
	call	pub_String_len
	popq	%rdi
	pushq	%rdi
	movl	%eax, %eax
	movl	%eax, %edi
	call	malloc
	popq	%rdi
	movq	%rax, %rbx
	movq	%rbx, -16(%rbp)
	movq	-16(%rbp), %rbx
	movq	%rbx, -24(%rbp)
	lea	-8(%rbp), %rax
	pushq	%rdi
	movq	(%rax), %rax
	movq	%rax, %rdi
	call	pub_String_len
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -28(%rbp)
	mov	$1, %edx
	mov	-28(%rbp), %eax
	add	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -28(%rbp)
	movq	-8(%rbp), %rdx
	movq	0(%rdx), %rbx
	movq	%rbx, -36(%rbp)
	movl	$0, %ebx
	movl	%ebx, -40(%rbp)
	jmp	.LtoUpper17
.LtoUpper16:
	lea	-8(%rbp), %rax
	pushq	%rdi
	movq	(%rax), %rax
	movq	%rax, %rdi
	pushq	%rsi
	movl	-40(%rbp), %eax
	movl	%eax, %esi
	call	pub_String_at
	popq	%rsi
	popq	%rdi
	movb	%al, %bl
	movb	%bl, -41(%rbp)
	pushq	%rdi
	movb	-41(%rbp), %al
	movb	%al, %dil
	call	upper
	popq	%rdi
	movb	%al, %bl
	movb	%bl, -41(%rbp)
	movq	-16(%rbp), %rax
	movb	-41(%rbp), %bl
	movb	%bl, (%rax)
	mov	$1, %edx
	mov	-16(%rbp), %rax
	add	%rdx, %rax
	movl	%eax, %ebx
	movl	%ebx, -16(%rbp)
	mov	$1, %edx
	mov	-40(%rbp), %eax
	add	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -40(%rbp)
.LtoUpper17:
	movl	-40(%rbp), %eax
	movl	-28(%rbp), %ecx
	cmpl	%ecx, %eax
	jl	.LtoUpper16
	pushq	%rdi
	movl	$8, %eax
	movl	%eax, %edi
	call	malloc
	popq	%rdi
	pushq	%rdi
	movq	%rax, %rdi
	pushq	%rsi
	movq	-24(%rbp), %rax
	movq	%rax, %rsi
	call	pub_String_init
	popq	%rsi
	popq	%rdi
	movq	%rax, %rbx
	movq	%rbx, -48(%rbp)
	pushq	%rdi
	movq	-24(%rbp), %rax
	movq	%rax, %rdi
	call	free
	popq	%rdi
	movq	-48(%rbp), %rax
	leave
	ret
	leave
	ret
pub_String_len:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$16, %rsp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rdx
	pushq	%rdi
	movq	0(%rdx), %rax
	movq	%rax, %rdi
	call	len
	popq	%rdi
	movl	%eax, %eax
	leave
	ret
	leave
	ret
lower:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$32, %rsp
	movb	%dil, -1(%rbp)
	lea	-5(%rbp), %rax
	movq	%rax, %rbx
	movq	%rbx, -13(%rbp)
	movq	-13(%rbp), %rax
	movl	$0, %ebx
	movl	%ebx, (%rax)
	movb	-1(%rbp), %al
	movb	$64, %cl
	cmpb	%cl, %al
	jle	.Llower18
	movb	-1(%rbp), %al
	movb	$91, %cl
	cmpb	%cl, %al
	jge	.Llower21
	mov	$32, %edx
	mov	-1(%rbp), %al
	add	%dl, %al
	movl	%eax, %ebx
	movl	%ebx, -17(%rbp)
	movq	-13(%rbp), %rax
	movl	-17(%rbp), %ebx
	movl	%ebx, (%rax)
	movq	-13(%rbp), %rax
	movb	(%rax), %al
	movb	%al, %bl
	movb	%bl, -1(%rbp)
.Llower21:
.Llower18:
	movb	-1(%rbp), %al
	leave
	ret
	leave
	ret
upper:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$32, %rsp
	movb	%dil, -1(%rbp)
	lea	-5(%rbp), %rax
	movq	%rax, %rbx
	movq	%rbx, -13(%rbp)
	movq	-13(%rbp), %rax
	movl	$0, %ebx
	movl	%ebx, (%rax)
	movb	-1(%rbp), %al
	movb	$96, %cl
	cmpb	%cl, %al
	jle	.Lupper24
	movb	-1(%rbp), %al
	movb	$123, %cl
	cmpb	%cl, %al
	jge	.Lupper27
	mov	$32, %edx
	mov	-1(%rbp), %al
	sub	%dl, %al
	movl	%eax, %ebx
	movl	%ebx, -17(%rbp)
	movq	-13(%rbp), %rax
	movl	-17(%rbp), %ebx
	movl	%ebx, (%rax)
	movq	-13(%rbp), %rax
	movb	(%rax), %al
	movb	%al, %bl
	movb	%bl, -1(%rbp)
.Lupper27:
.Lupper24:
	movb	-1(%rbp), %al
	leave
	ret
	leave
	ret


.data



.bss

