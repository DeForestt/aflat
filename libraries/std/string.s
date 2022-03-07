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
	movq	-16(%rbp), %rax
	movq	%rax, %rdi
	call	len
	movl	%eax, %ebx
	movl	%ebx, -20(%rbp)
	mov	$1, %edx
	mov	-20(%rbp), %eax
	add	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -20(%rbp)
	movl	-20(%rbp), %eax
	movl	%eax, %edi
	call	malloc
	movq	-8(%rbp), %rdx
	movq	%rax, %rbx
	movq	%rbx, 0(%rdx)
	movq	-8(%rbp), %rdx
	movq	0(%rdx), %rbx
	movq	%rbx, -28(%rbp)
	movq	-16(%rbp), %rax
	movq	%rax, %rdi
	movq	-28(%rbp), %rax
	movq	%rax, %rsi
	movl	-20(%rbp), %eax
	movl	%eax, %edx
	call	memcopy
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
	movq	0(%rdx), %rax
	movq	%rax, %rdi
	call	len
	movl	%eax, %ebx
	movl	%ebx, -20(%rbp)
	movq	-16(%rbp), %rdx
	movq	0(%rdx), %rax
	movq	%rax, %rdi
	call	len
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
	movl	-28(%rbp), %eax
	movl	%eax, %edi
	call	malloc
	movq	%rax, %rbx
	movq	%rbx, -36(%rbp)
	movq	-36(%rbp), %rbx
	movq	%rbx, -44(%rbp)
	movq	-8(%rbp), %rdx
	movq	0(%rdx), %rax
	movq	%rax, %rdi
	movq	-36(%rbp), %rax
	movq	%rax, %rsi
	movl	-20(%rbp), %eax
	movl	%eax, %edx
	call	memcopy
	mov	-20(%rbp), %edx
	mov	-36(%rbp), %rax
	add	%rdx, %rax
	movl	%eax, %ebx
	movl	%ebx, -36(%rbp)
	movq	-16(%rbp), %rdx
	movq	0(%rdx), %rax
	movq	%rax, %rdi
	movq	-36(%rbp), %rax
	movq	%rax, %rsi
	movl	-24(%rbp), %eax
	movl	%eax, %edx
	call	memcopy
	movl	$8, %eax
	movl	%eax, %edi
	call	malloc
	movq	%rax, %rdi
	movq	-44(%rbp), %rax
	movq	%rax, %rsi
	call	pub_String_init
	movq	%rax, %rbx
	movq	%rbx, -52(%rbp)
	movq	-44(%rbp), %rax
	movq	%rax, %rdi
	call	free
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
	movq	0(%rdx), %rax
	movq	%rax, %rdi
	call	len
	movl	%eax, %ebx
	movl	%ebx, -20(%rbp)
	movq	-16(%rbp), %rax
	movq	%rax, %rdi
	call	len
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
	movl	-28(%rbp), %eax
	movl	%eax, %edi
	call	malloc
	movq	%rax, %rbx
	movq	%rbx, -36(%rbp)
	movq	-36(%rbp), %rbx
	movq	%rbx, -44(%rbp)
	movq	-8(%rbp), %rdx
	movq	0(%rdx), %rax
	movq	%rax, %rdi
	movq	-36(%rbp), %rax
	movq	%rax, %rsi
	movl	-20(%rbp), %eax
	movl	%eax, %edx
	call	memcopy
	mov	-20(%rbp), %edx
	mov	-36(%rbp), %rax
	add	%rdx, %rax
	movl	%eax, %ebx
	movl	%ebx, -36(%rbp)
	movq	-16(%rbp), %rax
	movq	%rax, %rdi
	movq	-36(%rbp), %rax
	movq	%rax, %rsi
	movl	-24(%rbp), %eax
	movl	%eax, %edx
	call	memcopy
	movl	$8, %eax
	movl	%eax, %edi
	call	malloc
	movq	%rax, %rdi
	movq	-44(%rbp), %rax
	movq	%rax, %rsi
	call	pub_String_init
	movq	%rax, %rbx
	movq	%rbx, -52(%rbp)
	movq	-44(%rbp), %rax
	movq	%rax, %rdi
	call	free
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
	movq	-24(%rbp), %rax
	movq	%rax, %rdi
	call	len
	movl	%eax, %ebx
	movl	%ebx, -28(%rbp)
	mov	-12(%rbp), %edx
	mov	-16(%rbp), %eax
	sub	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -32(%rbp)
	movl	-32(%rbp), %eax
	movl	%eax, %edi
	call	malloc
	movq	%rax, %rbx
	movq	%rbx, -40(%rbp)
	movq	-40(%rbp), %rbx
	movq	%rbx, -48(%rbp)
	mov	-12(%rbp), %edx
	mov	-24(%rbp), %rax
	add	%rdx, %rax
	movl	%eax, %ebx
	movl	%ebx, -24(%rbp)
	movq	-24(%rbp), %rax
	movq	%rax, %rdi
	movq	-40(%rbp), %rax
	movq	%rax, %rsi
	movl	-32(%rbp), %eax
	movl	%eax, %edx
	call	memcopy
	movl	$8, %eax
	movl	%eax, %edi
	call	malloc
	movq	%rax, %rdi
	movq	-48(%rbp), %rax
	movq	%rax, %rsi
	call	pub_String_init
	movq	%rax, %rbx
	movq	%rbx, -56(%rbp)
	movq	-48(%rbp), %rax
	movq	%rax, %rdi
	call	free
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
	movq	-24(%rbp), %rax
	movq	%rax, %rdi
	movq	-32(%rbp), %rax
	movq	%rax, %rsi
	call	str_comp
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
	movq	0(%rdx), %rax
	movq	%rax, %rdi
	movq	-16(%rbp), %rax
	movq	%rax, %rsi
	call	str_comp
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
	movq	0(%rdx), %rax
	movq	%rax, %rdi
	movl	-12(%rbp), %eax
	movl	%eax, %esi
	call	str_at
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
	movq	0(%rdx), %rax
	movq	%rax, %rdi
	movb	-9(%rbp), %al
	movb	%al, %sil
	call	str_find
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
	movq	-16(%rbp), %rax
	movq	%rax, %rdi
	call	free
	movq	-8(%rbp), %rax
	movq	%rax, %rdi
	call	free
	leave
	ret
printString:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$16, %rsp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rdx
	movq	0(%rdx), %rax
	movq	%rax, %rdi
	call	print
	leave
	ret
getString:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$48, %rsp
	movl	$1, %eax
	movl	%eax, %edi
	call	malloc
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
	movq	-16(%rbp), %rax
	movq	%rax, %rdi
	movl	-20(%rbp), %eax
	movl	%eax, %esi
	call	realloc
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
	movl	$8, %eax
	movl	%eax, %edi
	call	malloc
	movq	%rax, %rdi
	movq	-16(%rbp), %rax
	movq	%rax, %rsi
	call	pub_String_init
	movq	%rax, %rbx
	movq	%rbx, -43(%rbp)
	movq	-16(%rbp), %rax
	movq	%rax, %rdi
	call	free
	movq	-43(%rbp), %rax
	leave
	ret
	leave
	ret


.data



.bss

