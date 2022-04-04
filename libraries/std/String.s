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
.global	pub_String_delete
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
	movq	$lambda_1649090706959_0, %rax
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
	movb	-9(%rbp), %al
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
	pushq	%rdi
	movq	-16(%rbp), %rax
	movq	%rax, %rdi
	call	strings.len
	popq	%rdi
	popq	%rdx
	movl	%eax, %ebx
	movl	%ebx, -20(%rbp)
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
	pushq	%rdx
	pushq	%rdi
	movl	-20(%rbp), %eax
	movl	%eax, %edi
	call	malloc
	popq	%rdi
	popq	%rdx
	movq	-8(%rbp), %rdx
	movq	%rax, %rbx
	movq	%rbx, 0(%rdx)
	movq	-8(%rbp), %r14
	movq	0(%r14), %rbx
	movq	%rbx, -28(%rbp)
	pushq	%rdx
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
	movq	-8(%rbp), %r14
	pushq	%rdi
	movq	0(%r14), %rax
	movq	%rax, %rdi
	call	strings.len
	popq	%rdi
	popq	%rdx
	movl	%eax, %ebx
	movl	%ebx, -20(%rbp)
	pushq	%rdx
	movq	-16(%rbp), %r14
	pushq	%rdi
	movq	0(%r14), %rax
	movq	%rax, %rdi
	call	strings.len
	popq	%rdi
	popq	%rdx
	movl	%eax, %ebx
	movl	%ebx, -24(%rbp)
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
	pushq	%rdi
	pushq	%rdx
	movl	-24(%rbp), %edx
	movl	-20(%rbp), %edi
	add	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -28(%rbp)
	pushq	%rdx
	pushq	%rdi
	movl	-28(%rbp), %eax
	movl	%eax, %edi
	call	malloc
	popq	%rdi
	popq	%rdx
	movq	%rax, %rbx
	movq	%rbx, -36(%rbp)
	movq	-36(%rbp), %rbx
	movq	%rbx, -44(%rbp)
	pushq	%rdx
	movq	-8(%rbp), %r14
	pushq	%rdi
	movq	0(%r14), %rax
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
	popq	%rdx
	pushq	%rdi
	pushq	%rdx
	movl	-20(%rbp), %edx
	movq	-36(%rbp), %rdi
	add	%rdx, %rdi
	movq	%rdi, %rax
	popq	%rdx
	popq	%rdi
	movq	%rax, %rbx
	movq	%rbx, -36(%rbp)
	pushq	%rdx
	movq	-16(%rbp), %r14
	pushq	%rdi
	movq	0(%r14), %rax
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
	pushq	%rsi
	movq	-44(%rbp), %rax
	movq	%rax, %rsi
	call	pub_String_init
	popq	%rsi
	popq	%rdi
	popq	%rdx
	movq	%rax, %rbx
	movq	%rbx, -52(%rbp)
	pushq	%rdx
	pushq	%rdi
	movq	-44(%rbp), %rax
	movq	%rax, %rdi
	call	free
	popq	%rdi
	popq	%rdx
	movq	-52(%rbp), %rax
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
	movq	-8(%rbp), %r14
	pushq	%rdi
	movq	0(%r14), %rax
	movq	%rax, %rdi
	call	strings.len
	popq	%rdi
	popq	%rdx
	movl	%eax, %ebx
	movl	%ebx, -20(%rbp)
	pushq	%rdx
	pushq	%rdi
	movq	-16(%rbp), %rax
	movq	%rax, %rdi
	call	strings.len
	popq	%rdi
	popq	%rdx
	movl	%eax, %ebx
	movl	%ebx, -24(%rbp)
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
	pushq	%rdi
	pushq	%rdx
	movl	-24(%rbp), %edx
	movl	-20(%rbp), %edi
	add	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -28(%rbp)
	pushq	%rdx
	pushq	%rdi
	movl	-28(%rbp), %eax
	movl	%eax, %edi
	call	malloc
	popq	%rdi
	popq	%rdx
	movq	%rax, %rbx
	movq	%rbx, -36(%rbp)
	movq	-36(%rbp), %rbx
	movq	%rbx, -44(%rbp)
	pushq	%rdx
	movq	-8(%rbp), %r14
	pushq	%rdi
	movq	0(%r14), %rax
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
	popq	%rdx
	pushq	%rdi
	pushq	%rdx
	movl	-20(%rbp), %edx
	movq	-36(%rbp), %rdi
	add	%rdx, %rdi
	movq	%rdi, %rax
	popq	%rdx
	popq	%rdi
	movq	%rax, %rbx
	movq	%rbx, -36(%rbp)
	pushq	%rdx
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
	pushq	%rsi
	movq	-44(%rbp), %rax
	movq	%rax, %rsi
	call	pub_String_init
	popq	%rsi
	popq	%rdi
	popq	%rdx
	movq	%rax, %rbx
	movq	%rbx, -52(%rbp)
	pushq	%rdx
	pushq	%rdi
	movq	-44(%rbp), %rax
	movq	%rax, %rdi
	call	free
	popq	%rdi
	popq	%rdx
	movq	-52(%rbp), %rax
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
	movq	-8(%rbp), %r14
	movq	0(%r14), %rbx
	movq	%rbx, -24(%rbp)
	pushq	%rdx
	pushq	%rdi
	movq	-24(%rbp), %rax
	movq	%rax, %rdi
	call	strings.len
	popq	%rdi
	popq	%rdx
	movl	%eax, %ebx
	movl	%ebx, -28(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	-12(%rbp), %edx
	movl	-16(%rbp), %edi
	sub	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -32(%rbp)
	movl	-32(%rbp), %ebx
	movl	%ebx, -32(%rbp)
	pushq	%rdx
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movl	-32(%rbp), %edi
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
	movq	-40(%rbp), %rbx
	movq	%rbx, -48(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	-12(%rbp), %edx
	movq	-24(%rbp), %rdi
	add	%rdx, %rdi
	movq	%rdi, %rax
	popq	%rdx
	popq	%rdi
	movq	%rax, %rbx
	movq	%rbx, -24(%rbp)
	pushq	%rdx
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
	popq	%rdx
	pushq	%rdi
	pushq	%rdx
	movl	-32(%rbp), %edx
	movq	-40(%rbp), %rdi
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
	pushq	%rsi
	movq	-48(%rbp), %rax
	movq	%rax, %rsi
	call	pub_String_init
	popq	%rsi
	popq	%rdi
	popq	%rdx
	movq	%rax, %rbx
	movq	%rbx, -56(%rbp)
	pushq	%rdx
	pushq	%rdi
	movq	-48(%rbp), %rax
	movq	%rax, %rdi
	call	free
	popq	%rdi
	popq	%rdx
	movq	-56(%rbp), %rax
	leave
	ret
pub_String_compare:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$48, %rsp
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	movq	-16(%rbp), %r14
	movq	0(%r14), %rbx
	movq	%rbx, -24(%rbp)
	movq	-8(%rbp), %r14
	movq	0(%r14), %rbx
	movq	%rbx, -32(%rbp)
	pushq	%rdx
	pushq	%rdi
	movq	-24(%rbp), %rax
	movq	%rax, %rdi
	pushq	%rsi
	movq	-32(%rbp), %rax
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
	movl	-36(%rbp), %edi
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
	movq	-8(%rbp), %r14
	pushq	%rdi
	movq	0(%r14), %rax
	movq	%rax, %rdi
	pushq	%rsi
	movq	-16(%rbp), %rax
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
	movl	-20(%rbp), %edi
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
	movq	-8(%rbp), %r14
	movq	0(%r14), %rax
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
	movq	-8(%rbp), %r14
	pushq	%rdi
	movq	0(%r14), %rax
	movq	%rax, %rdi
	pushq	%rsi
	movl	-12(%rbp), %eax
	movl	%eax, %esi
	call	strings.str_at
	popq	%rsi
	popq	%rdi
	popq	%rdx
	movb	%al, %bl
	movb	%bl, -13(%rbp)
	movb	-13(%rbp), %al
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
	movq	-8(%rbp), %r14
	pushq	%rdi
	movq	0(%r14), %rax
	movq	%rax, %rdi
	pushq	%rsi
	movb	-9(%rbp), %al
	movb	%al, %sil
	call	strings.str_find
	popq	%rsi
	popq	%rdi
	popq	%rdx
	movl	%eax, %ebx
	movl	%ebx, -13(%rbp)
	movl	-13(%rbp), %eax
	leave
	ret
pub_String_delete:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$16, %rsp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %r14
	movq	0(%r14), %rbx
	movq	%rbx, -16(%rbp)
	pushq	%rdx
	pushq	%rdi
	movq	-16(%rbp), %rax
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
	movq	-8(%rbp), %r14
	movq	0(%r14), %rbx
	movq	%rbx, -32(%rbp)
	pushq	%rdx
	pushq	%rdi
	movq	-32(%rbp), %rax
	movq	%rax, %rdi
	call	strings.len
	popq	%rdi
	popq	%rdx
	movl	%eax, %ebx
	movl	%ebx, -36(%rbp)
	movl	$0, %ebx
	movl	%ebx, -40(%rbp)
	jmp	.Lloop3
.Lloop2:
	pushq	%rdx
	pushq	%rdi
	movq	-32(%rbp), %rax
	movq	%rax, %rdi
	pushq	%rsi
	movl	-40(%rbp), %eax
	movl	%eax, %esi
	call	strings.str_at
	popq	%rsi
	popq	%rdi
	popq	%rdx
	movb	%al, %bl
	movb	%bl, -41(%rbp)
	pushq	%rdx
	movq	-16(%rbp), %r15
	pushq	%rdi
	movb	-41(%rbp), %al
	movb	%al, %dil
	pushq	%rsi
	movl	-40(%rbp), %eax
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
	movl	%ebx, -45(%rbp)
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
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movl	-45(%rbp), %edi
	cmpl	%edx, %edi
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.Lloop4
	movl	$0, %eax
	leave
	ret
.Lloop4:
.Lloop3:
	pushq	%rdi
	pushq	%rdx
	movl	-36(%rbp), %edx
	movl	-40(%rbp), %edi
	cmpl	%edx, %edi
	setl	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$1, %al
	je	.Lloop2
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
	pushq	%rdi
	movq	-16(%rbp), %rax
	movq	%rax, %rdi
	movq	$0, %rsi
	call	io.print
	popq	%rdi
	popq	%rdx
	movq	-8(%rbp), %rax
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
	jmp	.LgetString6
.LgetString5:
	lea	-29(%rbp), %rax
	movq	$1, %rdi
	movq	%rax, %rsi
	movq	$1, %rdx
	movq	$0, %rax
	syscall	
	pushq	%rdi
	pushq	%rdx
	movl	-35(%rbp), %edx
	movq	-8(%rbp), %rdi
	add	%rdx, %rdi
	movq	%rdi, %rax
	popq	%rdx
	popq	%rdi
	movq	%rax, %rbx
	movq	%rbx, -8(%rbp)
	movq	-8(%rbp), %rax
	movb	-29(%rbp), %bl
	movb	%bl, (%rax)
	pushq	%rdi
	pushq	%rdx
	movb	-30(%rbp), %dl
	movb	-29(%rbp), %dil
	cmpb	%dl, %dil
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.LgetString7
	movq	-8(%rbp), %rax
	movb	-31(%rbp), %bl
	movb	%bl, (%rax)
.LgetString7:
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
	pushq	%rdx
	pushq	%rdi
	movq	-16(%rbp), %rax
	movq	%rax, %rdi
	pushq	%rsi
	movl	-20(%rbp), %eax
	movl	%eax, %esi
	call	realloc
	popq	%rsi
	popq	%rdi
	popq	%rdx
	movq	%rax, %rbx
	movq	%rbx, -16(%rbp)
	movq	-16(%rbp), %rbx
	movq	%rbx, -8(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movl	-35(%rbp), %edi
	add	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -35(%rbp)
.LgetString6:
	pushq	%rdi
	pushq	%rdx
	movb	-30(%rbp), %dl
	movb	-29(%rbp), %dil
	cmpb	%dl, %dil
	setne	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$1, %al
	je	.LgetString5
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
	pushq	%rsi
	movq	-16(%rbp), %rax
	movq	%rax, %rsi
	call	pub_String_init
	popq	%rsi
	popq	%rdi
	popq	%rdx
	movq	%rax, %rbx
	movq	%rbx, -43(%rbp)
	pushq	%rdx
	pushq	%rdi
	movq	-16(%rbp), %rax
	movq	%rax, %rdi
	call	free
	popq	%rdi
	popq	%rdx
	movq	-43(%rbp), %rax
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
	call	pub_String_delete
	popq	%rdi
	popq	%rdx
	movq	-16(%rbp), %rax
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
	movb	-9(%rbp), %dil
	cmpb	%dl, %dil
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.Ltrim8
	movb	$32, %bl
	movb	%bl, -9(%rbp)
.Ltrim8:
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
	pushq	%rsi
	movl	-17(%rbp), %eax
	movl	%eax, %esi
	call	pub_String_at
	popq	%rsi
	popq	%rdi
	popq	%rdx
	movb	%al, %bl
	movb	%bl, -18(%rbp)
	jmp	.Ltrim10
.Ltrim9:
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movl	-17(%rbp), %edi
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
	pushq	%rsi
	movl	-17(%rbp), %eax
	movl	%eax, %esi
	call	pub_String_at
	popq	%rsi
	popq	%rdi
	popq	%rdx
	movb	%al, %bl
	movb	%bl, -18(%rbp)
.Ltrim10:
	pushq	%rdi
	pushq	%rdx
	movb	-9(%rbp), %dl
	movb	-18(%rbp), %dil
	cmpb	%dl, %dil
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$1, %al
	je	.Ltrim9
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movl	-13(%rbp), %edi
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
	pushq	%rsi
	movl	-22(%rbp), %eax
	movl	%eax, %esi
	call	pub_String_at
	popq	%rsi
	popq	%rdi
	popq	%rdx
	movb	%al, %bl
	movb	%bl, -18(%rbp)
	jmp	.Ltrim12
.Ltrim11:
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movl	-22(%rbp), %edi
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
	pushq	%rsi
	movl	-22(%rbp), %eax
	movl	%eax, %esi
	call	pub_String_at
	popq	%rsi
	popq	%rdi
	popq	%rdx
	movb	%al, %bl
	movb	%bl, -18(%rbp)
.Ltrim12:
	pushq	%rdi
	pushq	%rdx
	movb	-9(%rbp), %dl
	movb	-18(%rbp), %dil
	cmpb	%dl, %dil
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$1, %al
	je	.Ltrim11
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movl	-22(%rbp), %edi
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
	popq	%rdx
	movq	%rax, %rbx
	movq	%rbx, -30(%rbp)
	movq	-30(%rbp), %rax
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
	movq	-18(%rbp), %rbx
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
	movl	-30(%rbp), %edi
	add	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -30(%rbp)
	movq	-8(%rbp), %r14
	movq	0(%r14), %rbx
	movq	%rbx, -38(%rbp)
	pushq	%rdx
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
	popq	%rdx
	movl	$0, %ebx
	movl	%ebx, -42(%rbp)
	jmp	.Lreplace14
.Lreplace13:
	pushq	%rdi
	pushq	%rdx
	movb	-9(%rbp), %dl
	pushq	%rdx
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
	popq	%rdx
	movb	%al, %dil
	cmpb	%dl, %dil
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.Lreplace15
	pushq	%rdi
	pushq	%rdx
	movl	-42(%rbp), %edx
	movq	-18(%rbp), %rdi
	add	%rdx, %rdi
	movq	%rdi, %rax
	popq	%rdx
	popq	%rdi
	movq	%rax, %rbx
	movq	%rbx, -18(%rbp)
	movq	-18(%rbp), %rax
	movb	-10(%rbp), %bl
	movb	%bl, (%rax)
.Lreplace15:
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movl	-42(%rbp), %edi
	add	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -42(%rbp)
.Lreplace14:
	pushq	%rdi
	pushq	%rdx
	movl	-30(%rbp), %edx
	movl	-42(%rbp), %edi
	cmpl	%edx, %edi
	setl	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$1, %al
	je	.Lreplace13
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
	pushq	%rsi
	movq	-26(%rbp), %rax
	movq	%rax, %rsi
	call	pub_String_init
	popq	%rsi
	popq	%rdi
	popq	%rdx
	movq	%rax, %rbx
	movq	%rbx, -50(%rbp)
	pushq	%rdx
	pushq	%rdi
	movq	-26(%rbp), %rax
	movq	%rax, %rdi
	call	free
	popq	%rdi
	popq	%rdx
	movq	-50(%rbp), %rax
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
	movq	-16(%rbp), %rbx
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
	movl	-28(%rbp), %edi
	add	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -28(%rbp)
	movq	-8(%rbp), %r14
	movq	0(%r14), %rbx
	movq	%rbx, -36(%rbp)
	movl	$0, %ebx
	movl	%ebx, -40(%rbp)
	jmp	.LtoLower17
.LtoLower16:
	pushq	%rdx
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
	popq	%rdx
	movb	%al, %bl
	movb	%bl, -41(%rbp)
	pushq	%rdx
	pushq	%rdi
	movb	-41(%rbp), %al
	movb	%al, %dil
	call	lower
	popq	%rdi
	popq	%rdx
	movb	%al, %bl
	movb	%bl, -41(%rbp)
	movq	-16(%rbp), %rax
	movb	-41(%rbp), %bl
	movb	%bl, (%rax)
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movq	-16(%rbp), %rdi
	add	%rdx, %rdi
	movq	%rdi, %rax
	popq	%rdx
	popq	%rdi
	movq	%rax, %rbx
	movq	%rbx, -16(%rbp)
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
.LtoLower17:
	pushq	%rdi
	pushq	%rdx
	movl	-28(%rbp), %edx
	movl	-40(%rbp), %edi
	cmpl	%edx, %edi
	setl	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$1, %al
	je	.LtoLower16
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
	pushq	%rsi
	movq	-24(%rbp), %rax
	movq	%rax, %rsi
	call	pub_String_init
	popq	%rsi
	popq	%rdi
	popq	%rdx
	movq	%rax, %rbx
	movq	%rbx, -48(%rbp)
	pushq	%rdx
	pushq	%rdi
	movq	-24(%rbp), %rax
	movq	%rax, %rdi
	call	free
	popq	%rdi
	popq	%rdx
	movq	-48(%rbp), %rax
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
	movq	-16(%rbp), %rbx
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
	movl	-28(%rbp), %edi
	add	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -28(%rbp)
	movq	-8(%rbp), %r14
	movq	0(%r14), %rbx
	movq	%rbx, -36(%rbp)
	movl	$0, %ebx
	movl	%ebx, -40(%rbp)
	jmp	.LtoUpper19
.LtoUpper18:
	pushq	%rdx
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
	popq	%rdx
	movb	%al, %bl
	movb	%bl, -41(%rbp)
	pushq	%rdx
	pushq	%rdi
	movb	-41(%rbp), %al
	movb	%al, %dil
	call	upper
	popq	%rdi
	popq	%rdx
	movb	%al, %bl
	movb	%bl, -41(%rbp)
	movq	-16(%rbp), %rax
	movb	-41(%rbp), %bl
	movb	%bl, (%rax)
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movq	-16(%rbp), %rdi
	add	%rdx, %rdi
	movq	%rdi, %rax
	popq	%rdx
	popq	%rdi
	movq	%rax, %rbx
	movq	%rbx, -16(%rbp)
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
.LtoUpper19:
	pushq	%rdi
	pushq	%rdx
	movl	-28(%rbp), %edx
	movl	-40(%rbp), %edi
	cmpl	%edx, %edi
	setl	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$1, %al
	je	.LtoUpper18
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
	pushq	%rsi
	movq	-24(%rbp), %rax
	movq	%rax, %rsi
	call	pub_String_init
	popq	%rsi
	popq	%rdi
	popq	%rdx
	movq	%rax, %rbx
	movq	%rbx, -48(%rbp)
	pushq	%rdx
	pushq	%rdi
	movq	-24(%rbp), %rax
	movq	%rax, %rdi
	call	free
	popq	%rdi
	popq	%rdx
	movq	-48(%rbp), %rax
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
	movq	%rbx, -17(%rbp)
	pushq	%rdi
	pushq	%rdx
	movb	$0, %dl
	movb	-9(%rbp), %dil
	cmpb	%dl, %dil
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.Lsplit20
	movb	$32, %bl
	movb	%bl, -9(%rbp)
.Lsplit20:
	movq	-8(%rbp), %r14
	movq	0(%r14), %rbx
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
	jmp	.Lsplit22
.Lsplit21:
	pushq	%rdx
	lea	-8(%rbp), %rax
	pushq	%rdi
	movq	(%rax), %rax
	movq	%rax, %rdi
	pushq	%rsi
	movl	-37(%rbp), %eax
	movl	%eax, %esi
	call	pub_String_at
	popq	%rsi
	popq	%rdi
	popq	%rdx
	movb	%al, %bl
	movb	%bl, -38(%rbp)
	pushq	%rdi
	pushq	%rdx
	movb	-9(%rbp), %dl
	movb	-38(%rbp), %dil
	cmpb	%dl, %dil
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.Lsplit23
	movl	-37(%rbp), %ebx
	movl	%ebx, -42(%rbp)
	pushq	%rdx
	lea	-8(%rbp), %rax
	pushq	%rdi
	movq	(%rax), %rax
	movq	%rax, %rdi
	pushq	%rsi
	movl	-33(%rbp), %eax
	movl	%eax, %esi
	pushq	%rdx
	movl	-42(%rbp), %eax
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
	pushq	%rsi
	movq	-50(%rbp), %rax
	movq	%rax, %rsi
	call	pub_LinkedList_append
	popq	%rsi
	popq	%rdi
	popq	%rdx
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movl	-42(%rbp), %edi
	add	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -33(%rbp)
.Lsplit23:
	add	$1, -37(%rbp)
.Lsplit22:
	pushq	%rdi
	pushq	%rdx
	movl	-29(%rbp), %edx
	movl	-37(%rbp), %edi
	cmpl	%edx, %edi
	setl	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$1, %al
	je	.Lsplit21
	pushq	%rdx
	lea	-8(%rbp), %rax
	pushq	%rdi
	movq	(%rax), %rax
	movq	%rax, %rdi
	pushq	%rsi
	movl	-33(%rbp), %eax
	movl	%eax, %esi
	pushq	%rdx
	movl	-29(%rbp), %eax
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
	pushq	%rsi
	movq	-41(%rbp), %rax
	movq	%rax, %rsi
	call	pub_LinkedList_append
	popq	%rsi
	popq	%rdi
	popq	%rdx
	movq	-17(%rbp), %rax
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
	je	.LtoInt24
	pushq	%rdx
	pushq	%rdi
	movq	$.strString.lambda_1649090706959_025, %rax
	movq	%rax, %rdi
	call	panic
	popq	%rdi
	popq	%rdx
.LtoInt24:
	pushq	%rdx
	movq	-8(%rbp), %r14
	pushq	%rdi
	movq	0(%r14), %rax
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
	movq	-8(%rbp), %r14
	movq	0(%r14), %rbx
	movq	%rbx, -20(%rbp)
	jmp	.Llen27
.Llen26:
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movq	-20(%rbp), %rdi
	add	%rdx, %rdi
	movq	%rdi, %rax
	popq	%rdx
	popq	%rdi
	movq	%rax, %rbx
	movq	%rbx, -20(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movl	-12(%rbp), %edi
	add	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -12(%rbp)
.Llen27:
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
	je	.Llen26
	movl	-12(%rbp), %eax
	leave
	ret
lower:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$16, %rsp
	movb	%dil, -1(%rbp)
	lea	-5(%rbp), %rax
	movq	%rax, %rbx
	movq	%rbx, -13(%rbp)
	movq	-13(%rbp), %rax
	movl	$0, %ebx
	movl	%ebx, (%rax)
	pushq	%rdi
	pushq	%rdx
	movb	$64, %dl
	movb	-1(%rbp), %dil
	cmpb	%dl, %dil
	setg	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.Llower28
	pushq	%rdi
	pushq	%rdx
	movb	$91, %dl
	movb	-1(%rbp), %dil
	cmpb	%dl, %dil
	setl	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.Llower29
	pushq	%rdi
	pushq	%rdx
	movl	$32, %edx
	movb	-1(%rbp), %dil
	add	%dl, %dil
	movb	%dil, %al
	popq	%rdx
	popq	%rdi
	movb	%al, %bl
	movb	%bl, -1(%rbp)
.Llower29:
.Llower28:
	movb	-1(%rbp), %al
	leave
	ret
upper:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$16, %rsp
	movb	%dil, -1(%rbp)
	lea	-5(%rbp), %rax
	movq	%rax, %rbx
	movq	%rbx, -13(%rbp)
	movq	-13(%rbp), %rax
	movl	$0, %ebx
	movl	%ebx, (%rax)
	pushq	%rdi
	pushq	%rdx
	movb	$96, %dl
	movb	-1(%rbp), %dil
	cmpb	%dl, %dil
	setg	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.Lupper30
	pushq	%rdi
	pushq	%rdx
	movb	$123, %dl
	movb	-1(%rbp), %dil
	cmpb	%dl, %dil
	setl	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.Lupper31
	pushq	%rdi
	pushq	%rdx
	movl	$32, %edx
	movb	-1(%rbp), %dil
	sub	%dl, %dil
	movb	%dil, %al
	popq	%rdx
	popq	%rdi
	movb	%al, %bl
	movb	%bl, -1(%rbp)
.Lupper31:
.Lupper30:
	movb	-1(%rbp), %al
	leave
	ret
lambda_1649090706959_0:
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
	pushq	%rdi
	movb	-10(%rbp), %al
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
	je	.LisNumeric1
	movq	-22(%rbp), %rax
	movb	$0, %bl
	movb	%bl, (%rax)
	movl	$1, %eax
	leave
	ret
.LisNumeric1:
	movl	$0, %eax
	leave
	ret


.data

.strString.lambda_1649090706959_025:
	.asciz	 "Attempt to convert non-numeric string to int"


.bss

