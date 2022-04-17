.global	pub_String_len
.global	pub_String_toInt
.global	pub_String_split
.global	pub_String_toUpper
.global	pub_String_toLower
.global	pub_String_replace
.global	pub_String_trim
.global	pub_String_copy
.global	String.getString
.global	String.printString
.global	pub_String_loop
.global	pub_String_del
.global	pub_String_indexOf
.global	pub_String_at
.global	pub_String_getPrimitive
.global	pub_String_equal
.global	pub_String_compare
.global	pub_String_subString
.global	pub_String_stitch
.global	pub_String_concat
.global	pub_String_init
.global	pub_String_isNumeric


.text

pub_ICollection_init:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$16, %rsp
	movq	%rdi, -8(%rbp)
	pushq	%r14
	movq	-8(%rbp), %r14
	movq	$lambda_1650179953112_0, %rbx
	movq	%rbx, 0(%r14)
	popq	%r14
	pushq	%r14
	movq	-8(%rbp), %r14
	movq	$lambda_1650179953112_2, %rbx
	movq	%rbx, 8(%r14)
	popq	%r14
	movq	-8(%rbp), %r15
	movq	%r15, %rax
	leave
	ret
pub_String_isNumeric:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$16, %rsp
	movq	%rdi, -8(%rbp)
	movb	$1, %bl
	movb	%bl, -9(%rbp)
	pushq	%rdx
	lea	-8(%rbp), %rax
	pushq	%rdi
	movq	(%rax), %rax
	movq	%rax, %rdi
	pushq	%rsi
	movq	$lambda_1650179953123_4, %rax
	movq	%rax, %rsi
	lea	-9(%rbp), %rax
	pushq	%rdx
	movq	%rax, %rax
	movq	%rax, %rdx
	call	pub_String_loop
	popq	%rdx
	popq	%rsi
	popq	%rdi
	popq	%rdx
	movb	-9(%rbp), %r15b
	movb	%r15b, %al
	leave
	ret
pub_String_init:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$32, %rsp
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	pushq	%rdx
	movq	-16(%rbp), %r15
	pushq	%rdi
	movq	%r15, %rax
	movq	%rax, %rdi
	call	strings.len
	popq	%rdi
	popq	%rdx
	movl	%eax, %ebx
	movl	%ebx, -20(%rbp)
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
	pushq	%r14
	movq	-8(%rbp), %r14
	pushq	%rdx
	movl	-20(%rbp), %r15d
	pushq	%rdi
	movl	%r15d, %eax
	movl	%eax, %edi
	call	malloc
	popq	%rdi
	popq	%rdx
	movq	%rax, %rbx
	movq	%rbx, 0(%r14)
	popq	%r14
	pushq	%r14
	movq	-8(%rbp), %r14
	movq	0(%r14), %r15
	popq	%r14
	movq	%r15, %rbx
	movq	%rbx, -28(%rbp)
	pushq	%rdx
	movq	-16(%rbp), %r15
	pushq	%rdi
	movq	%r15, %rax
	movq	%rax, %rdi
	movq	-28(%rbp), %r15
	pushq	%rsi
	movq	%r15, %rax
	movq	%rax, %rsi
	movl	-20(%rbp), %r15d
	pushq	%rdx
	movl	%r15d, %eax
	movl	%eax, %edx
	call	memcopy
	popq	%rdx
	popq	%rsi
	popq	%rdi
	popq	%rdx
	popq	%rbx
	movq	-8(%rbp), %rax
	leave
	ret
pub_String_concat:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$64, %rsp
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	pushq	%rdx
	pushq	%r14
	movq	-8(%rbp), %r14
	movq	0(%r14), %r15
	popq	%r14
	pushq	%rdi
	movq	%r15, %rax
	movq	%rax, %rdi
	call	strings.len
	popq	%rdi
	popq	%rdx
	movl	%eax, %ebx
	movl	%ebx, -20(%rbp)
	pushq	%rdx
	pushq	%r14
	movq	-16(%rbp), %r14
	movq	0(%r14), %r15
	popq	%r14
	pushq	%rdi
	movq	%r15, %rax
	movq	%rax, %rdi
	call	strings.len
	popq	%rdi
	popq	%rdx
	movl	%eax, %ebx
	movl	%ebx, -24(%rbp)
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
	pushq	%rdi
	pushq	%rdx
	movl	-24(%rbp), %r15d
	movl	%r15d, %edx
	movl	-20(%rbp), %r15d
	movl	%r15d, %edi
	add	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -28(%rbp)
	pushq	%rdx
	movl	-28(%rbp), %r15d
	pushq	%rdi
	movl	%r15d, %eax
	movl	%eax, %edi
	call	malloc
	popq	%rdi
	popq	%rdx
	movq	%rax, %rbx
	movq	%rbx, -36(%rbp)
	movq	-36(%rbp), %r15
	movq	%r15, %rbx
	movq	%rbx, -44(%rbp)
	pushq	%rdx
	pushq	%r14
	movq	-8(%rbp), %r14
	movq	0(%r14), %r15
	popq	%r14
	pushq	%rdi
	movq	%r15, %rax
	movq	%rax, %rdi
	movq	-36(%rbp), %r15
	pushq	%rsi
	movq	%r15, %rax
	movq	%rax, %rsi
	movl	-20(%rbp), %r15d
	pushq	%rdx
	movl	%r15d, %eax
	movl	%eax, %edx
	call	memcopy
	popq	%rdx
	popq	%rsi
	popq	%rdi
	popq	%rdx
	pushq	%rdi
	pushq	%rdx
	movl	-20(%rbp), %r15d
	movl	%r15d, %edx
	movq	-36(%rbp), %r15
	movq	%r15, %rdi
	add	%rdx, %rdi
	movq	%rdi, %rax
	popq	%rdx
	popq	%rdi
	movq	%rax, %rbx
	movq	%rbx, -36(%rbp)
	pushq	%rdx
	pushq	%r14
	movq	-16(%rbp), %r14
	movq	0(%r14), %r15
	popq	%r14
	pushq	%rdi
	movq	%r15, %rax
	movq	%rax, %rdi
	movq	-36(%rbp), %r15
	pushq	%rsi
	movq	%r15, %rax
	movq	%rax, %rsi
	movl	-24(%rbp), %r15d
	pushq	%rdx
	movl	%r15d, %eax
	movl	%eax, %edx
	call	memcopy
	popq	%rdx
	popq	%rsi
	popq	%rdi
	popq	%rdx
	pushq	%rdx
	pushq	%rdi
	movl	$8, %eax
	movl	%eax, %edi
	call	malloc
	popq	%rdi
	popq	%rdx
	pushq	%rdi
	movq	%rax, %rdi
	pushq	%rdx
	movq	-44(%rbp), %r15
	pushq	%rsi
	movq	%r15, %rax
	movq	%rax, %rsi
	call	pub_String_init
	popq	%rsi
	popq	%rdi
	popq	%rdx
	movq	%rax, %rbx
	movq	%rbx, -52(%rbp)
	pushq	%rdx
	movq	-44(%rbp), %r15
	pushq	%rdi
	movq	%r15, %rax
	movq	%rax, %rdi
	call	free
	popq	%rdi
	popq	%rdx
	movq	-52(%rbp), %r15
	movq	%r15, %rax
	leave
	ret
pub_String_stitch:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$64, %rsp
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	pushq	%rdx
	pushq	%r14
	movq	-8(%rbp), %r14
	movq	0(%r14), %r15
	popq	%r14
	pushq	%rdi
	movq	%r15, %rax
	movq	%rax, %rdi
	call	strings.len
	popq	%rdi
	popq	%rdx
	movl	%eax, %ebx
	movl	%ebx, -20(%rbp)
	pushq	%rdx
	movq	-16(%rbp), %r15
	pushq	%rdi
	movq	%r15, %rax
	movq	%rax, %rdi
	call	strings.len
	popq	%rdi
	popq	%rdx
	movl	%eax, %ebx
	movl	%ebx, -24(%rbp)
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
	pushq	%rdi
	pushq	%rdx
	movl	-24(%rbp), %r15d
	movl	%r15d, %edx
	movl	-20(%rbp), %r15d
	movl	%r15d, %edi
	add	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -28(%rbp)
	pushq	%rdx
	movl	-28(%rbp), %r15d
	pushq	%rdi
	movl	%r15d, %eax
	movl	%eax, %edi
	call	malloc
	popq	%rdi
	popq	%rdx
	movq	%rax, %rbx
	movq	%rbx, -36(%rbp)
	movq	-36(%rbp), %r15
	movq	%r15, %rbx
	movq	%rbx, -44(%rbp)
	pushq	%rdx
	pushq	%r14
	movq	-8(%rbp), %r14
	movq	0(%r14), %r15
	popq	%r14
	pushq	%rdi
	movq	%r15, %rax
	movq	%rax, %rdi
	movq	-36(%rbp), %r15
	pushq	%rsi
	movq	%r15, %rax
	movq	%rax, %rsi
	movl	-20(%rbp), %r15d
	pushq	%rdx
	movl	%r15d, %eax
	movl	%eax, %edx
	call	memcopy
	popq	%rdx
	popq	%rsi
	popq	%rdi
	popq	%rdx
	pushq	%rdi
	pushq	%rdx
	movl	-20(%rbp), %r15d
	movl	%r15d, %edx
	movq	-36(%rbp), %r15
	movq	%r15, %rdi
	add	%rdx, %rdi
	movq	%rdi, %rax
	popq	%rdx
	popq	%rdi
	movq	%rax, %rbx
	movq	%rbx, -36(%rbp)
	pushq	%rdx
	movq	-16(%rbp), %r15
	pushq	%rdi
	movq	%r15, %rax
	movq	%rax, %rdi
	movq	-36(%rbp), %r15
	pushq	%rsi
	movq	%r15, %rax
	movq	%rax, %rsi
	movl	-24(%rbp), %r15d
	pushq	%rdx
	movl	%r15d, %eax
	movl	%eax, %edx
	call	memcopy
	popq	%rdx
	popq	%rsi
	popq	%rdi
	popq	%rdx
	pushq	%rdx
	pushq	%rdi
	movl	$8, %eax
	movl	%eax, %edi
	call	malloc
	popq	%rdi
	popq	%rdx
	pushq	%rdi
	movq	%rax, %rdi
	pushq	%rdx
	movq	-44(%rbp), %r15
	pushq	%rsi
	movq	%r15, %rax
	movq	%rax, %rsi
	call	pub_String_init
	popq	%rsi
	popq	%rdi
	popq	%rdx
	movq	%rax, %rbx
	movq	%rbx, -52(%rbp)
	pushq	%rdx
	movq	-44(%rbp), %r15
	pushq	%rdi
	movq	%r15, %rax
	movq	%rax, %rdi
	call	free
	popq	%rdi
	popq	%rdx
	movq	-52(%rbp), %r15
	movq	%r15, %rax
	leave
	ret
pub_String_subString:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$64, %rsp
	movq	%rdi, -8(%rbp)
	movl	%esi, -12(%rbp)
	movl	%edx, -16(%rbp)
	pushq	%r14
	movq	-8(%rbp), %r14
	movq	0(%r14), %r15
	popq	%r14
	movq	%r15, %rbx
	movq	%rbx, -24(%rbp)
	pushq	%rdx
	movq	-24(%rbp), %r15
	pushq	%rdi
	movq	%r15, %rax
	movq	%rax, %rdi
	call	strings.len
	popq	%rdi
	popq	%rdx
	movl	%eax, %ebx
	movl	%ebx, -28(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	-12(%rbp), %r15d
	movl	%r15d, %edx
	movl	-16(%rbp), %r15d
	movl	%r15d, %edi
	sub	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -32(%rbp)
	movl	-32(%rbp), %r15d
	movl	%r15d, %ebx
	movl	%ebx, -32(%rbp)
	pushq	%rdx
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movl	-32(%rbp), %r15d
	movl	%r15d, %edi
	add	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	pushq	%rdi
	movl	%eax, %eax
	movl	%eax, %edi
	call	malloc
	popq	%rdi
	popq	%rdx
	movq	%rax, %rbx
	movq	%rbx, -40(%rbp)
	movq	-40(%rbp), %r15
	movq	%r15, %rbx
	movq	%rbx, -48(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	-12(%rbp), %r15d
	movl	%r15d, %edx
	movq	-24(%rbp), %r15
	movq	%r15, %rdi
	add	%rdx, %rdi
	movq	%rdi, %rax
	popq	%rdx
	popq	%rdi
	movq	%rax, %rbx
	movq	%rbx, -24(%rbp)
	pushq	%rdx
	movq	-24(%rbp), %r15
	pushq	%rdi
	movq	%r15, %rax
	movq	%rax, %rdi
	movq	-40(%rbp), %r15
	pushq	%rsi
	movq	%r15, %rax
	movq	%rax, %rsi
	movl	-32(%rbp), %r15d
	pushq	%rdx
	movl	%r15d, %eax
	movl	%eax, %edx
	call	memcopy
	popq	%rdx
	popq	%rsi
	popq	%rdi
	popq	%rdx
	pushq	%rdi
	pushq	%rdx
	movl	-32(%rbp), %r15d
	movl	%r15d, %edx
	movq	-40(%rbp), %r15
	movq	%r15, %rdi
	add	%rdx, %rdi
	movq	%rdi, %rax
	popq	%rdx
	popq	%rdi
	movq	%rax, %rbx
	movq	%rbx, -40(%rbp)
	movq	-40(%rbp), %rax
	movb	$0, %bl
	movb	%bl, (%rax)
	pushq	%rdx
	pushq	%rdi
	movl	$8, %eax
	movl	%eax, %edi
	call	malloc
	popq	%rdi
	popq	%rdx
	pushq	%rdi
	movq	%rax, %rdi
	pushq	%rdx
	movq	-48(%rbp), %r15
	pushq	%rsi
	movq	%r15, %rax
	movq	%rax, %rsi
	call	pub_String_init
	popq	%rsi
	popq	%rdi
	popq	%rdx
	movq	%rax, %rbx
	movq	%rbx, -56(%rbp)
	pushq	%rdx
	movq	-48(%rbp), %r15
	pushq	%rdi
	movq	%r15, %rax
	movq	%rax, %rdi
	call	free
	popq	%rdi
	popq	%rdx
	movq	-56(%rbp), %r15
	movq	%r15, %rax
	leave
	ret
pub_String_compare:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$48, %rsp
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	pushq	%r14
	movq	-16(%rbp), %r14
	movq	0(%r14), %r15
	popq	%r14
	movq	%r15, %rbx
	movq	%rbx, -24(%rbp)
	pushq	%r14
	movq	-8(%rbp), %r14
	movq	0(%r14), %r15
	popq	%r14
	movq	%r15, %rbx
	movq	%rbx, -32(%rbp)
	pushq	%rdx
	movq	-24(%rbp), %r15
	pushq	%rdi
	movq	%r15, %rax
	movq	%rax, %rdi
	movq	-32(%rbp), %r15
	pushq	%rsi
	movq	%r15, %rax
	movq	%rax, %rsi
	call	strings.str_comp
	popq	%rsi
	popq	%rdi
	popq	%rdx
	movl	%eax, %ebx
	movl	%ebx, -36(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movl	-36(%rbp), %r15d
	movl	%r15d, %edi
	cmpl	%edx, %edi
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	leave
	ret
pub_String_equal:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$32, %rsp
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	pushq	%rdx
	pushq	%r14
	movq	-8(%rbp), %r14
	movq	0(%r14), %r15
	popq	%r14
	pushq	%rdi
	movq	%r15, %rax
	movq	%rax, %rdi
	movq	-16(%rbp), %r15
	pushq	%rsi
	movq	%r15, %rax
	movq	%rax, %rsi
	call	strings.str_comp
	popq	%rsi
	popq	%rdi
	popq	%rdx
	movl	%eax, %ebx
	movl	%ebx, -20(%rbp)
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
	leave
	ret
pub_String_getPrimitive:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$16, %rsp
	movq	%rdi, -8(%rbp)
	pushq	%r14
	movq	-8(%rbp), %r14
	movq	0(%r14), %r15
	popq	%r14
	movq	%r15, %rax
	leave
	ret
pub_String_at:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$16, %rsp
	movq	%rdi, -8(%rbp)
	movl	%esi, -12(%rbp)
	pushq	%rdx
	pushq	%r14
	movq	-8(%rbp), %r14
	movq	0(%r14), %r15
	popq	%r14
	pushq	%rdi
	movq	%r15, %rax
	movq	%rax, %rdi
	movl	-12(%rbp), %r15d
	pushq	%rsi
	movl	%r15d, %eax
	movl	%eax, %esi
	call	strings.str_at
	popq	%rsi
	popq	%rdi
	popq	%rdx
	movb	%al, %bl
	movb	%bl, -13(%rbp)
	movb	-13(%rbp), %r15b
	movb	%r15b, %al
	leave
	ret
pub_String_indexOf:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$16, %rsp
	movq	%rdi, -8(%rbp)
	movb	%sil, -9(%rbp)
	pushq	%rdx
	pushq	%r14
	movq	-8(%rbp), %r14
	movq	0(%r14), %r15
	popq	%r14
	pushq	%rdi
	movq	%r15, %rax
	movq	%rax, %rdi
	movb	-9(%rbp), %r15b
	pushq	%rsi
	movb	%r15b, %al
	movb	%al, %sil
	call	strings.str_find
	popq	%rsi
	popq	%rdi
	popq	%rdx
	movl	%eax, %ebx
	movl	%ebx, -13(%rbp)
	movl	-13(%rbp), %r15d
	movl	%r15d, %eax
	leave
	ret
pub_String_del:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$16, %rsp
	movq	%rdi, -8(%rbp)
	pushq	%r14
	movq	-8(%rbp), %r14
	movq	0(%r14), %r15
	popq	%r14
	movq	%r15, %rbx
	movq	%rbx, -16(%rbp)
	pushq	%rdx
	movq	-16(%rbp), %r15
	pushq	%rdi
	movq	%r15, %rax
	movq	%rax, %rdi
	call	free
	popq	%rdi
	popq	%rdx
	leave
	ret
pub_String_loop:
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
	pushq	%rdx
	movq	-32(%rbp), %r15
	pushq	%rdi
	movq	%r15, %rax
	movq	%rax, %rdi
	call	strings.len
	popq	%rdi
	popq	%rdx
	movl	%eax, %ebx
	movl	%ebx, -36(%rbp)
	movl	$0, %ebx
	movl	%ebx, -40(%rbp)
	jmp	.Lloop7
.Lloop6:
	pushq	%rdx
	movq	-32(%rbp), %r15
	pushq	%rdi
	movq	%r15, %rax
	movq	%rax, %rdi
	movl	-40(%rbp), %r15d
	pushq	%rsi
	movl	%r15d, %eax
	movl	%eax, %esi
	call	strings.str_at
	popq	%rsi
	popq	%rdi
	popq	%rdx
	movb	%al, %bl
	movb	%bl, -41(%rbp)
	pushq	%rdx
	movq	-16(%rbp), %r15
	movq	%r15, %r15
	movb	-41(%rbp), %r15b
	pushq	%rdi
	movb	%r15b, %al
	movb	%al, %dil
	movl	-40(%rbp), %r15d
	pushq	%rsi
	movl	%r15d, %eax
	movl	%eax, %esi
	movq	-24(%rbp), %r15
	pushq	%rdx
	movq	%r15, %rax
	movq	%rax, %rdx
	call	*%r15
	popq	%rdx
	popq	%rsi
	popq	%rdi
	popq	%rdx
	movl	%eax, %ebx
	movl	%ebx, -45(%rbp)
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
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movl	-45(%rbp), %r15d
	movl	%r15d, %edi
	cmpl	%edx, %edi
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.Lloop8
	movl	$0, %eax
	leave
	ret
.Lloop8:
.Lloop7:
	pushq	%rdi
	pushq	%rdx
	movl	-36(%rbp), %r15d
	movl	%r15d, %edx
	movl	-40(%rbp), %r15d
	movl	%r15d, %edi
	cmpl	%edx, %edi
	setl	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$1, %al
	je	.Lloop6
	movl	$0, %eax
	leave
	ret
String.printString:
printString:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$16, %rsp
	movq	%rdi, -8(%rbp)
	pushq	%rdx
	lea	-8(%rbp), %rax
	pushq	%rdi
	movq	(%rax), %rax
	movq	%rax, %rdi
	call	pub_String_getPrimitive
	popq	%rdi
	popq	%rdx
	movq	%rax, %rbx
	movq	%rbx, -16(%rbp)
	pushq	%rdx
	movq	-16(%rbp), %r15
	pushq	%rdi
	movq	%r15, %rax
	movq	%rax, %rdi
	movq	$0, %rsi
	call	io.print
	popq	%rdi
	popq	%rdx
	movq	-8(%rbp), %r15
	movq	%r15, %rax
	leave
	ret
String.getString:
getString:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$48, %rsp
	pushq	%rdx
	pushq	%rdi
	movl	$1, %eax
	movl	%eax, %edi
	call	malloc
	popq	%rdi
	popq	%rdx
	movq	%rax, %rbx
	movq	%rbx, -8(%rbp)
	movq	-8(%rbp), %r15
	movq	%r15, %rbx
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
	jmp	.LgetString10
.LgetString9:
	pushq	%rdx
	pushq	%rdi
	movl	$0, %eax
	movl	%eax, %edi
	lea	-29(%rbp), %rax
	pushq	%rsi
	movq	%rax, %rax
	movq	%rax, %rsi
	pushq	%rdx
	movl	$1, %eax
	movl	%eax, %edx
	call	sys_read
	popq	%rdx
	popq	%rsi
	popq	%rdi
	popq	%rdx
	pushq	%rdi
	pushq	%rdx
	movl	-35(%rbp), %r15d
	movl	%r15d, %edx
	movq	-8(%rbp), %r15
	movq	%r15, %rdi
	add	%rdx, %rdi
	movq	%rdi, %rax
	popq	%rdx
	popq	%rdi
	movq	%rax, %rbx
	movq	%rbx, -8(%rbp)
	movb	-29(%rbp), %r15b
	movq	-8(%rbp), %rax
	movb	%r15b, %bl
	movb	%bl, (%rax)
	pushq	%rdi
	pushq	%rdx
	movb	-30(%rbp), %r15b
	movb	%r15b, %dl
	movb	-29(%rbp), %r15b
	movb	%r15b, %dil
	cmpb	%dl, %dil
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.LgetString11
	movb	-31(%rbp), %r15b
	movq	-8(%rbp), %rax
	movb	%r15b, %bl
	movb	%bl, (%rax)
.LgetString11:
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
	pushq	%rdx
	movq	-16(%rbp), %r15
	pushq	%rdi
	movq	%r15, %rax
	movq	%rax, %rdi
	movl	-20(%rbp), %r15d
	pushq	%rsi
	movl	%r15d, %eax
	movl	%eax, %esi
	call	realloc
	popq	%rsi
	popq	%rdi
	popq	%rdx
	movq	%rax, %rbx
	movq	%rbx, -16(%rbp)
	movq	-16(%rbp), %r15
	movq	%r15, %rbx
	movq	%rbx, -8(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movl	-35(%rbp), %r15d
	movl	%r15d, %edi
	add	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -35(%rbp)
.LgetString10:
	pushq	%rdi
	pushq	%rdx
	movb	-30(%rbp), %r15b
	movb	%r15b, %dl
	movb	-29(%rbp), %r15b
	movb	%r15b, %dil
	cmpb	%dl, %dil
	setne	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$1, %al
	je	.LgetString9
	pushq	%rdx
	pushq	%rdi
	movl	$8, %eax
	movl	%eax, %edi
	call	malloc
	popq	%rdi
	popq	%rdx
	pushq	%rdi
	movq	%rax, %rdi
	pushq	%rdx
	movq	-16(%rbp), %r15
	pushq	%rsi
	movq	%r15, %rax
	movq	%rax, %rsi
	call	pub_String_init
	popq	%rsi
	popq	%rdi
	popq	%rdx
	movq	%rax, %rbx
	movq	%rbx, -43(%rbp)
	pushq	%rdx
	movq	-16(%rbp), %r15
	pushq	%rdi
	movq	%r15, %rax
	movq	%rax, %rdi
	call	free
	popq	%rdi
	popq	%rdx
	movq	-43(%rbp), %r15
	movq	%r15, %rax
	leave
	ret
pub_String_copy:
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
	call	pub_String_del
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
	movq	-16(%rbp), %r15
	movq	%r15, %rax
	leave
	ret
pub_String_trim:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$32, %rsp
	movq	%rdi, -8(%rbp)
	movb	%sil, -9(%rbp)
	pushq	%rdi
	pushq	%rdx
	movb	$0, %dl
	movb	-9(%rbp), %r15b
	movb	%r15b, %dil
	cmpb	%dl, %dil
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.Ltrim12
	movb	$32, %bl
	movb	%bl, -9(%rbp)
.Ltrim12:
	pushq	%rdx
	lea	-8(%rbp), %rax
	pushq	%rdi
	movq	(%rax), %rax
	movq	%rax, %rdi
	call	pub_String_len
	popq	%rdi
	popq	%rdx
	movl	%eax, %ebx
	movl	%ebx, -13(%rbp)
	movl	$0, %ebx
	movl	%ebx, -17(%rbp)
	pushq	%rdx
	lea	-8(%rbp), %rax
	pushq	%rdi
	movq	(%rax), %rax
	movq	%rax, %rdi
	movl	-17(%rbp), %r15d
	pushq	%rsi
	movl	%r15d, %eax
	movl	%eax, %esi
	call	pub_String_at
	popq	%rsi
	popq	%rdi
	popq	%rdx
	movb	%al, %bl
	movb	%bl, -18(%rbp)
	jmp	.Ltrim14
.Ltrim13:
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movl	-17(%rbp), %r15d
	movl	%r15d, %edi
	add	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -17(%rbp)
	pushq	%rdx
	lea	-8(%rbp), %rax
	pushq	%rdi
	movq	(%rax), %rax
	movq	%rax, %rdi
	movl	-17(%rbp), %r15d
	pushq	%rsi
	movl	%r15d, %eax
	movl	%eax, %esi
	call	pub_String_at
	popq	%rsi
	popq	%rdi
	popq	%rdx
	movb	%al, %bl
	movb	%bl, -18(%rbp)
.Ltrim14:
	pushq	%rdi
	pushq	%rdx
	movb	-9(%rbp), %r15b
	movb	%r15b, %dl
	movb	-18(%rbp), %r15b
	movb	%r15b, %dil
	cmpb	%dl, %dil
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$1, %al
	je	.Ltrim13
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movl	-13(%rbp), %r15d
	movl	%r15d, %edi
	sub	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -22(%rbp)
	pushq	%rdx
	lea	-8(%rbp), %rax
	pushq	%rdi
	movq	(%rax), %rax
	movq	%rax, %rdi
	movl	-22(%rbp), %r15d
	pushq	%rsi
	movl	%r15d, %eax
	movl	%eax, %esi
	call	pub_String_at
	popq	%rsi
	popq	%rdi
	popq	%rdx
	movb	%al, %bl
	movb	%bl, -18(%rbp)
	jmp	.Ltrim16
.Ltrim15:
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movl	-22(%rbp), %r15d
	movl	%r15d, %edi
	sub	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -22(%rbp)
	pushq	%rdx
	lea	-8(%rbp), %rax
	pushq	%rdi
	movq	(%rax), %rax
	movq	%rax, %rdi
	movl	-22(%rbp), %r15d
	pushq	%rsi
	movl	%r15d, %eax
	movl	%eax, %esi
	call	pub_String_at
	popq	%rsi
	popq	%rdi
	popq	%rdx
	movb	%al, %bl
	movb	%bl, -18(%rbp)
.Ltrim16:
	pushq	%rdi
	pushq	%rdx
	movb	-9(%rbp), %r15b
	movb	%r15b, %dl
	movb	-18(%rbp), %r15b
	movb	%r15b, %dil
	cmpb	%dl, %dil
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$1, %al
	je	.Ltrim15
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movl	-22(%rbp), %r15d
	movl	%r15d, %edi
	add	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -22(%rbp)
	pushq	%rdx
	lea	-8(%rbp), %rax
	pushq	%rdi
	movq	(%rax), %rax
	movq	%rax, %rdi
	movl	-17(%rbp), %r15d
	pushq	%rsi
	movl	%r15d, %eax
	movl	%eax, %esi
	movl	-22(%rbp), %r15d
	pushq	%rdx
	movl	%r15d, %eax
	movl	%eax, %edx
	call	pub_String_subString
	popq	%rdx
	popq	%rsi
	popq	%rdi
	popq	%rdx
	movq	%rax, %rbx
	movq	%rbx, -30(%rbp)
	movq	-30(%rbp), %r15
	movq	%r15, %rax
	leave
	ret
pub_String_replace:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$64, %rsp
	movq	%rdi, -8(%rbp)
	movb	%sil, -9(%rbp)
	movb	%dl, -10(%rbp)
	pushq	%rdx
	pushq	%rdx
	lea	-8(%rbp), %rax
	pushq	%rdi
	movq	(%rax), %rax
	movq	%rax, %rdi
	call	pub_String_len
	popq	%rdi
	popq	%rdx
	pushq	%rdi
	movl	%eax, %eax
	movl	%eax, %edi
	call	malloc
	popq	%rdi
	popq	%rdx
	movq	%rax, %rbx
	movq	%rbx, -18(%rbp)
	movq	-18(%rbp), %r15
	movq	%r15, %rbx
	movq	%rbx, -26(%rbp)
	pushq	%rdx
	lea	-8(%rbp), %rax
	pushq	%rdi
	movq	(%rax), %rax
	movq	%rax, %rdi
	call	pub_String_len
	popq	%rdi
	popq	%rdx
	movl	%eax, %ebx
	movl	%ebx, -30(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movl	-30(%rbp), %r15d
	movl	%r15d, %edi
	add	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -30(%rbp)
	pushq	%r14
	movq	-8(%rbp), %r14
	movq	0(%r14), %r15
	popq	%r14
	movq	%r15, %rbx
	movq	%rbx, -38(%rbp)
	pushq	%rdx
	movq	-38(%rbp), %r15
	pushq	%rdi
	movq	%r15, %rax
	movq	%rax, %rdi
	movq	-18(%rbp), %r15
	pushq	%rsi
	movq	%r15, %rax
	movq	%rax, %rsi
	movl	-30(%rbp), %r15d
	pushq	%rdx
	movl	%r15d, %eax
	movl	%eax, %edx
	call	memcopy
	popq	%rdx
	popq	%rsi
	popq	%rdi
	popq	%rdx
	movl	$0, %ebx
	movl	%ebx, -42(%rbp)
	jmp	.Lreplace18
.Lreplace17:
	pushq	%rdi
	pushq	%rdx
	movb	-9(%rbp), %r15b
	movb	%r15b, %dl
	pushq	%rdx
	lea	-8(%rbp), %rax
	pushq	%rdi
	movq	(%rax), %rax
	movq	%rax, %rdi
	movl	-42(%rbp), %r15d
	pushq	%rsi
	movl	%r15d, %eax
	movl	%eax, %esi
	call	pub_String_at
	popq	%rsi
	popq	%rdi
	popq	%rdx
	movb	%al, %dil
	cmpb	%dl, %dil
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.Lreplace19
	pushq	%rdi
	pushq	%rdx
	movl	-42(%rbp), %r15d
	movl	%r15d, %edx
	movq	-18(%rbp), %r15
	movq	%r15, %rdi
	add	%rdx, %rdi
	movq	%rdi, %rax
	popq	%rdx
	popq	%rdi
	movq	%rax, %rbx
	movq	%rbx, -18(%rbp)
	movb	-10(%rbp), %r15b
	movq	-18(%rbp), %rax
	movb	%r15b, %bl
	movb	%bl, (%rax)
.Lreplace19:
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movl	-42(%rbp), %r15d
	movl	%r15d, %edi
	add	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -42(%rbp)
.Lreplace18:
	pushq	%rdi
	pushq	%rdx
	movl	-30(%rbp), %r15d
	movl	%r15d, %edx
	movl	-42(%rbp), %r15d
	movl	%r15d, %edi
	cmpl	%edx, %edi
	setl	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$1, %al
	je	.Lreplace17
	pushq	%rdx
	pushq	%rdi
	movl	$8, %eax
	movl	%eax, %edi
	call	malloc
	popq	%rdi
	popq	%rdx
	pushq	%rdi
	movq	%rax, %rdi
	pushq	%rdx
	movq	-26(%rbp), %r15
	pushq	%rsi
	movq	%r15, %rax
	movq	%rax, %rsi
	call	pub_String_init
	popq	%rsi
	popq	%rdi
	popq	%rdx
	movq	%rax, %rbx
	movq	%rbx, -50(%rbp)
	pushq	%rdx
	movq	-26(%rbp), %r15
	pushq	%rdi
	movq	%r15, %rax
	movq	%rax, %rdi
	call	free
	popq	%rdi
	popq	%rdx
	movq	-50(%rbp), %r15
	movq	%r15, %rax
	leave
	ret
pub_String_toLower:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$48, %rsp
	movq	%rdi, -8(%rbp)
	pushq	%rdx
	pushq	%rdx
	lea	-8(%rbp), %rax
	pushq	%rdi
	movq	(%rax), %rax
	movq	%rax, %rdi
	call	pub_String_len
	popq	%rdi
	popq	%rdx
	pushq	%rdi
	movl	%eax, %eax
	movl	%eax, %edi
	call	malloc
	popq	%rdi
	popq	%rdx
	movq	%rax, %rbx
	movq	%rbx, -16(%rbp)
	movq	-16(%rbp), %r15
	movq	%r15, %rbx
	movq	%rbx, -24(%rbp)
	pushq	%rdx
	lea	-8(%rbp), %rax
	pushq	%rdi
	movq	(%rax), %rax
	movq	%rax, %rdi
	call	pub_String_len
	popq	%rdi
	popq	%rdx
	movl	%eax, %ebx
	movl	%ebx, -28(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movl	-28(%rbp), %r15d
	movl	%r15d, %edi
	add	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -28(%rbp)
	pushq	%r14
	movq	-8(%rbp), %r14
	movq	0(%r14), %r15
	popq	%r14
	movq	%r15, %rbx
	movq	%rbx, -36(%rbp)
	movl	$0, %ebx
	movl	%ebx, -40(%rbp)
	jmp	.LtoLower21
.LtoLower20:
	pushq	%rdx
	lea	-8(%rbp), %rax
	pushq	%rdi
	movq	(%rax), %rax
	movq	%rax, %rdi
	movl	-40(%rbp), %r15d
	pushq	%rsi
	movl	%r15d, %eax
	movl	%eax, %esi
	call	pub_String_at
	popq	%rsi
	popq	%rdi
	popq	%rdx
	movb	%al, %bl
	movb	%bl, -41(%rbp)
	pushq	%rdx
	movb	-41(%rbp), %r15b
	pushq	%rdi
	movb	%r15b, %al
	movb	%al, %dil
	call	lower
	popq	%rdi
	popq	%rdx
	movb	%al, %bl
	movb	%bl, -41(%rbp)
	movb	-41(%rbp), %r15b
	movq	-16(%rbp), %rax
	movb	%r15b, %bl
	movb	%bl, (%rax)
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
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
	movl	$1, %edx
	movl	-40(%rbp), %r15d
	movl	%r15d, %edi
	add	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -40(%rbp)
.LtoLower21:
	pushq	%rdi
	pushq	%rdx
	movl	-28(%rbp), %r15d
	movl	%r15d, %edx
	movl	-40(%rbp), %r15d
	movl	%r15d, %edi
	cmpl	%edx, %edi
	setl	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$1, %al
	je	.LtoLower20
	pushq	%rdx
	pushq	%rdi
	movl	$8, %eax
	movl	%eax, %edi
	call	malloc
	popq	%rdi
	popq	%rdx
	pushq	%rdi
	movq	%rax, %rdi
	pushq	%rdx
	movq	-24(%rbp), %r15
	pushq	%rsi
	movq	%r15, %rax
	movq	%rax, %rsi
	call	pub_String_init
	popq	%rsi
	popq	%rdi
	popq	%rdx
	movq	%rax, %rbx
	movq	%rbx, -48(%rbp)
	pushq	%rdx
	movq	-24(%rbp), %r15
	pushq	%rdi
	movq	%r15, %rax
	movq	%rax, %rdi
	call	free
	popq	%rdi
	popq	%rdx
	movq	-48(%rbp), %r15
	movq	%r15, %rax
	leave
	ret
pub_String_toUpper:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$48, %rsp
	movq	%rdi, -8(%rbp)
	pushq	%rdx
	pushq	%rdx
	lea	-8(%rbp), %rax
	pushq	%rdi
	movq	(%rax), %rax
	movq	%rax, %rdi
	call	pub_String_len
	popq	%rdi
	popq	%rdx
	pushq	%rdi
	movl	%eax, %eax
	movl	%eax, %edi
	call	malloc
	popq	%rdi
	popq	%rdx
	movq	%rax, %rbx
	movq	%rbx, -16(%rbp)
	movq	-16(%rbp), %r15
	movq	%r15, %rbx
	movq	%rbx, -24(%rbp)
	pushq	%rdx
	lea	-8(%rbp), %rax
	pushq	%rdi
	movq	(%rax), %rax
	movq	%rax, %rdi
	call	pub_String_len
	popq	%rdi
	popq	%rdx
	movl	%eax, %ebx
	movl	%ebx, -28(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movl	-28(%rbp), %r15d
	movl	%r15d, %edi
	add	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -28(%rbp)
	pushq	%r14
	movq	-8(%rbp), %r14
	movq	0(%r14), %r15
	popq	%r14
	movq	%r15, %rbx
	movq	%rbx, -36(%rbp)
	movl	$0, %ebx
	movl	%ebx, -40(%rbp)
	jmp	.LtoUpper23
.LtoUpper22:
	pushq	%rdx
	lea	-8(%rbp), %rax
	pushq	%rdi
	movq	(%rax), %rax
	movq	%rax, %rdi
	movl	-40(%rbp), %r15d
	pushq	%rsi
	movl	%r15d, %eax
	movl	%eax, %esi
	call	pub_String_at
	popq	%rsi
	popq	%rdi
	popq	%rdx
	movb	%al, %bl
	movb	%bl, -41(%rbp)
	pushq	%rdx
	movb	-41(%rbp), %r15b
	pushq	%rdi
	movb	%r15b, %al
	movb	%al, %dil
	call	upper
	popq	%rdi
	popq	%rdx
	movb	%al, %bl
	movb	%bl, -41(%rbp)
	movb	-41(%rbp), %r15b
	movq	-16(%rbp), %rax
	movb	%r15b, %bl
	movb	%bl, (%rax)
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
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
	movl	$1, %edx
	movl	-40(%rbp), %r15d
	movl	%r15d, %edi
	add	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -40(%rbp)
.LtoUpper23:
	pushq	%rdi
	pushq	%rdx
	movl	-28(%rbp), %r15d
	movl	%r15d, %edx
	movl	-40(%rbp), %r15d
	movl	%r15d, %edi
	cmpl	%edx, %edi
	setl	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$1, %al
	je	.LtoUpper22
	pushq	%rdx
	pushq	%rdi
	movl	$8, %eax
	movl	%eax, %edi
	call	malloc
	popq	%rdi
	popq	%rdx
	pushq	%rdi
	movq	%rax, %rdi
	pushq	%rdx
	movq	-24(%rbp), %r15
	pushq	%rsi
	movq	%r15, %rax
	movq	%rax, %rsi
	call	pub_String_init
	popq	%rsi
	popq	%rdi
	popq	%rdx
	movq	%rax, %rbx
	movq	%rbx, -48(%rbp)
	pushq	%rdx
	movq	-24(%rbp), %r15
	pushq	%rdi
	movq	%r15, %rax
	movq	%rax, %rdi
	call	free
	popq	%rdi
	popq	%rdx
	movq	-48(%rbp), %r15
	movq	%r15, %rax
	leave
	ret
pub_String_split:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$64, %rsp
	movq	%rdi, -8(%rbp)
	movb	%sil, -9(%rbp)
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
	movq	%rbx, -17(%rbp)
	pushq	%rdi
	pushq	%rdx
	movb	$0, %dl
	movb	-9(%rbp), %r15b
	movb	%r15b, %dil
	cmpb	%dl, %dil
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.Lsplit24
	movb	$32, %bl
	movb	%bl, -9(%rbp)
.Lsplit24:
	pushq	%r14
	movq	-8(%rbp), %r14
	movq	0(%r14), %r15
	popq	%r14
	movq	%r15, %rbx
	movq	%rbx, -25(%rbp)
	pushq	%rdx
	lea	-8(%rbp), %rax
	pushq	%rdi
	movq	(%rax), %rax
	movq	%rax, %rdi
	call	pub_String_len
	popq	%rdi
	popq	%rdx
	movl	%eax, %ebx
	movl	%ebx, -29(%rbp)
	movl	$0, %ebx
	movl	%ebx, -33(%rbp)
	movl	$0, %ebx
	movl	%ebx, -37(%rbp)
	jmp	.Lsplit26
.Lsplit25:
	pushq	%rdx
	lea	-8(%rbp), %rax
	pushq	%rdi
	movq	(%rax), %rax
	movq	%rax, %rdi
	movl	-37(%rbp), %r15d
	pushq	%rsi
	movl	%r15d, %eax
	movl	%eax, %esi
	call	pub_String_at
	popq	%rsi
	popq	%rdi
	popq	%rdx
	movb	%al, %bl
	movb	%bl, -38(%rbp)
	pushq	%rdi
	pushq	%rdx
	movb	-9(%rbp), %r15b
	movb	%r15b, %dl
	movb	-38(%rbp), %r15b
	movb	%r15b, %dil
	cmpb	%dl, %dil
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.Lsplit27
	movl	-37(%rbp), %r15d
	movl	%r15d, %ebx
	movl	%ebx, -42(%rbp)
	pushq	%rdx
	lea	-8(%rbp), %rax
	pushq	%rdi
	movq	(%rax), %rax
	movq	%rax, %rdi
	movl	-33(%rbp), %r15d
	pushq	%rsi
	movl	%r15d, %eax
	movl	%eax, %esi
	movl	-42(%rbp), %r15d
	pushq	%rdx
	movl	%r15d, %eax
	movl	%eax, %edx
	call	pub_String_subString
	popq	%rdx
	popq	%rsi
	popq	%rdi
	popq	%rdx
	movq	%rax, %rbx
	movq	%rbx, -50(%rbp)
	pushq	%rdx
	lea	-17(%rbp), %rax
	pushq	%rdi
	movq	(%rax), %rax
	movq	%rax, %rdi
	movq	-50(%rbp), %r15
	pushq	%rsi
	movq	%r15, %rax
	movq	%rax, %rsi
	call	pub_LinkedList_append
	popq	%rsi
	popq	%rdi
	popq	%rdx
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movl	-42(%rbp), %r15d
	movl	%r15d, %edi
	add	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -33(%rbp)
.Lsplit27:
	add	$1, -37(%rbp)
.Lsplit26:
	pushq	%rdi
	pushq	%rdx
	movl	-29(%rbp), %r15d
	movl	%r15d, %edx
	movl	-37(%rbp), %r15d
	movl	%r15d, %edi
	cmpl	%edx, %edi
	setl	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$1, %al
	je	.Lsplit25
	pushq	%rdx
	lea	-8(%rbp), %rax
	pushq	%rdi
	movq	(%rax), %rax
	movq	%rax, %rdi
	movl	-33(%rbp), %r15d
	pushq	%rsi
	movl	%r15d, %eax
	movl	%eax, %esi
	movl	-29(%rbp), %r15d
	pushq	%rdx
	movl	%r15d, %eax
	movl	%eax, %edx
	call	pub_String_subString
	popq	%rdx
	popq	%rsi
	popq	%rdi
	popq	%rdx
	movq	%rax, %rbx
	movq	%rbx, -41(%rbp)
	pushq	%rdx
	lea	-17(%rbp), %rax
	pushq	%rdi
	movq	(%rax), %rax
	movq	%rax, %rdi
	movq	-41(%rbp), %r15
	pushq	%rsi
	movq	%r15, %rax
	movq	%rax, %rsi
	call	pub_LinkedList_append
	popq	%rsi
	popq	%rdi
	popq	%rdx
	movq	-17(%rbp), %r15
	movq	%r15, %rax
	leave
	ret
pub_String_toInt:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$16, %rsp
	movq	%rdi, -8(%rbp)
	pushq	%rdi
	pushq	%rdx
	movb	$0, %dl
	pushq	%rdx
	lea	-8(%rbp), %rax
	pushq	%rdi
	movq	(%rax), %rax
	movq	%rax, %rdi
	call	pub_String_isNumeric
	popq	%rdi
	popq	%rdx
	movb	%al, %dil
	cmpb	%dl, %dil
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.LtoInt28
	pushq	%rdx
	pushq	%rdi
	movq	$.strString.lambda_1650179953123_429, %rax
	movq	%rax, %rdi
	call	panic
	popq	%rdi
	popq	%rdx
.LtoInt28:
	pushq	%rdx
	pushq	%r14
	movq	-8(%rbp), %r14
	movq	0(%r14), %r15
	popq	%r14
	pushq	%rdi
	movq	%r15, %rax
	movq	%rax, %rdi
	call	strings.str_toInt
	popq	%rdi
	popq	%rdx
	movl	%eax, %eax
	leave
	ret
pub_String_len:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$32, %rsp
	movq	%rdi, -8(%rbp)
	movl	$0, %ebx
	movl	%ebx, -12(%rbp)
	pushq	%r14
	movq	-8(%rbp), %r14
	movq	0(%r14), %r15
	popq	%r14
	movq	%r15, %rbx
	movq	%rbx, -20(%rbp)
	jmp	.Llen31
.Llen30:
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movq	-20(%rbp), %r15
	movq	%r15, %rdi
	add	%rdx, %rdi
	movq	%rdi, %rax
	popq	%rdx
	popq	%rdi
	movq	%rax, %rbx
	movq	%rbx, -20(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movl	-12(%rbp), %r15d
	movl	%r15d, %edi
	add	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -12(%rbp)
.Llen31:
	pushq	%rdi
	pushq	%rdx
	movb	$0, %dl
	movq	-20(%rbp), %rax
	movb	(%rax), %al
	movb	%al, %dil
	cmpb	%dl, %dil
	setne	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$1, %al
	je	.Llen30
	movl	-12(%rbp), %r15d
	movl	%r15d, %eax
	leave
	ret
lower:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$16, %rsp
	movb	%dil, -1(%rbp)
	pushq	%rdi
	pushq	%rdx
	movb	$64, %dl
	movb	-1(%rbp), %r15b
	movb	%r15b, %dil
	cmpb	%dl, %dil
	setg	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.Llower32
	pushq	%rdi
	pushq	%rdx
	movb	$91, %dl
	movb	-1(%rbp), %r15b
	movb	%r15b, %dil
	cmpb	%dl, %dil
	setl	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.Llower33
	pushq	%rdi
	pushq	%rdx
	movl	$32, %edx
	movb	-1(%rbp), %r15b
	movb	%r15b, %dil
	add	%dl, %dil
	movb	%dil, %al
	popq	%rdx
	popq	%rdi
	movb	%al, %bl
	movb	%bl, -1(%rbp)
.Llower33:
.Llower32:
	movb	-1(%rbp), %r15b
	movb	%r15b, %al
	leave
	ret
upper:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$16, %rsp
	movb	%dil, -1(%rbp)
	pushq	%rdi
	pushq	%rdx
	movb	$96, %dl
	movb	-1(%rbp), %r15b
	movb	%r15b, %dil
	cmpb	%dl, %dil
	setg	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.Lupper34
	pushq	%rdi
	pushq	%rdx
	movb	$123, %dl
	movb	-1(%rbp), %r15b
	movb	%r15b, %dil
	cmpb	%dl, %dil
	setl	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.Lupper35
	pushq	%rdi
	pushq	%rdx
	movl	$32, %edx
	movb	-1(%rbp), %r15b
	movb	%r15b, %dil
	sub	%dl, %dil
	movb	%dil, %al
	popq	%rdx
	popq	%rdi
	movb	%al, %bl
	movb	%bl, -1(%rbp)
.Lupper35:
.Lupper34:
	movb	-1(%rbp), %r15b
	movb	%r15b, %al
	leave
	ret
lambda_1650179953112_0:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$16, %rsp
	pushq	%rdx
	pushq	%rdi
	movq	$.strICollection.lambda_1650179953112_01, %rax
	movq	%rax, %rdi
	call	panic
	popq	%rdi
	popq	%rdx
	leave
	ret
lambda_1650179953112_2:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$16, %rsp
	pushq	%rdx
	pushq	%rdi
	movq	$.strICollection.lambda_1650179953112_23, %rax
	movq	%rax, %rdi
	call	panic
	popq	%rdi
	popq	%rdx
	leave
	ret
lambda_1650179953123_4:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$32, %rsp
	movb	%dil, -10(%rbp)
	movl	%esi, -14(%rbp)
	movq	%rdx, -22(%rbp)
	pushq	%rdi
	pushq	%rdx
	movb	$0, %dl
	pushq	%rdx
	movb	-10(%rbp), %r15b
	pushq	%rdi
	movb	%r15b, %al
	movb	%al, %dil
	call	strings.char_isDigit
	popq	%rdi
	popq	%rdx
	movb	%al, %dil
	cmpb	%dl, %dil
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.Llambda_1650179953123_45
	movq	-22(%rbp), %rax
	movb	$0, %bl
	movb	%bl, (%rax)
	movl	$1, %eax
	leave
	ret
.Llambda_1650179953123_45:
	movl	$0, %eax
	leave
	ret


.data

.strICollection.lambda_1650179953112_23:
	.asciz	 "Function (where) not implemented on this instance"
.strICollection.lambda_1650179953112_01:
	.asciz	 "Function (forEach) not implemented on this instance"
.strString.lambda_1650179953123_429:
	.asciz	 "Attempt to convert non-numeric string to int"


.bss

