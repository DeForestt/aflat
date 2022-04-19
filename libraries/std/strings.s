.global	strings.hex_toInt
.global	strings.char_isDigit
.global	strings.str_forEach
.global	strings.str_at
.global	strings.str_reverse
.global	strings.str_subString
.global	strings.float_toString
.global	strings.sPrintFloat
.global	strings.str_toFloat
.global	strings.charToFloat
.global	strings.str_split
.global	strings.str_find
.global	strings.int_toStringL0
.global	strings.int_toString
.global	strings.str_toInt
.global	strings.str_concat
.global	strings.str_comp
.global	strings.len
.global	strings.ascii


.text

strings.ascii:
ascii:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$16, %rsp
	movl	%edi, -4(%rbp)
	lea	-4(%rbp), %rax
	movq	%rax, %rbx
	movq	%rbx, -12(%rbp)
	movq	-12(%rbp), %rax
	movb	(%rax), %al
	movb	%al, %al
	leave
	ret
strings.len:
len:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$16, %rsp
	movq	%rdi, -8(%rbp)
	pushq	%rdx
	pushq	%rdi
	movl	$0, %eax
	movl	%eax, %edi
	call	ascii
	popq	%rdi
	popq	%rdx
	movb	%al, %bl
	movb	%bl, -9(%rbp)
	movl	$0, %ebx
	movl	%ebx, -13(%rbp)
	jmp	.Llen1
.Llen0:
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movl	-13(%rbp), %r15d
	movl	%r15d, %edi
	add	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -13(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movq	-8(%rbp), %r15
	movq	%r15, %rdi
	add	%rdx, %rdi
	movq	%rdi, %rax
	popq	%rdx
	popq	%rdi
	movq	%rax, %rbx
	movq	%rbx, -8(%rbp)
.Llen1:
	pushq	%rdi
	pushq	%rdx
	movb	-9(%rbp), %r15b
	movb	%r15b, %dl
	movq	-8(%rbp), %rax
	movb	(%rax), %al
	movb	%al, %dil
	cmpb	%dl, %dil
	setne	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$1, %al
	je	.Llen0
	movl	-13(%rbp), %r15d
	movl	%r15d, %eax
	leave
	ret
strings.str_comp:
str_comp:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$32, %rsp
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	pushq	%rdx
	movq	-8(%rbp), %r15
	pushq	%rdi
	movq	%r15, %rax
	movq	%rax, %rdi
	call	len
	popq	%rdi
	popq	%rdx
	movl	%eax, %ebx
	movl	%ebx, -20(%rbp)
	pushq	%rdx
	movq	-16(%rbp), %r15
	pushq	%rdi
	movq	%r15, %rax
	movq	%rax, %rdi
	call	len
	popq	%rdi
	popq	%rdx
	movl	%eax, %ebx
	movl	%ebx, -24(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	-24(%rbp), %r15d
	movl	%r15d, %edx
	movl	-20(%rbp), %r15d
	movl	%r15d, %edi
	cmpl	%edx, %edi
	setne	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.Lstr_comp2
	movl	$0, %eax
	leave
	ret
.Lstr_comp2:
	movl	$0, %ebx
	movl	%ebx, -28(%rbp)
	jmp	.Lstr_comp4
.Lstr_comp3:
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
	movq	-8(%rbp), %rax
	movb	(%rax), %al
	movb	%al, %bl
	movb	%bl, -29(%rbp)
	movq	-16(%rbp), %rax
	movb	(%rax), %al
	movb	%al, %bl
	movb	%bl, -30(%rbp)
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
	cmpb	$0, %al
	je	.Lstr_comp5
	movl	$0, %eax
	leave
	ret
.Lstr_comp5:
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
	movq	-8(%rbp), %r15
	movq	%r15, %rdi
	add	%rdx, %rdi
	movq	%rdi, %rax
	popq	%rdx
	popq	%rdi
	movq	%rax, %rbx
	movq	%rbx, -8(%rbp)
.Lstr_comp4:
	pushq	%rdi
	pushq	%rdx
	movl	-20(%rbp), %r15d
	movl	%r15d, %edx
	movl	-28(%rbp), %r15d
	movl	%r15d, %edi
	cmpl	%edx, %edi
	setl	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$1, %al
	je	.Lstr_comp3
	movl	$1, %eax
	leave
	ret
strings.str_concat:
str_concat:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$32, %rsp
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	movq	%rdx, -24(%rbp)
	movb	$0, %bl
	movb	%bl, -25(%rbp)
	jmp	.Lstr_concat7
.Lstr_concat6:
	movq	-8(%rbp), %rax
	movb	(%rax), %al
	movb	%al, %bl
	movb	%bl, -26(%rbp)
	movb	-26(%rbp), %r15b
	movq	-24(%rbp), %rax
	movb	%r15b, %bl
	movb	%bl, (%rax)
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movq	-8(%rbp), %r15
	movq	%r15, %rdi
	add	%rdx, %rdi
	movq	%rdi, %rax
	popq	%rdx
	popq	%rdi
	movq	%rax, %rbx
	movq	%rbx, -8(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movq	-24(%rbp), %r15
	movq	%r15, %rdi
	add	%rdx, %rdi
	movq	%rdi, %rax
	popq	%rdx
	popq	%rdi
	movq	%rax, %rbx
	movq	%rbx, -24(%rbp)
.Lstr_concat7:
	pushq	%rdi
	pushq	%rdx
	movb	-25(%rbp), %r15b
	movb	%r15b, %dl
	movq	-8(%rbp), %rax
	movb	(%rax), %al
	movb	%al, %dil
	cmpb	%dl, %dil
	setne	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$1, %al
	je	.Lstr_concat6
	jmp	.Lstr_concat9
.Lstr_concat8:
	movq	-16(%rbp), %rax
	movb	(%rax), %al
	movb	%al, %bl
	movb	%bl, -26(%rbp)
	movb	-26(%rbp), %r15b
	movq	-24(%rbp), %rax
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
	movq	-24(%rbp), %r15
	movq	%r15, %rdi
	add	%rdx, %rdi
	movq	%rdi, %rax
	popq	%rdx
	popq	%rdi
	movq	%rax, %rbx
	movq	%rbx, -24(%rbp)
.Lstr_concat9:
	pushq	%rdi
	pushq	%rdx
	movb	-25(%rbp), %r15b
	movb	%r15b, %dl
	movq	-16(%rbp), %rax
	movb	(%rax), %al
	movb	%al, %dil
	cmpb	%dl, %dil
	setne	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$1, %al
	je	.Lstr_concat8
	movb	-25(%rbp), %r15b
	movq	-24(%rbp), %rax
	movb	%r15b, %bl
	movb	%bl, (%rax)
	movl	$0, %eax
	leave
	ret
strings.str_toInt:
str_toInt:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$48, %rsp
	movq	%rdi, -8(%rbp)
	movl	$1, %ebx
	movl	%ebx, -12(%rbp)
	movq	-8(%rbp), %rax
	movb	(%rax), %al
	movb	%al, %bl
	movb	%bl, -13(%rbp)
	pushq	%rdi
	pushq	%rdx
	movb	$45, %dl
	movb	-13(%rbp), %r15b
	movb	%r15b, %dil
	cmpb	%dl, %dil
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.Lstr_toInt10
	movl	$-1, %ebx
	movl	%ebx, -12(%rbp)
	pushq	%rdi
	pushq	%rdx
	movq	$1, %rdx
	movq	-8(%rbp), %r15
	movq	%r15, %rdi
	add	%rdx, %rdi
	movq	%rdi, %rax
	popq	%rdx
	popq	%rdi
	movq	%rax, %rbx
	movq	%rbx, -8(%rbp)
.Lstr_toInt10:
	pushq	%rdx
	pushq	%rdi
	movl	$0, %eax
	movl	%eax, %edi
	call	ascii
	popq	%rdi
	popq	%rdx
	movb	%al, %bl
	movb	%bl, -14(%rbp)
	movl	$0, %ebx
	movl	%ebx, -18(%rbp)
	pushq	%rdx
	movq	-8(%rbp), %r15
	pushq	%rdi
	movq	%r15, %rax
	movq	%rax, %rdi
	call	len
	popq	%rdi
	popq	%rdx
	movl	%eax, %ebx
	movl	%ebx, -22(%rbp)
	movl	$0, %ebx
	movl	%ebx, -26(%rbp)
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
	movl	%ebx, -30(%rbp)
	jmp	.Lstr_toInt12
.Lstr_toInt11:
	pushq	%rdx
	pushq	%rdi
	movl	$10, %eax
	movl	%eax, %edi
	movl	-30(%rbp), %r15d
	pushq	%rsi
	movl	%r15d, %eax
	movl	%eax, %esi
	call	math.exp
	popq	%rsi
	popq	%rdi
	popq	%rdx
	movl	%eax, %ebx
	movl	%ebx, -18(%rbp)
	movq	-8(%rbp), %rax
	movb	(%rax), %al
	movb	%al, %bl
	movb	%bl, -31(%rbp)
	pushq	%rdx
	movb	-31(%rbp), %r15b
	pushq	%rdi
	movb	%r15b, %al
	movb	%al, %dil
	call	io.toInt
	popq	%rdi
	popq	%rdx
	movl	%eax, %ebx
	movl	%ebx, -35(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	-35(%rbp), %r15d
	movl	%r15d, %edx
	movl	-18(%rbp), %r15d
	movl	%r15d, %edi
	imul	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -18(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	-18(%rbp), %r15d
	movl	%r15d, %edx
	movl	-26(%rbp), %r15d
	movl	%r15d, %edi
	add	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -26(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movl	-30(%rbp), %r15d
	movl	%r15d, %edi
	sub	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -30(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movq	-8(%rbp), %r15
	movq	%r15, %rdi
	add	%rdx, %rdi
	movq	%rdi, %rax
	popq	%rdx
	popq	%rdi
	movq	%rax, %rbx
	movq	%rbx, -8(%rbp)
.Lstr_toInt12:
	pushq	%rdi
	pushq	%rdx
	movl	$-1, %edx
	movl	-30(%rbp), %r15d
	movl	%r15d, %edi
	cmpl	%edx, %edi
	setg	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$1, %al
	je	.Lstr_toInt11
	pushq	%rdi
	pushq	%rdx
	movl	-12(%rbp), %r15d
	movl	%r15d, %edx
	movl	-26(%rbp), %r15d
	movl	%r15d, %edi
	imul	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -26(%rbp)
	movl	-26(%rbp), %r15d
	movl	%r15d, %eax
	leave
	ret
strings.int_toString:
int_toString:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$32, %rsp
	movl	%edi, -4(%rbp)
	movq	%rsi, -12(%rbp)
	movl	$0, %ebx
	movl	%ebx, -16(%rbp)
	movl	$10, %ebx
	movl	%ebx, -20(%rbp)
	movl	$0, %ebx
	movl	%ebx, -24(%rbp)
	movl	$0, %ebx
	movl	%ebx, -28(%rbp)
	pushq	%rdx
	pushq	%rdi
	movl	$0, %eax
	movl	%eax, %edi
	call	ascii
	popq	%rdi
	popq	%rdx
	movb	%al, %bl
	movb	%bl, -29(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	$0, %edx
	movl	-4(%rbp), %r15d
	movl	%r15d, %edi
	cmpl	%edx, %edi
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.Lint_toString13
	movq	-12(%rbp), %rax
	movb	$48, %bl
	movb	%bl, (%rax)
	movb	-29(%rbp), %r15b
	movq	-12(%rbp), %rax
	movb	%r15b, %bl
	movb	%bl, (%rax)
	pushq	%rdi
	pushq	%rdx
	movq	$1, %rdx
	movq	-12(%rbp), %r15
	movq	%r15, %rdi
	add	%rdx, %rdi
	movq	%rdi, %rax
	popq	%rdx
	popq	%rdi
	movq	%rax, %rbx
	movq	%rbx, -12(%rbp)
	movl	$0, %eax
	leave
	ret
.Lint_toString13:
	pushq	%rdi
	pushq	%rdx
	movl	$0, %edx
	movl	-4(%rbp), %r15d
	movl	%r15d, %edi
	cmpl	%edx, %edi
	setl	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.Lint_toString14
	movq	-12(%rbp), %rax
	movb	$45, %bl
	movb	%bl, (%rax)
	pushq	%rdi
	pushq	%rdx
	movq	$1, %rdx
	movq	-12(%rbp), %r15
	movq	%r15, %rdi
	add	%rdx, %rdi
	movq	%rdi, %rax
	popq	%rdx
	popq	%rdi
	movq	%rax, %rbx
	movq	%rbx, -12(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	$-1, %edx
	movl	-4(%rbp), %r15d
	movl	%r15d, %edi
	imul	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -4(%rbp)
.Lint_toString14:
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movl	-4(%rbp), %r15d
	movl	%r15d, %edi
	imul	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -16(%rbp)
	jmp	.Lint_toString16
.Lint_toString15:
	pushq	%rdi
	pushq	%rdx
	movl	-16(%rbp), %r15d
	movl	%r15d, %eax
	movl	-20(%rbp), %r15d
	movl	%r15d, %ecx
	cltd
	idiv	%ecx
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -16(%rbp)
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
.Lint_toString16:
	pushq	%rdi
	pushq	%rdx
	movl	$0, %edx
	movl	-16(%rbp), %r15d
	movl	%r15d, %edi
	cmpl	%edx, %edi
	setne	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$1, %al
	je	.Lint_toString15
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movl	-28(%rbp), %r15d
	movl	%r15d, %edi
	sub	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -28(%rbp)
	movb	$32, %bl
	movb	%bl, -30(%rbp)
	jmp	.Lint_toString18
.Lint_toString17:
	pushq	%rdx
	pushq	%rdi
	movl	$10, %eax
	movl	%eax, %edi
	movl	-28(%rbp), %r15d
	pushq	%rsi
	movl	%r15d, %eax
	movl	%eax, %esi
	call	math.exp
	popq	%rsi
	popq	%rdi
	popq	%rdx
	movl	%eax, %ebx
	movl	%ebx, -24(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	-4(%rbp), %r15d
	movl	%r15d, %eax
	movl	-24(%rbp), %r15d
	movl	%r15d, %ecx
	cltd
	idiv	%ecx
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -16(%rbp)
	pushq	%rdx
	movl	-16(%rbp), %r15d
	pushq	%rdi
	movl	%r15d, %eax
	movl	%eax, %edi
	call	io.toChar
	popq	%rdi
	popq	%rdx
	movb	%al, %bl
	movb	%bl, -30(%rbp)
	movb	-30(%rbp), %r15b
	movq	-12(%rbp), %rax
	movb	%r15b, %bl
	movb	%bl, (%rax)
	pushq	%rdi
	pushq	%rdx
	movq	$1, %rdx
	movq	-12(%rbp), %r15
	movq	%r15, %rdi
	add	%rdx, %rdi
	movq	%rdi, %rax
	popq	%rdx
	popq	%rdi
	movq	%rax, %rbx
	movq	%rbx, -12(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	-24(%rbp), %r15d
	movl	%r15d, %edx
	movl	-16(%rbp), %r15d
	movl	%r15d, %edi
	imul	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -16(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	-16(%rbp), %r15d
	movl	%r15d, %edx
	movl	-4(%rbp), %r15d
	movl	%r15d, %edi
	sub	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -4(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movl	-28(%rbp), %r15d
	movl	%r15d, %edi
	sub	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -28(%rbp)
.Lint_toString18:
	pushq	%rdi
	pushq	%rdx
	movl	$-1, %edx
	movl	-28(%rbp), %r15d
	movl	%r15d, %edi
	cmpl	%edx, %edi
	setg	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$1, %al
	je	.Lint_toString17
	movb	-29(%rbp), %r15b
	movq	-12(%rbp), %rax
	movb	%r15b, %bl
	movb	%bl, (%rax)
	movl	$0, %eax
	leave
	ret
strings.int_toStringL0:
int_toStringL0:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$48, %rsp
	movl	%edi, -4(%rbp)
	movq	%rsi, -12(%rbp)
	movl	%edx, -16(%rbp)
	movl	$0, %ebx
	movl	%ebx, -20(%rbp)
	movl	$10, %ebx
	movl	%ebx, -24(%rbp)
	movl	$0, %ebx
	movl	%ebx, -28(%rbp)
	movl	$0, %ebx
	movl	%ebx, -32(%rbp)
	pushq	%rdx
	pushq	%rdi
	movl	$0, %eax
	movl	%eax, %edi
	call	ascii
	popq	%rdi
	popq	%rdx
	movb	%al, %bl
	movb	%bl, -33(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movl	-4(%rbp), %r15d
	movl	%r15d, %edi
	imul	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -20(%rbp)
	jmp	.Lint_toStringL020
.Lint_toStringL019:
	pushq	%rdi
	pushq	%rdx
	movl	-20(%rbp), %r15d
	movl	%r15d, %eax
	movl	-24(%rbp), %r15d
	movl	%r15d, %ecx
	cltd
	idiv	%ecx
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -20(%rbp)
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
.Lint_toStringL020:
	pushq	%rdi
	pushq	%rdx
	movl	$0, %edx
	movl	-20(%rbp), %r15d
	movl	%r15d, %edi
	cmpl	%edx, %edi
	setne	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$1, %al
	je	.Lint_toStringL019
	pushq	%rdi
	pushq	%rdx
	movl	-32(%rbp), %r15d
	movl	%r15d, %edx
	movl	-16(%rbp), %r15d
	movl	%r15d, %edi
	sub	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -37(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	$0, %edx
	movl	-37(%rbp), %r15d
	movl	%r15d, %edi
	cmpl	%edx, %edi
	setl	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.Lint_toStringL021
	movl	$0, %ebx
	movl	%ebx, -37(%rbp)
.Lint_toStringL021:
	movl	$0, %ebx
	movl	%ebx, -20(%rbp)
	movl	$0, %ebx
	movl	%ebx, -32(%rbp)
	jmp	.Lint_toStringL023
.Lint_toStringL022:
	movq	-12(%rbp), %rax
	movb	$48, %bl
	movb	%bl, (%rax)
	pushq	%rdi
	pushq	%rdx
	movq	$1, %rdx
	movq	-12(%rbp), %r15
	movq	%r15, %rdi
	add	%rdx, %rdi
	movq	%rdi, %rax
	popq	%rdx
	popq	%rdi
	movq	%rax, %rbx
	movq	%rbx, -12(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movl	-37(%rbp), %r15d
	movl	%r15d, %edi
	sub	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -37(%rbp)
.Lint_toStringL023:
	pushq	%rdi
	pushq	%rdx
	movl	$0, %edx
	movl	-37(%rbp), %r15d
	movl	%r15d, %edi
	cmpl	%edx, %edi
	setg	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$1, %al
	je	.Lint_toStringL022
	pushq	%rdi
	pushq	%rdx
	movl	$0, %edx
	movl	-4(%rbp), %r15d
	movl	%r15d, %edi
	cmpl	%edx, %edi
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.Lint_toStringL024
	movq	-12(%rbp), %rax
	movb	$48, %bl
	movb	%bl, (%rax)
	movb	-33(%rbp), %r15b
	movq	-12(%rbp), %rax
	movb	%r15b, %bl
	movb	%bl, (%rax)
	pushq	%rdi
	pushq	%rdx
	movq	$1, %rdx
	movq	-12(%rbp), %r15
	movq	%r15, %rdi
	add	%rdx, %rdi
	movq	%rdi, %rax
	popq	%rdx
	popq	%rdi
	movq	%rax, %rbx
	movq	%rbx, -12(%rbp)
	movl	$0, %eax
	leave
	ret
.Lint_toStringL024:
	pushq	%rdi
	pushq	%rdx
	movl	$0, %edx
	movl	-4(%rbp), %r15d
	movl	%r15d, %edi
	cmpl	%edx, %edi
	setl	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.Lint_toStringL025
	movq	-12(%rbp), %rax
	movb	$45, %bl
	movb	%bl, (%rax)
	pushq	%rdi
	pushq	%rdx
	movq	$1, %rdx
	movq	-12(%rbp), %r15
	movq	%r15, %rdi
	add	%rdx, %rdi
	movq	%rdi, %rax
	popq	%rdx
	popq	%rdi
	movq	%rax, %rbx
	movq	%rbx, -12(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	$-1, %edx
	movl	-4(%rbp), %r15d
	movl	%r15d, %edi
	imul	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -4(%rbp)
.Lint_toStringL025:
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movl	-4(%rbp), %r15d
	movl	%r15d, %edi
	imul	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -20(%rbp)
	jmp	.Lint_toStringL027
.Lint_toStringL026:
	pushq	%rdi
	pushq	%rdx
	movl	-20(%rbp), %r15d
	movl	%r15d, %eax
	movl	-24(%rbp), %r15d
	movl	%r15d, %ecx
	cltd
	idiv	%ecx
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -20(%rbp)
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
.Lint_toStringL027:
	pushq	%rdi
	pushq	%rdx
	movl	$0, %edx
	movl	-20(%rbp), %r15d
	movl	%r15d, %edi
	cmpl	%edx, %edi
	setne	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$1, %al
	je	.Lint_toStringL026
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movl	-32(%rbp), %r15d
	movl	%r15d, %edi
	sub	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -32(%rbp)
	movb	$32, %bl
	movb	%bl, -38(%rbp)
	jmp	.Lint_toStringL029
.Lint_toStringL028:
	pushq	%rdx
	pushq	%rdi
	movl	$10, %eax
	movl	%eax, %edi
	movl	-32(%rbp), %r15d
	pushq	%rsi
	movl	%r15d, %eax
	movl	%eax, %esi
	call	math.exp
	popq	%rsi
	popq	%rdi
	popq	%rdx
	movl	%eax, %ebx
	movl	%ebx, -28(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	-4(%rbp), %r15d
	movl	%r15d, %eax
	movl	-28(%rbp), %r15d
	movl	%r15d, %ecx
	cltd
	idiv	%ecx
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -20(%rbp)
	pushq	%rdx
	movl	-20(%rbp), %r15d
	pushq	%rdi
	movl	%r15d, %eax
	movl	%eax, %edi
	call	io.toChar
	popq	%rdi
	popq	%rdx
	movb	%al, %bl
	movb	%bl, -38(%rbp)
	movb	-38(%rbp), %r15b
	movq	-12(%rbp), %rax
	movb	%r15b, %bl
	movb	%bl, (%rax)
	pushq	%rdi
	pushq	%rdx
	movq	$1, %rdx
	movq	-12(%rbp), %r15
	movq	%r15, %rdi
	add	%rdx, %rdi
	movq	%rdi, %rax
	popq	%rdx
	popq	%rdi
	movq	%rax, %rbx
	movq	%rbx, -12(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	-28(%rbp), %r15d
	movl	%r15d, %edx
	movl	-20(%rbp), %r15d
	movl	%r15d, %edi
	imul	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -20(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	-20(%rbp), %r15d
	movl	%r15d, %edx
	movl	-4(%rbp), %r15d
	movl	%r15d, %edi
	sub	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -4(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movl	-32(%rbp), %r15d
	movl	%r15d, %edi
	sub	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -32(%rbp)
.Lint_toStringL029:
	pushq	%rdi
	pushq	%rdx
	movl	$-1, %edx
	movl	-32(%rbp), %r15d
	movl	%r15d, %edi
	cmpl	%edx, %edi
	setg	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$1, %al
	je	.Lint_toStringL028
	movb	-33(%rbp), %r15b
	movq	-12(%rbp), %rax
	movb	%r15b, %bl
	movb	%bl, (%rax)
	leave
	ret
strings.str_find:
str_find:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$32, %rsp
	movq	%rdi, -8(%rbp)
	movb	%sil, -9(%rbp)
	movl	$0, %ebx
	movl	%ebx, -13(%rbp)
	pushq	%rdx
	movq	-8(%rbp), %r15
	pushq	%rdi
	movq	%r15, %rax
	movq	%rax, %rdi
	call	len
	popq	%rdi
	popq	%rdx
	movl	%eax, %ebx
	movl	%ebx, -17(%rbp)
	jmp	.Lstr_find31
.Lstr_find30:
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movq	-8(%rbp), %r15
	movq	%r15, %rdi
	add	%rdx, %rdi
	movq	%rdi, %rax
	popq	%rdx
	popq	%rdi
	movq	%rax, %rbx
	movq	%rbx, -8(%rbp)
	movq	-8(%rbp), %rax
	movb	(%rax), %al
	movb	%al, %bl
	movb	%bl, -18(%rbp)
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
	cmpb	$0, %al
	je	.Lstr_find32
	movl	-13(%rbp), %r15d
	movl	%r15d, %eax
	leave
	ret
.Lstr_find32:
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movl	-13(%rbp), %r15d
	movl	%r15d, %edi
	add	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -13(%rbp)
.Lstr_find31:
	pushq	%rdi
	pushq	%rdx
	movl	-17(%rbp), %r15d
	movl	%r15d, %edx
	movl	-13(%rbp), %r15d
	movl	%r15d, %edi
	cmpl	%edx, %edi
	setl	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$1, %al
	je	.Lstr_find30
	movl	$-1, %eax
	leave
	ret
strings.str_split:
str_split:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$32, %rsp
	movq	%rdi, -8(%rbp)
	movb	%sil, -9(%rbp)
	movq	%rdx, -17(%rbp)
	movq	%rcx, -25(%rbp)
	pushq	%rdx
	pushq	%rdi
	movl	$0, %eax
	movl	%eax, %edi
	call	ascii
	popq	%rdi
	popq	%rdx
	movb	%al, %bl
	movb	%bl, -26(%rbp)
	movq	-8(%rbp), %rax
	movb	(%rax), %al
	movb	%al, %bl
	movb	%bl, -27(%rbp)
	jmp	.Lstr_split34
.Lstr_split33:
	pushq	%rdi
	pushq	%rdx
	movb	-26(%rbp), %r15b
	movb	%r15b, %dl
	movb	-27(%rbp), %r15b
	movb	%r15b, %dil
	cmpb	%dl, %dil
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.Lstr_split35
	movl	$-1, %eax
	leave
	ret
.Lstr_split35:
	pushq	%rdi
	pushq	%rdx
	movb	-9(%rbp), %r15b
	movb	%r15b, %dl
	movb	-27(%rbp), %r15b
	movb	%r15b, %dil
	cmpb	%dl, %dil
	setne	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.Lstr_split36
	movb	-27(%rbp), %r15b
	movq	-17(%rbp), %rax
	movb	%r15b, %bl
	movb	%bl, (%rax)
.Lstr_split36:
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movq	-8(%rbp), %r15
	movq	%r15, %rdi
	add	%rdx, %rdi
	movq	%rdi, %rax
	popq	%rdx
	popq	%rdi
	movq	%rax, %rbx
	movq	%rbx, -8(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movq	-17(%rbp), %r15
	movq	%r15, %rdi
	add	%rdx, %rdi
	movq	%rdi, %rax
	popq	%rdx
	popq	%rdi
	movq	%rax, %rbx
	movq	%rbx, -17(%rbp)
	movq	-8(%rbp), %rax
	movb	(%rax), %al
	movb	%al, %bl
	movb	%bl, -27(%rbp)
.Lstr_split34:
	pushq	%rdi
	pushq	%rdx
	movb	-9(%rbp), %r15b
	movb	%r15b, %dl
	movb	-27(%rbp), %r15b
	movb	%r15b, %dil
	cmpb	%dl, %dil
	setne	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$1, %al
	je	.Lstr_split33
	movb	-26(%rbp), %r15b
	movq	-17(%rbp), %rax
	movb	%r15b, %bl
	movb	%bl, (%rax)
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movq	-8(%rbp), %r15
	movq	%r15, %rdi
	add	%rdx, %rdi
	movq	%rdi, %rax
	popq	%rdx
	popq	%rdi
	movq	%rax, %rbx
	movq	%rbx, -8(%rbp)
	pushq	%rdi
	pushq	%rdx
	movb	-26(%rbp), %r15b
	movb	%r15b, %dl
	movq	-8(%rbp), %rax
	movb	(%rax), %al
	movb	%al, %dil
	cmpb	%dl, %dil
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.Lstr_split37
	movl	$-1, %eax
	leave
	ret
.Lstr_split37:
	pushq	%rdx
	pushq	%rdi
	movq	$.strstr_split38, %rax
	movq	%rax, %rdi
	movq	-8(%rbp), %r15
	pushq	%rsi
	movq	%r15, %rax
	movq	%rax, %rsi
	movq	-25(%rbp), %r15
	pushq	%rdx
	movq	%r15, %rax
	movq	%rax, %rdx
	call	str_concat
	popq	%rdx
	popq	%rsi
	popq	%rdi
	popq	%rdx
	movl	$0, %eax
	leave
	ret
strings.charToFloat:
charToFloat:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$16, %rsp
	movb	%dil, -1(%rbp)
	movss	.floatcharToFloat39, %xmm0
	movss	%xmm0, %xmm0
	movss	%xmm0, -5(%rbp)
	pushq	%rdi
	pushq	%rdx
	movb	$48, %dl
	movb	-1(%rbp), %r15b
	movb	%r15b, %dil
	cmpb	%dl, %dil
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.LcharToFloat40
	movss	.floatcharToFloat41, %xmm0
	movss	%xmm0, %xmm0
	movss	%xmm0, -5(%rbp)
.LcharToFloat40:
	pushq	%rdi
	pushq	%rdx
	movb	$49, %dl
	movb	-1(%rbp), %r15b
	movb	%r15b, %dil
	cmpb	%dl, %dil
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.LcharToFloat42
	movss	.floatcharToFloat43, %xmm0
	movss	%xmm0, %xmm0
	movss	%xmm0, -5(%rbp)
.LcharToFloat42:
	pushq	%rdi
	pushq	%rdx
	movb	$50, %dl
	movb	-1(%rbp), %r15b
	movb	%r15b, %dil
	cmpb	%dl, %dil
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.LcharToFloat44
	movss	.floatcharToFloat45, %xmm0
	movss	%xmm0, %xmm0
	movss	%xmm0, -5(%rbp)
.LcharToFloat44:
	pushq	%rdi
	pushq	%rdx
	movb	$51, %dl
	movb	-1(%rbp), %r15b
	movb	%r15b, %dil
	cmpb	%dl, %dil
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.LcharToFloat46
	movss	.floatcharToFloat47, %xmm0
	movss	%xmm0, %xmm0
	movss	%xmm0, -5(%rbp)
.LcharToFloat46:
	pushq	%rdi
	pushq	%rdx
	movb	$52, %dl
	movb	-1(%rbp), %r15b
	movb	%r15b, %dil
	cmpb	%dl, %dil
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.LcharToFloat48
	movss	.floatcharToFloat49, %xmm0
	movss	%xmm0, %xmm0
	movss	%xmm0, -5(%rbp)
.LcharToFloat48:
	pushq	%rdi
	pushq	%rdx
	movb	$53, %dl
	movb	-1(%rbp), %r15b
	movb	%r15b, %dil
	cmpb	%dl, %dil
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.LcharToFloat50
	movss	.floatcharToFloat51, %xmm0
	movss	%xmm0, %xmm0
	movss	%xmm0, -5(%rbp)
.LcharToFloat50:
	pushq	%rdi
	pushq	%rdx
	movb	$54, %dl
	movb	-1(%rbp), %r15b
	movb	%r15b, %dil
	cmpb	%dl, %dil
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.LcharToFloat52
	movss	.floatcharToFloat53, %xmm0
	movss	%xmm0, %xmm0
	movss	%xmm0, -5(%rbp)
.LcharToFloat52:
	pushq	%rdi
	pushq	%rdx
	movb	$55, %dl
	movb	-1(%rbp), %r15b
	movb	%r15b, %dil
	cmpb	%dl, %dil
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.LcharToFloat54
	movss	.floatcharToFloat55, %xmm0
	movss	%xmm0, %xmm0
	movss	%xmm0, -5(%rbp)
.LcharToFloat54:
	pushq	%rdi
	pushq	%rdx
	movb	$56, %dl
	movb	-1(%rbp), %r15b
	movb	%r15b, %dil
	cmpb	%dl, %dil
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.LcharToFloat56
	movss	.floatcharToFloat57, %xmm0
	movss	%xmm0, %xmm0
	movss	%xmm0, -5(%rbp)
.LcharToFloat56:
	pushq	%rdi
	pushq	%rdx
	movb	$57, %dl
	movb	-1(%rbp), %r15b
	movb	%r15b, %dil
	cmpb	%dl, %dil
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.LcharToFloat58
	movss	.floatcharToFloat59, %xmm0
	movss	%xmm0, %xmm0
	movss	%xmm0, -5(%rbp)
.LcharToFloat58:
	movss	-5(%rbp), %xmm0
	movss	%xmm0, %xmm0
	leave
	ret
strings.str_toFloat:
str_toFloat:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$112, %rsp
	movq	%rdi, -8(%rbp)
	lea	-28(%rbp), %rax
	movq	%rax, %rbx
	movq	%rbx, -36(%rbp)
	lea	-56(%rbp), %rax
	movq	%rax, %rbx
	movq	%rbx, -64(%rbp)
	pushq	%rdx
	movq	-8(%rbp), %r15
	pushq	%rdi
	movq	%r15, %rax
	movq	%rax, %rdi
	pushq	%rsi
	movb	$46, %al
	movb	%al, %sil
	movq	-36(%rbp), %r15
	pushq	%rdx
	movq	%r15, %rax
	movq	%rax, %rdx
	movq	-64(%rbp), %r15
	pushq	%rcx
	movq	%r15, %rax
	movq	%rax, %rcx
	call	str_split
	popq	%rcx
	popq	%rdx
	popq	%rsi
	popq	%rdi
	popq	%rdx
	pushq	%rdx
	movq	-36(%rbp), %r15
	pushq	%rdi
	movq	%r15, %rax
	movq	%rax, %rdi
	call	len
	popq	%rdi
	popq	%rdx
	movl	%eax, %ebx
	movl	%ebx, -68(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movl	-68(%rbp), %r15d
	movl	%r15d, %edi
	sub	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -68(%rbp)
	movss	.floatstr_toFloat60, %xmm0
	movss	%xmm0, %xmm0
	movss	%xmm0, -72(%rbp)
	movl	$0, %ebx
	movl	%ebx, -76(%rbp)
	movss	.floatstr_toFloat61, %xmm0
	movss	%xmm0, %xmm0
	movss	%xmm0, -80(%rbp)
	jmp	.Lstr_toFloat63
.Lstr_toFloat62:
	pushq	%rdi
	pushq	%rdx
	movss	.floatstr_toFloat64, %xmm0
	movss	%xmm0, %xmm1
	movss	-80(%rbp), %xmm0
	movss	%xmm0, %xmm0
	mulss	%xmm1, %xmm0
	movss	%xmm0, %xmm2
	popq	%rdx
	popq	%rdi
	movss	%xmm2, %xmm0
	movss	%xmm0, -80(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movl	-76(%rbp), %r15d
	movl	%r15d, %edi
	add	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -76(%rbp)
.Lstr_toFloat63:
	pushq	%rdi
	pushq	%rdx
	movl	-68(%rbp), %r15d
	movl	%r15d, %edx
	movl	-76(%rbp), %r15d
	movl	%r15d, %edi
	cmpl	%edx, %edi
	setl	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$1, %al
	je	.Lstr_toFloat62
	movl	$0, %ebx
	movl	%ebx, -76(%rbp)
	movss	.floatstr_toFloat65, %xmm0
	movss	%xmm0, %xmm0
	movss	%xmm0, -84(%rbp)
	pushq	%rdx
	pushq	%rdi
	movl	$0, %eax
	movl	%eax, %edi
	call	ascii
	popq	%rdi
	popq	%rdx
	movb	%al, %bl
	movb	%bl, -85(%rbp)
	movq	-8(%rbp), %rax
	movb	(%rax), %al
	movb	%al, %bl
	movb	%bl, -86(%rbp)
	movss	.floatstr_toFloat66, %xmm0
	movss	%xmm0, %xmm0
	movss	%xmm0, -90(%rbp)
	jmp	.Lstr_toFloat68
.Lstr_toFloat67:
	pushq	%rdi
	pushq	%rdx
	movb	$46, %dl
	movb	-86(%rbp), %r15b
	movb	%r15b, %dil
	cmpb	%dl, %dil
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.Lstr_toFloat69
	movss	.floatstr_toFloat70, %xmm0
	movss	%xmm0, %xmm0
	movss	%xmm0, -80(%rbp)
.Lstr_toFloat69:
	pushq	%rdi
	pushq	%rdx
	movb	$46, %dl
	movb	-86(%rbp), %r15b
	movb	%r15b, %dil
	cmpb	%dl, %dil
	setne	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.Lstr_toFloat71
	pushq	%rdx
	movb	-86(%rbp), %r15b
	pushq	%rdi
	movb	%r15b, %al
	movb	%al, %dil
	call	charToFloat
	popq	%rdi
	popq	%rdx
	movl	%eax, %ebx
	movl	%ebx, -84(%rbp)
	pushq	%rdi
	pushq	%rdx
	movss	.floatstr_toFloat72, %xmm0
	movss	%xmm0, %xmm1
	movss	-72(%rbp), %xmm0
	movss	%xmm0, %xmm0
	addss	%xmm1, %xmm0
	movss	%xmm0, %xmm2
	popq	%rdx
	popq	%rdi
	movss	%xmm2, %xmm0
	movss	%xmm0, -94(%rbp)
	pushq	%rdi
	pushq	%rdx
	movss	-80(%rbp), %xmm0
	movss	%xmm0, %xmm1
	movss	-84(%rbp), %xmm0
	movss	%xmm0, %xmm0
	mulss	%xmm1, %xmm0
	movss	%xmm0, %xmm2
	popq	%rdx
	popq	%rdi
	movss	%xmm2, %xmm0
	movss	%xmm0, -72(%rbp)
	pushq	%rdi
	pushq	%rdx
	movss	-94(%rbp), %xmm0
	movss	%xmm0, %xmm1
	movss	-72(%rbp), %xmm0
	movss	%xmm0, %xmm0
	addss	%xmm1, %xmm0
	movss	%xmm0, %xmm2
	popq	%rdx
	popq	%rdi
	movss	%xmm2, %xmm0
	movss	%xmm0, -72(%rbp)
	pushq	%rdi
	pushq	%rdx
	movss	-90(%rbp), %xmm0
	movss	%xmm0, %xmm1
	movss	-80(%rbp), %xmm0
	movss	%xmm0, %xmm0
	divss	%xmm1, %xmm0
	popq	%rdx
	popq	%rdi
	movss	%xmm0, %xmm0
	movss	%xmm0, -80(%rbp)
.Lstr_toFloat71:
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movq	-8(%rbp), %r15
	movq	%r15, %rdi
	add	%rdx, %rdi
	movq	%rdi, %rax
	popq	%rdx
	popq	%rdi
	movq	%rax, %rbx
	movq	%rbx, -8(%rbp)
	movq	-8(%rbp), %rax
	movb	(%rax), %al
	movb	%al, %bl
	movb	%bl, -86(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movl	-76(%rbp), %r15d
	movl	%r15d, %edi
	add	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -76(%rbp)
.Lstr_toFloat68:
	pushq	%rdi
	pushq	%rdx
	movb	-85(%rbp), %r15b
	movb	%r15b, %dl
	movb	-86(%rbp), %r15b
	movb	%r15b, %dil
	cmpb	%dl, %dil
	setne	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$1, %al
	je	.Lstr_toFloat67
	lea	-72(%rbp), %rax
	movq	%rax, %rbx
	movq	%rbx, -98(%rbp)
	movss	.floatstr_toFloat73, %xmm0
	movss	%xmm0, %xmm0
	movss	%xmm0, -102(%rbp)
	pushq	%rdi
	pushq	%rdx
	movss	-102(%rbp), %xmm0
	movss	%xmm0, %xmm1
	movss	-102(%rbp), %xmm0
	movss	%xmm0, %xmm0
	mulss	%xmm1, %xmm0
	movss	%xmm0, %xmm2
	popq	%rdx
	popq	%rdi
	movss	%xmm2, %xmm0
	movss	%xmm0, -102(%rbp)
	movq	-98(%rbp), %rax
	movl	(%rax), %eax
	movl	%eax, %ebx
	movl	%ebx, -106(%rbp)
	movl	-106(%rbp), %r15d
	movl	%r15d, %eax
	leave
	ret
strings.sPrintFloat:
sPrintFloat:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$16, %rsp
	movq	%rdi, -8(%rbp)
	pushq	%rdx
	movq	-8(%rbp), %r15
	pushq	%rdi
	movq	%r15, %rax
	movq	%rax, %rdi
	movq	$0, %rsi
	call	io.printFloat
	popq	%rdi
	popq	%rdx
	movl	$0, %eax
	leave
	ret
strings.float_toString:
float_toString:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$128, %rsp
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	movl	%edx, -20(%rbp)
	pushq	%rdi
	pushq	%rdx
	movq	$0, %rdx
	movq	-8(%rbp), %r15
	movq	%r15, %rdi
	cmpq	%rdx, %rdi
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.Lfloat_toString74
	movl	$-1, %eax
	leave
	ret
.Lfloat_toString74:
	movl	$6, %ebx
	movl	%ebx, -24(%rbp)
	movq	$.strfloat_toString75, %rbx
	movq	%rbx, -32(%rbp)
	movq	-8(%rbp), %rax
	movl	(%rax), %eax
	movl	%eax, %ebx
	movl	%ebx, -36(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	$0, %edx
	movl	-36(%rbp), %r15d
	movl	%r15d, %edi
	cmpl	%edx, %edi
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.Lfloat_toString76
	pushq	%rdx
	pushq	%rdi
	movl	$0, %eax
	movl	%eax, %edi
	call	io.printInt
	popq	%rdi
	popq	%rdx
	movl	$0, %eax
	leave
	ret
.Lfloat_toString76:
	pushq	%rdi
	pushq	%rdx
	movl	$31, %edx
	movl	-36(%rbp), %r15d
	movl	%r15d, %edi
	movl	%edx, %ecx
	sarl	%cl, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -40(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	$23, %edx
	movl	-36(%rbp), %r15d
	movl	%r15d, %edi
	movl	%edx, %ecx
	sarl	%cl, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -44(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	$255, %edx
	movl	-44(%rbp), %r15d
	movl	%r15d, %edi
	andl	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -44(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	$127, %edx
	movl	-44(%rbp), %r15d
	movl	%r15d, %edi
	sub	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -44(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	$8388607, %edx
	movl	-36(%rbp), %r15d
	movl	%r15d, %edi
	andl	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -48(%rbp)
	pushq	%rdx
	pushq	%rdi
	movl	$0, %eax
	movl	%eax, %edi
	call	newBit
	popq	%rdi
	popq	%rdx
	movq	%rax, %rbx
	movq	%rbx, -56(%rbp)
	movq	-56(%rbp), %r15
	movq	%r15, %rbx
	movq	%rbx, -64(%rbp)
	movl	$0, %ebx
	movl	%ebx, -68(%rbp)
	jmp	.Lfloat_toString78
.Lfloat_toString77:
	pushq	%rdx
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movl	-48(%rbp), %r15d
	movl	%r15d, %edi
	andl	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	pushq	%rdi
	movl	%eax, %eax
	movl	%eax, %edi
	call	newBit
	popq	%rdi
	popq	%rdx
	movq	%rax, %rbx
	movq	%rbx, -76(%rbp)
	pushq	%r14
	movq	-64(%rbp), %r14
	movq	-76(%rbp), %r15
	movq	%r15, %rbx
	movq	%rbx, 12(%r14)
	popq	%r14
	pushq	%r14
	movq	-76(%rbp), %r14
	movq	-64(%rbp), %r15
	movq	%r15, %rbx
	movq	%rbx, 4(%r14)
	popq	%r14
	movq	-76(%rbp), %r15
	movq	%r15, %rbx
	movq	%rbx, -64(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movl	-48(%rbp), %r15d
	movl	%r15d, %edi
	movl	%edx, %ecx
	sarl	%cl, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -48(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movl	-68(%rbp), %r15d
	movl	%r15d, %edi
	add	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -68(%rbp)
.Lfloat_toString78:
	pushq	%rdi
	pushq	%rdx
	movl	$23, %edx
	movl	-68(%rbp), %r15d
	movl	%r15d, %edi
	cmpl	%edx, %edi
	setl	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$1, %al
	je	.Lfloat_toString77
	pushq	%rdx
	pushq	%rdi
	movl	$1, %eax
	movl	%eax, %edi
	call	newBit
	popq	%rdi
	popq	%rdx
	movq	%rax, %rbx
	movq	%rbx, -76(%rbp)
	pushq	%r14
	movq	-64(%rbp), %r14
	movq	-76(%rbp), %r15
	movq	%r15, %rbx
	movq	%rbx, 12(%r14)
	popq	%r14
	pushq	%r14
	movq	-76(%rbp), %r14
	movq	-64(%rbp), %r15
	movq	%r15, %rbx
	movq	%rbx, 4(%r14)
	popq	%r14
	movq	-76(%rbp), %r15
	movq	%r15, %rbx
	movq	%rbx, -64(%rbp)
	movq	-56(%rbp), %r15
	movq	%r15, %rbx
	movq	%rbx, -64(%rbp)
	jmp	.Lfloat_toString80
.Lfloat_toString79:
	pushq	%r14
	movq	-64(%rbp), %r14
	movq	12(%r14), %r15
	popq	%r14
	movq	%r15, %rbx
	movq	%rbx, -64(%rbp)
.Lfloat_toString80:
	pushq	%rdi
	pushq	%rdx
	movq	$0, %rdx
	pushq	%r14
	movq	-64(%rbp), %r14
	movq	12(%r14), %r15
	popq	%r14
	movq	%r15, %rdi
	cmpq	%rdx, %rdi
	setne	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$1, %al
	je	.Lfloat_toString79
	pushq	%rdi
	pushq	%rdx
	movl	$0, %edx
	movl	-40(%rbp), %r15d
	movl	%r15d, %edi
	cmpl	%edx, %edi
	setne	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.Lfloat_toString81
	movq	$.strfloat_toString82, %rbx
	movq	%rbx, -32(%rbp)
.Lfloat_toString81:
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
	cmpb	$0, %al
	je	.Lfloat_toString83
	pushq	%rdi
	pushq	%rdx
	movl	$0, %edx
	movl	-40(%rbp), %r15d
	movl	%r15d, %edi
	cmpl	%edx, %edi
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.Lfloat_toString84
	movq	$.strfloat_toString85, %rbx
	movq	%rbx, -32(%rbp)
.Lfloat_toString84:
.Lfloat_toString83:
	movl	-44(%rbp), %r15d
	movl	%r15d, %ebx
	movl	%ebx, -80(%rbp)
	movl	$0, %ebx
	movl	%ebx, -88(%rbp)
	jmp	.Lfloat_toString87
.Lfloat_toString86:
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	pushq	%r14
	movq	-64(%rbp), %r14
	movl	0(%r14), %r15d
	popq	%r14
	movl	%r15d, %edi
	cmpl	%edx, %edi
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.Lfloat_toString88
	pushq	%rdx
	pushq	%rdi
	movl	$2, %eax
	movl	%eax, %edi
	movl	-80(%rbp), %r15d
	pushq	%rsi
	movl	%r15d, %eax
	movl	%eax, %esi
	call	math.exp
	popq	%rsi
	popq	%rdi
	popq	%rdx
	movl	%eax, %ebx
	movl	%ebx, -92(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	-92(%rbp), %r15d
	movl	%r15d, %edx
	movl	-84(%rbp), %r15d
	movl	%r15d, %edi
	add	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -84(%rbp)
.Lfloat_toString88:
	pushq	%r14
	movq	-64(%rbp), %r14
	movq	4(%r14), %r15
	popq	%r14
	movq	%r15, %rbx
	movq	%rbx, -64(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movl	-80(%rbp), %r15d
	movl	%r15d, %edi
	sub	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -80(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movl	-88(%rbp), %r15d
	movl	%r15d, %edi
	add	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -88(%rbp)
.Lfloat_toString87:
	pushq	%rdi
	pushq	%rdx
	movl	$-1, %edx
	movl	-80(%rbp), %r15d
	movl	%r15d, %edi
	cmpl	%edx, %edi
	setg	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$1, %al
	je	.Lfloat_toString86
	pushq	%rdx
	pushq	%rdi
	movl	$10, %eax
	movl	%eax, %edi
	movl	-24(%rbp), %r15d
	pushq	%rsi
	movl	%r15d, %eax
	movl	%eax, %esi
	call	math.exp
	popq	%rsi
	popq	%rdi
	popq	%rdx
	movl	%eax, %ebx
	movl	%ebx, -80(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	$5, %edx
	movl	-80(%rbp), %r15d
	movl	%r15d, %edi
	imul	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -80(%rbp)
	movl	$2, %ebx
	movl	%ebx, -92(%rbp)
	movl	$0, %ebx
	movl	%ebx, -96(%rbp)
	jmp	.Lfloat_toString90
.Lfloat_toString89:
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	pushq	%r14
	movq	-64(%rbp), %r14
	movl	0(%r14), %r15d
	popq	%r14
	movl	%r15d, %edi
	cmpl	%edx, %edi
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.Lfloat_toString91
	pushq	%rdi
	pushq	%rdx
	movl	-80(%rbp), %r15d
	movl	%r15d, %edx
	movl	-96(%rbp), %r15d
	movl	%r15d, %edi
	add	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -96(%rbp)
.Lfloat_toString91:
	pushq	%r14
	movq	-64(%rbp), %r14
	movq	4(%r14), %r15
	popq	%r14
	movq	%r15, %rbx
	movq	%rbx, -64(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	-80(%rbp), %r15d
	movl	%r15d, %eax
	movl	-92(%rbp), %r15d
	movl	%r15d, %ecx
	cltd
	idiv	%ecx
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -80(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movl	-88(%rbp), %r15d
	movl	%r15d, %edi
	add	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -88(%rbp)
.Lfloat_toString90:
	pushq	%rdi
	pushq	%rdx
	movl	$23, %edx
	movl	-88(%rbp), %r15d
	movl	%r15d, %edi
	cmpl	%edx, %edi
	setl	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$1, %al
	je	.Lfloat_toString89
	lea	-32(%rbp), %rax
	movq	$1, %rdi
	movq	%rax, %rsi
	movq	$1, %rdx
	movq	$1, %rax
	syscall	
	pushq	%rdx
	movl	-84(%rbp), %r15d
	pushq	%rdi
	movl	%r15d, %eax
	movl	%eax, %edi
	call	io.printInt
	popq	%rdi
	popq	%rdx
	pushq	%rdx
	pushq	%rdi
	movq	$.strfloat_toString92, %rax
	movq	%rax, %rdi
	movq	$0, %rsi
	call	io.print
	popq	%rdi
	popq	%rdx
	pushq	%rdx
	movl	-96(%rbp), %r15d
	pushq	%rdi
	movl	%r15d, %eax
	movl	%eax, %edi
	call	io.printInt
	popq	%rdi
	popq	%rdx
	lea	-106(%rbp), %rax
	movq	%rax, %rbx
	movq	%rbx, -114(%rbp)
	pushq	%rdx
	movl	-84(%rbp), %r15d
	pushq	%rdi
	movl	%r15d, %eax
	movl	%eax, %edi
	movq	-114(%rbp), %r15
	pushq	%rsi
	movq	%r15, %rax
	movq	%rax, %rsi
	call	int_toString
	popq	%rsi
	popq	%rdi
	popq	%rdx
	pushq	%rdx
	movq	-32(%rbp), %r15
	pushq	%rdi
	movq	%r15, %rax
	movq	%rax, %rdi
	movq	-114(%rbp), %r15
	pushq	%rsi
	movq	%r15, %rax
	movq	%rax, %rsi
	movq	-16(%rbp), %r15
	pushq	%rdx
	movq	%r15, %rax
	movq	%rax, %rdx
	call	str_concat
	popq	%rdx
	popq	%rsi
	popq	%rdi
	popq	%rdx
	pushq	%rdx
	movq	-16(%rbp), %r15
	pushq	%rdi
	movq	%r15, %rax
	movq	%rax, %rdi
	pushq	%rsi
	movq	$.strfloat_toString93, %rax
	movq	%rax, %rsi
	movq	-16(%rbp), %r15
	pushq	%rdx
	movq	%r15, %rax
	movq	%rax, %rdx
	call	str_concat
	popq	%rdx
	popq	%rsi
	popq	%rdi
	popq	%rdx
	pushq	%rdx
	movl	-96(%rbp), %r15d
	pushq	%rdi
	movl	%r15d, %eax
	movl	%eax, %edi
	movq	-114(%rbp), %r15
	pushq	%rsi
	movq	%r15, %rax
	movq	%rax, %rsi
	call	int_toString
	popq	%rsi
	popq	%rdi
	popq	%rdx
	pushq	%rdx
	movq	-16(%rbp), %r15
	pushq	%rdi
	movq	%r15, %rax
	movq	%rax, %rdi
	movq	-114(%rbp), %r15
	pushq	%rsi
	movq	%r15, %rax
	movq	%rax, %rsi
	movq	-16(%rbp), %r15
	pushq	%rdx
	movq	%r15, %rax
	movq	%rax, %rdx
	call	str_concat
	popq	%rdx
	popq	%rsi
	popq	%rdi
	popq	%rdx
	movl	$0, %eax
	leave
	ret
strings.str_subString:
str_subString:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$64, %rsp
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	movq	%rdx, -16(%rbp)
	movq	%rcx, -24(%rbp)
	movq	$0, %rbx
	movq	%rbx, -32(%rbp)
	lea	-4(%rbp), %rax
	movq	%rax, %rbx
	movq	%rbx, -32(%rbp)
	movq	-32(%rbp), %rax
	movl	(%rax), %eax
	movl	%eax, %ebx
	movl	%ebx, -36(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	-36(%rbp), %r15d
	movl	%r15d, %edx
	movq	-16(%rbp), %r15
	movq	%r15, %rdi
	add	%rdx, %rdi
	movq	%rdi, %rax
	popq	%rdx
	popq	%rdi
	movq	%rax, %rbx
	movq	%rbx, -16(%rbp)
	movq	-24(%rbp), %r15
	movq	%r15, %rbx
	movq	%rbx, -44(%rbp)
	movl	$0, %ebx
	movl	%ebx, -48(%rbp)
	jmp	.Lstr_subString95
.Lstr_subString94:
	movq	-16(%rbp), %rax
	movb	(%rax), %al
	movb	%al, %bl
	movb	%bl, -49(%rbp)
	movb	-49(%rbp), %r15b
	movq	-44(%rbp), %rax
	movb	%r15b, %bl
	movb	%bl, (%rax)
	pushq	%rdi
	pushq	%rdx
	movq	$1, %rdx
	movq	-44(%rbp), %r15
	movq	%r15, %rdi
	add	%rdx, %rdi
	movq	%rdi, %rax
	popq	%rdx
	popq	%rdi
	movq	%rax, %rbx
	movq	%rbx, -44(%rbp)
	pushq	%rdi
	pushq	%rdx
	movq	$1, %rdx
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
	movl	-48(%rbp), %r15d
	movl	%r15d, %edi
	add	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -48(%rbp)
.Lstr_subString95:
	pushq	%rdi
	pushq	%rdx
	movl	-8(%rbp), %r15d
	movl	%r15d, %edx
	movl	-48(%rbp), %r15d
	movl	%r15d, %edi
	cmpl	%edx, %edi
	setl	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$1, %al
	je	.Lstr_subString94
	movq	-44(%rbp), %rax
	movb	$0, %bl
	movb	%bl, (%rax)
	movq	-24(%rbp), %r15
	movq	%r15, %rax
	leave
	ret
strings.str_reverse:
str_reverse:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$48, %rsp
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	pushq	%rdx
	movq	-8(%rbp), %r15
	pushq	%rdi
	movq	%r15, %rax
	movq	%rax, %rdi
	call	len
	popq	%rdi
	popq	%rdx
	movl	%eax, %ebx
	movl	%ebx, -20(%rbp)
	movq	-16(%rbp), %r15
	movq	%r15, %rbx
	movq	%rbx, -28(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movl	-20(%rbp), %r15d
	movl	%r15d, %edi
	sub	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -32(%rbp)
	jmp	.Lstr_reverse97
.Lstr_reverse96:
	pushq	%rdi
	pushq	%rdx
	movl	-32(%rbp), %r15d
	movl	%r15d, %edx
	movq	-8(%rbp), %r15
	movq	%r15, %rdi
	add	%rdx, %rdi
	movq	%rdi, %rax
	popq	%rdx
	popq	%rdi
	movq	%rax, %rbx
	movq	%rbx, -40(%rbp)
	movq	-40(%rbp), %rax
	movb	(%rax), %al
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
	movl	-32(%rbp), %r15d
	movl	%r15d, %edi
	sub	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -32(%rbp)
.Lstr_reverse97:
	pushq	%rdi
	pushq	%rdx
	movl	$-1, %edx
	movl	-32(%rbp), %r15d
	movl	%r15d, %edi
	cmpl	%edx, %edi
	setg	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$1, %al
	je	.Lstr_reverse96
	movq	-16(%rbp), %rax
	movb	$0, %bl
	movb	%bl, (%rax)
	movq	-28(%rbp), %r15
	movq	%r15, %rax
	leave
	ret
strings.str_at:
str_at:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$16, %rsp
	movq	%rdi, -8(%rbp)
	movl	%esi, -12(%rbp)
	movl	$0, %ebx
	movl	%ebx, -16(%rbp)
	jmp	.Lstr_at99
.Lstr_at98:
	pushq	%rdi
	pushq	%rdx
	movq	$1, %rdx
	movq	-8(%rbp), %r15
	movq	%r15, %rdi
	add	%rdx, %rdi
	movq	%rdi, %rax
	popq	%rdx
	popq	%rdi
	movq	%rax, %rbx
	movq	%rbx, -8(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movl	-16(%rbp), %r15d
	movl	%r15d, %edi
	add	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -16(%rbp)
.Lstr_at99:
	pushq	%rdi
	pushq	%rdx
	movl	-12(%rbp), %r15d
	movl	%r15d, %edx
	movl	-16(%rbp), %r15d
	movl	%r15d, %edi
	cmpl	%edx, %edi
	setl	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$1, %al
	je	.Lstr_at98
	movq	-8(%rbp), %rax
	movb	(%rax), %al
	movb	%al, %bl
	movb	%bl, -13(%rbp)
	movb	-13(%rbp), %r15b
	movb	%r15b, %al
	leave
	ret
strings.str_forEach:
str_forEach:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$48, %rsp
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	movq	%rdx, -24(%rbp)
	pushq	%rdx
	movq	-8(%rbp), %r15
	pushq	%rdi
	movq	%r15, %rax
	movq	%rax, %rdi
	call	len
	popq	%rdi
	popq	%rdx
	movl	%eax, %ebx
	movl	%ebx, -28(%rbp)
	movl	$0, %ebx
	movl	%ebx, -32(%rbp)
	jmp	.Lstr_forEach101
.Lstr_forEach100:
	pushq	%rdx
	movq	-8(%rbp), %r15
	pushq	%rdi
	movq	%r15, %rax
	movq	%rax, %rdi
	movl	-32(%rbp), %r15d
	pushq	%rsi
	movl	%r15d, %eax
	movl	%eax, %esi
	call	str_at
	popq	%rsi
	popq	%rdi
	popq	%rdx
	movb	%al, %bl
	movb	%bl, -33(%rbp)
	pushq	%rdx
	movq	-16(%rbp), %r15
	movq	%r15, %r11
	movb	-33(%rbp), %r15b
	pushq	%rdi
	movb	%r15b, %al
	movb	%al, %dil
	movl	-32(%rbp), %r15d
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
	movl	%ebx, -37(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movl	-37(%rbp), %r15d
	movl	%r15d, %edi
	cmpl	%edx, %edi
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.Lstr_forEach102
	movl	-37(%rbp), %r15d
	movl	%r15d, %eax
	leave
	ret
.Lstr_forEach102:
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
.Lstr_forEach101:
	pushq	%rdi
	pushq	%rdx
	movl	-28(%rbp), %r15d
	movl	%r15d, %edx
	movl	-32(%rbp), %r15d
	movl	%r15d, %edi
	cmpl	%edx, %edi
	setl	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$1, %al
	je	.Lstr_forEach100
	movl	$0, %eax
	leave
	ret
strings.char_isDigit:
char_isDigit:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$16, %rsp
	movb	%dil, -1(%rbp)
	pushq	%rdi
	pushq	%rdx
	pushq	%rdi
	pushq	%rdx
	movb	$57, %dl
	movb	-1(%rbp), %r15b
	movb	%r15b, %dil
	cmpb	%dl, %dil
	setle	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %dl
	pushq	%rdi
	pushq	%rdx
	movb	$48, %dl
	movb	-1(%rbp), %r15b
	movb	%r15b, %dil
	cmpb	%dl, %dil
	setge	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %dil
	andb	%dl, %dil
	movb	%dil, %al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	leave
	ret
strings.hex_toInt:
hex_toInt:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$32, %rsp
	movq	%rdi, -8(%rbp)
	movl	$0, %ebx
	movl	%ebx, -12(%rbp)
	movl	$0, %ebx
	movl	%ebx, -16(%rbp)
	movq	-8(%rbp), %rax
	movb	(%rax), %al
	movb	%al, %bl
	movb	%bl, -17(%rbp)
	jmp	.Lhex_toInt104
.Lhex_toInt103:
	pushq	%rdx
	movb	-17(%rbp), %r15b
	pushq	%rdi
	movb	%r15b, %al
	movb	%al, %dil
	call	io.toInt
	popq	%rdi
	popq	%rdx
	movl	%eax, %ebx
	movl	%ebx, -21(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	$16, %edx
	movl	-12(%rbp), %r15d
	movl	%r15d, %edi
	imul	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -25(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	-25(%rbp), %r15d
	movl	%r15d, %edx
	movl	-21(%rbp), %r15d
	movl	%r15d, %edi
	add	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -12(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movq	-8(%rbp), %r15
	movq	%r15, %rdi
	add	%rdx, %rdi
	movq	%rdi, %rax
	popq	%rdx
	popq	%rdi
	movq	%rax, %rbx
	movq	%rbx, -8(%rbp)
	movq	-8(%rbp), %rax
	movb	(%rax), %al
	movb	%al, %bl
	movb	%bl, -17(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movl	-16(%rbp), %r15d
	movl	%r15d, %edi
	add	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -16(%rbp)
.Lhex_toInt104:
	pushq	%rdi
	pushq	%rdx
	movb	$0, %dl
	movb	-17(%rbp), %r15b
	movb	%r15b, %dil
	cmpb	%dl, %dil
	setne	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$1, %al
	je	.Lhex_toInt103
	movl	-12(%rbp), %r15d
	movl	%r15d, %eax
	leave
	ret


.data

.strfloat_toString93:
	.asciz	 "."
.strfloat_toString92:
	.asciz	 "."
.strfloat_toString85:
	.asciz	 "+"
.strfloat_toString82:
	.asciz	 "-"
.strfloat_toString75:
	.asciz	 ""
.floatstr_toFloat73:
	.float	0.0
.floatstr_toFloat72:
	.float	0.00
.floatstr_toFloat70:
	.float	0.1
.floatstr_toFloat66:
	.float	10.0
.floatstr_toFloat65:
	.float	0.00
.floatstr_toFloat64:
	.float	10.0
.floatstr_toFloat61:
	.float	1.0
.floatstr_toFloat60:
	.float	0.00
.floatcharToFloat59:
	.float	9.00
.floatcharToFloat57:
	.float	8.00
.floatcharToFloat55:
	.float	7.00
.floatcharToFloat53:
	.float	6.00
.floatcharToFloat51:
	.float	5.00
.floatcharToFloat49:
	.float	4.00
.floatcharToFloat47:
	.float	3.00
.floatcharToFloat45:
	.float	2.00
.floatcharToFloat43:
	.float	1.00
.floatcharToFloat41:
	.float	0.00
.floatcharToFloat39:
	.float	0.00
.strstr_split38:
	.asciz	 ""


.bss

