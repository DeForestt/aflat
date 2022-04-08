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
	movl	-13(%rbp), %edi
	add	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -13(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movq	-8(%rbp), %rdi
	add	%rdx, %rdi
	movq	%rdi, %rax
	popq	%rdx
	popq	%rdi
	movq	%rax, %rbx
	movq	%rbx, -8(%rbp)
.Llen1:
	pushq	%rdi
	pushq	%rdx
	movb	-9(%rbp), %dl
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
	movl	-13(%rbp), %eax
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
	pushq	%rdi
	movq	-8(%rbp), %rax
	movq	%rax, %rdi
	call	len
	popq	%rdi
	popq	%rdx
	movl	%eax, %ebx
	movl	%ebx, -20(%rbp)
	pushq	%rdx
	pushq	%rdi
	movq	-16(%rbp), %rax
	movq	%rax, %rdi
	call	len
	popq	%rdi
	popq	%rdx
	movl	%eax, %ebx
	movl	%ebx, -24(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	-24(%rbp), %edx
	movl	-20(%rbp), %edi
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
	movl	-28(%rbp), %edi
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
	movb	-30(%rbp), %dl
	movb	-29(%rbp), %dil
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
	movq	-8(%rbp), %rdi
	add	%rdx, %rdi
	movq	%rdi, %rax
	popq	%rdx
	popq	%rdi
	movq	%rax, %rbx
	movq	%rbx, -8(%rbp)
.Lstr_comp4:
	pushq	%rdi
	pushq	%rdx
	movl	-20(%rbp), %edx
	movl	-28(%rbp), %edi
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
	movq	-24(%rbp), %rax
	movb	-26(%rbp), %bl
	movb	%bl, (%rax)
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movq	-8(%rbp), %rdi
	add	%rdx, %rdi
	movq	%rdi, %rax
	popq	%rdx
	popq	%rdi
	movq	%rax, %rbx
	movq	%rbx, -8(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movq	-24(%rbp), %rdi
	add	%rdx, %rdi
	movq	%rdi, %rax
	popq	%rdx
	popq	%rdi
	movq	%rax, %rbx
	movq	%rbx, -24(%rbp)
.Lstr_concat7:
	pushq	%rdi
	pushq	%rdx
	movb	-25(%rbp), %dl
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
	movq	-24(%rbp), %rax
	movb	-26(%rbp), %bl
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
	movq	-24(%rbp), %rdi
	add	%rdx, %rdi
	movq	%rdi, %rax
	popq	%rdx
	popq	%rdi
	movq	%rax, %rbx
	movq	%rbx, -24(%rbp)
.Lstr_concat9:
	pushq	%rdi
	pushq	%rdx
	movb	-25(%rbp), %dl
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
	movq	-24(%rbp), %rax
	movb	-25(%rbp), %bl
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
	movb	-13(%rbp), %dil
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
	movq	-8(%rbp), %rdi
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
	pushq	%rdi
	movq	-8(%rbp), %rax
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
	movl	-22(%rbp), %edi
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
	pushq	%rsi
	movl	-30(%rbp), %eax
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
	pushq	%rdi
	movb	-31(%rbp), %al
	movb	%al, %dil
	call	io.toInt
	popq	%rdi
	popq	%rdx
	movl	%eax, %ebx
	movl	%ebx, -35(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	-35(%rbp), %edx
	movl	-18(%rbp), %edi
	imul	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -18(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	-18(%rbp), %edx
	movl	-26(%rbp), %edi
	add	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -26(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movl	-30(%rbp), %edi
	sub	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -30(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movq	-8(%rbp), %rdi
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
	movl	-30(%rbp), %edi
	cmpl	%edx, %edi
	setg	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$1, %al
	je	.Lstr_toInt11
	pushq	%rdi
	pushq	%rdx
	movl	-12(%rbp), %edx
	movl	-26(%rbp), %edi
	imul	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -26(%rbp)
	movl	-26(%rbp), %eax
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
	movl	-4(%rbp), %edi
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
	movq	-12(%rbp), %rax
	movb	-29(%rbp), %bl
	movb	%bl, (%rax)
	pushq	%rdi
	pushq	%rdx
	movq	$1, %rdx
	movq	-12(%rbp), %rdi
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
	movl	-4(%rbp), %edi
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
	movq	-12(%rbp), %rdi
	add	%rdx, %rdi
	movq	%rdi, %rax
	popq	%rdx
	popq	%rdi
	movq	%rax, %rbx
	movq	%rbx, -12(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	$-1, %edx
	movl	-4(%rbp), %edi
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
	movl	-4(%rbp), %edi
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
	movl	-16(%rbp), %eax
	movl	-20(%rbp), %ecx
	cltd
	idiv	%ecx
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -16(%rbp)
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
.Lint_toString16:
	pushq	%rdi
	pushq	%rdx
	movl	$0, %edx
	movl	-16(%rbp), %edi
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
	movl	-28(%rbp), %edi
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
	pushq	%rsi
	movl	-28(%rbp), %eax
	movl	%eax, %esi
	call	math.exp
	popq	%rsi
	popq	%rdi
	popq	%rdx
	movl	%eax, %ebx
	movl	%ebx, -24(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	-4(%rbp), %eax
	movl	-24(%rbp), %ecx
	cltd
	idiv	%ecx
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -16(%rbp)
	pushq	%rdx
	pushq	%rdi
	movl	-16(%rbp), %eax
	movl	%eax, %edi
	call	io.toChar
	popq	%rdi
	popq	%rdx
	movb	%al, %bl
	movb	%bl, -30(%rbp)
	movq	-12(%rbp), %rax
	movb	-30(%rbp), %bl
	movb	%bl, (%rax)
	pushq	%rdi
	pushq	%rdx
	movq	$1, %rdx
	movq	-12(%rbp), %rdi
	add	%rdx, %rdi
	movq	%rdi, %rax
	popq	%rdx
	popq	%rdi
	movq	%rax, %rbx
	movq	%rbx, -12(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	-24(%rbp), %edx
	movl	-16(%rbp), %edi
	imul	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -16(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	-16(%rbp), %edx
	movl	-4(%rbp), %edi
	sub	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -4(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movl	-28(%rbp), %edi
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
	movl	-28(%rbp), %edi
	cmpl	%edx, %edi
	setg	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$1, %al
	je	.Lint_toString17
	movq	-12(%rbp), %rax
	movb	-29(%rbp), %bl
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
	movl	-4(%rbp), %edi
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
	movl	-20(%rbp), %eax
	movl	-24(%rbp), %ecx
	cltd
	idiv	%ecx
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -20(%rbp)
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
.Lint_toStringL020:
	pushq	%rdi
	pushq	%rdx
	movl	$0, %edx
	movl	-20(%rbp), %edi
	cmpl	%edx, %edi
	setne	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$1, %al
	je	.Lint_toStringL019
	pushq	%rdi
	pushq	%rdx
	movl	-32(%rbp), %edx
	movl	-16(%rbp), %edi
	sub	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -37(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	$0, %edx
	movl	-37(%rbp), %edi
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
	movq	-12(%rbp), %rdi
	add	%rdx, %rdi
	movq	%rdi, %rax
	popq	%rdx
	popq	%rdi
	movq	%rax, %rbx
	movq	%rbx, -12(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movl	-37(%rbp), %edi
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
	movl	-37(%rbp), %edi
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
	movl	-4(%rbp), %edi
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
	movq	-12(%rbp), %rax
	movb	-33(%rbp), %bl
	movb	%bl, (%rax)
	pushq	%rdi
	pushq	%rdx
	movq	$1, %rdx
	movq	-12(%rbp), %rdi
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
	movl	-4(%rbp), %edi
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
	movq	-12(%rbp), %rdi
	add	%rdx, %rdi
	movq	%rdi, %rax
	popq	%rdx
	popq	%rdi
	movq	%rax, %rbx
	movq	%rbx, -12(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	$-1, %edx
	movl	-4(%rbp), %edi
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
	movl	-4(%rbp), %edi
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
	movl	-20(%rbp), %eax
	movl	-24(%rbp), %ecx
	cltd
	idiv	%ecx
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -20(%rbp)
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
.Lint_toStringL027:
	pushq	%rdi
	pushq	%rdx
	movl	$0, %edx
	movl	-20(%rbp), %edi
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
	movl	-32(%rbp), %edi
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
	pushq	%rsi
	movl	-32(%rbp), %eax
	movl	%eax, %esi
	call	math.exp
	popq	%rsi
	popq	%rdi
	popq	%rdx
	movl	%eax, %ebx
	movl	%ebx, -28(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	-4(%rbp), %eax
	movl	-28(%rbp), %ecx
	cltd
	idiv	%ecx
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -20(%rbp)
	pushq	%rdx
	pushq	%rdi
	movl	-20(%rbp), %eax
	movl	%eax, %edi
	call	io.toChar
	popq	%rdi
	popq	%rdx
	movb	%al, %bl
	movb	%bl, -38(%rbp)
	movq	-12(%rbp), %rax
	movb	-38(%rbp), %bl
	movb	%bl, (%rax)
	pushq	%rdi
	pushq	%rdx
	movq	$1, %rdx
	movq	-12(%rbp), %rdi
	add	%rdx, %rdi
	movq	%rdi, %rax
	popq	%rdx
	popq	%rdi
	movq	%rax, %rbx
	movq	%rbx, -12(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	-28(%rbp), %edx
	movl	-20(%rbp), %edi
	imul	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -20(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	-20(%rbp), %edx
	movl	-4(%rbp), %edi
	sub	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -4(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movl	-32(%rbp), %edi
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
	movl	-32(%rbp), %edi
	cmpl	%edx, %edi
	setg	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$1, %al
	je	.Lint_toStringL028
	movq	-12(%rbp), %rax
	movb	-33(%rbp), %bl
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
	pushq	%rdi
	movq	-8(%rbp), %rax
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
	movq	-8(%rbp), %rdi
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
	movb	-9(%rbp), %dl
	movb	-18(%rbp), %dil
	cmpb	%dl, %dil
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.Lstr_find32
	movl	-13(%rbp), %eax
	leave
	ret
.Lstr_find32:
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movl	-13(%rbp), %edi
	add	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -13(%rbp)
.Lstr_find31:
	pushq	%rdi
	pushq	%rdx
	movl	-17(%rbp), %edx
	movl	-13(%rbp), %edi
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
	movb	-26(%rbp), %dl
	movb	-27(%rbp), %dil
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
	movb	-9(%rbp), %dl
	movb	-27(%rbp), %dil
	cmpb	%dl, %dil
	setne	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.Lstr_split36
	movq	-17(%rbp), %rax
	movb	-27(%rbp), %bl
	movb	%bl, (%rax)
.Lstr_split36:
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movq	-8(%rbp), %rdi
	add	%rdx, %rdi
	movq	%rdi, %rax
	popq	%rdx
	popq	%rdi
	movq	%rax, %rbx
	movq	%rbx, -8(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movq	-17(%rbp), %rdi
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
	movb	-9(%rbp), %dl
	movb	-27(%rbp), %dil
	cmpb	%dl, %dil
	setne	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$1, %al
	je	.Lstr_split33
	movq	-17(%rbp), %rax
	movb	-26(%rbp), %bl
	movb	%bl, (%rax)
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movq	-8(%rbp), %rdi
	add	%rdx, %rdi
	movq	%rdi, %rax
	popq	%rdx
	popq	%rdi
	movq	%rax, %rbx
	movq	%rbx, -8(%rbp)
	pushq	%rdi
	pushq	%rdx
	movb	-26(%rbp), %dl
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
	pushq	%rsi
	movq	-8(%rbp), %rax
	movq	%rax, %rsi
	pushq	%rdx
	movq	-25(%rbp), %rax
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
	movb	-1(%rbp), %dil
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
	movb	-1(%rbp), %dil
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
	movb	-1(%rbp), %dil
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
	movb	-1(%rbp), %dil
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
	movb	-1(%rbp), %dil
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
	movb	-1(%rbp), %dil
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
	movb	-1(%rbp), %dil
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
	movb	-1(%rbp), %dil
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
	movb	-1(%rbp), %dil
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
	movb	-1(%rbp), %dil
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
	movl	-5(%rbp), %eax
	leave
	ret
strings.str_toFloat:
str_toFloat:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$96, %rsp
	movq	%rdi, -8(%rbp)
	pushq	%rdx
	pushq	%rdi
	movq	-8(%rbp), %rax
	movq	%rax, %rdi
	pushq	%rsi
	movb	$46, %al
	movb	%al, %sil
	lea	-28(%rbp), %rax
	pushq	%rdx
	movq	%rax, %rax
	movq	%rax, %rdx
	lea	-48(%rbp), %rax
	pushq	%rcx
	movq	%rax, %rax
	movq	%rax, %rcx
	call	str_split
	popq	%rcx
	popq	%rdx
	popq	%rsi
	popq	%rdi
	popq	%rdx
	pushq	%rdx
	lea	-28(%rbp), %rax
	pushq	%rdi
	movq	%rax, %rax
	movq	%rax, %rdi
	call	len
	popq	%rdi
	popq	%rdx
	movl	%eax, %ebx
	movl	%ebx, -52(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movl	-52(%rbp), %edi
	sub	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -52(%rbp)
	movss	.floatstr_toFloat60, %xmm0
	movss	%xmm0, %xmm0
	movss	%xmm0, -56(%rbp)
	movl	$0, %ebx
	movl	%ebx, -60(%rbp)
	movss	.floatstr_toFloat61, %xmm0
	movss	%xmm0, %xmm0
	movss	%xmm0, -64(%rbp)
	jmp	.Lstr_toFloat63
.Lstr_toFloat62:
	pushq	%rdi
	pushq	%rdx
	movss	.floatstr_toFloat64, %xmm0
	movss	%xmm0, %xmm1
	movss	-64(%rbp), %xmm0
	mulss	%xmm1, %xmm0
	movss	%xmm0, %xmm2
	popq	%rdx
	popq	%rdi
	movss	%xmm2, %xmm0
	movss	%xmm0, -64(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movl	-60(%rbp), %edi
	add	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -60(%rbp)
.Lstr_toFloat63:
	pushq	%rdi
	pushq	%rdx
	movl	-52(%rbp), %edx
	movl	-60(%rbp), %edi
	cmpl	%edx, %edi
	setl	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$1, %al
	je	.Lstr_toFloat62
	movl	$0, %ebx
	movl	%ebx, -60(%rbp)
	movss	.floatstr_toFloat65, %xmm0
	movss	%xmm0, %xmm0
	movss	%xmm0, -68(%rbp)
	pushq	%rdx
	pushq	%rdi
	movl	$0, %eax
	movl	%eax, %edi
	call	ascii
	popq	%rdi
	popq	%rdx
	movb	%al, %bl
	movb	%bl, -69(%rbp)
	movq	-8(%rbp), %rax
	movb	(%rax), %al
	movb	%al, %bl
	movb	%bl, -70(%rbp)
	movss	.floatstr_toFloat66, %xmm0
	movss	%xmm0, %xmm0
	movss	%xmm0, -74(%rbp)
	jmp	.Lstr_toFloat68
.Lstr_toFloat67:
	pushq	%rdi
	pushq	%rdx
	movb	$46, %dl
	movb	-70(%rbp), %dil
	cmpb	%dl, %dil
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.Lstr_toFloat69
	movss	.floatstr_toFloat70, %xmm0
	movss	%xmm0, %xmm0
	movss	%xmm0, -64(%rbp)
.Lstr_toFloat69:
	pushq	%rdi
	pushq	%rdx
	movb	$46, %dl
	movb	-70(%rbp), %dil
	cmpb	%dl, %dil
	setne	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.Lstr_toFloat71
	pushq	%rdx
	pushq	%rdi
	movb	-70(%rbp), %al
	movb	%al, %dil
	call	charToFloat
	popq	%rdi
	popq	%rdx
	movl	%eax, %ebx
	movl	%ebx, -68(%rbp)
	pushq	%rdi
	pushq	%rdx
	movss	.floatstr_toFloat72, %xmm0
	movss	%xmm0, %xmm1
	movss	-56(%rbp), %xmm0
	addss	%xmm1, %xmm0
	movss	%xmm0, %xmm2
	popq	%rdx
	popq	%rdi
	movss	%xmm2, %xmm0
	movss	%xmm0, -78(%rbp)
	pushq	%rdi
	pushq	%rdx
	movss	-64(%rbp), %xmm1
	movss	-68(%rbp), %xmm0
	mulss	%xmm1, %xmm0
	movss	%xmm0, %xmm2
	popq	%rdx
	popq	%rdi
	movss	%xmm2, %xmm0
	movss	%xmm0, -56(%rbp)
	pushq	%rdi
	pushq	%rdx
	movss	-78(%rbp), %xmm1
	movss	-56(%rbp), %xmm0
	addss	%xmm1, %xmm0
	movss	%xmm0, %xmm2
	popq	%rdx
	popq	%rdi
	movss	%xmm2, %xmm0
	movss	%xmm0, -56(%rbp)
	pushq	%rdi
	pushq	%rdx
	movss	-74(%rbp), %xmm1
	movss	-64(%rbp), %xmm0
	divss	%xmm1, %xmm0
	popq	%rdx
	popq	%rdi
	movss	%xmm0, %xmm0
	movss	%xmm0, -64(%rbp)
.Lstr_toFloat71:
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movq	-8(%rbp), %rdi
	add	%rdx, %rdi
	movq	%rdi, %rax
	popq	%rdx
	popq	%rdi
	movq	%rax, %rbx
	movq	%rbx, -8(%rbp)
	movq	-8(%rbp), %rax
	movb	(%rax), %al
	movb	%al, %bl
	movb	%bl, -70(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movl	-60(%rbp), %edi
	add	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -60(%rbp)
.Lstr_toFloat68:
	pushq	%rdi
	pushq	%rdx
	movb	-69(%rbp), %dl
	movb	-70(%rbp), %dil
	cmpb	%dl, %dil
	setne	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$1, %al
	je	.Lstr_toFloat67
	lea	-56(%rbp), %rax
	movq	%rax, %rbx
	movq	%rbx, -82(%rbp)
	movss	.floatstr_toFloat73, %xmm0
	movss	%xmm0, %xmm0
	movss	%xmm0, -86(%rbp)
	pushq	%rdi
	pushq	%rdx
	movss	-86(%rbp), %xmm1
	movss	-86(%rbp), %xmm0
	mulss	%xmm1, %xmm0
	movss	%xmm0, %xmm2
	popq	%rdx
	popq	%rdi
	movss	%xmm2, %xmm0
	movss	%xmm0, -86(%rbp)
	movq	-82(%rbp), %rax
	movl	(%rax), %eax
	movl	%eax, %ebx
	movl	%ebx, -90(%rbp)
	movl	-90(%rbp), %eax
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
	pushq	%rdi
	movq	-8(%rbp), %rax
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
	subq	$112, %rsp
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	movl	%edx, -20(%rbp)
	pushq	%rdi
	pushq	%rdx
	movq	$0, %rdx
	movq	-8(%rbp), %rdi
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
	movl	-36(%rbp), %edi
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
	movl	-36(%rbp), %edi
	movl	%edi, %ecx
	sarl	%cl, %edi
	popq	%rdx
	popq	%rdi
	movl	%edi, %ebx
	movl	%ebx, -40(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	$23, %edx
	movl	-36(%rbp), %edi
	movl	%edi, %ecx
	sarl	%cl, %edi
	popq	%rdx
	popq	%rdi
	movl	%edi, %ebx
	movl	%ebx, -44(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	$255, %edx
	movl	-44(%rbp), %edi
	andl	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -44(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	$127, %edx
	movl	-44(%rbp), %edi
	sub	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -44(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	$8388607, %edx
	movl	-36(%rbp), %edi
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
	movq	-56(%rbp), %rbx
	movq	%rbx, -64(%rbp)
	movl	$0, %ebx
	movl	%ebx, -68(%rbp)
	jmp	.Lfloat_toString78
.Lfloat_toString77:
	pushq	%rdx
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movl	-48(%rbp), %edi
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
	movq	-64(%rbp), %rdx
	movq	-76(%rbp), %rbx
	movq	%rbx, 12(%rdx)
	movq	-76(%rbp), %rdx
	movq	-64(%rbp), %rbx
	movq	%rbx, 4(%rdx)
	movq	-76(%rbp), %rbx
	movq	%rbx, -64(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movl	-48(%rbp), %edi
	movl	%edi, %ecx
	sarl	%cl, %edi
	popq	%rdx
	popq	%rdi
	movl	%edi, %ebx
	movl	%ebx, -48(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movl	-68(%rbp), %edi
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
	movl	-68(%rbp), %edi
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
	movq	-64(%rbp), %rdx
	movq	-76(%rbp), %rbx
	movq	%rbx, 12(%rdx)
	movq	-76(%rbp), %rdx
	movq	-64(%rbp), %rbx
	movq	%rbx, 4(%rdx)
	movq	-76(%rbp), %rbx
	movq	%rbx, -64(%rbp)
	movq	-56(%rbp), %rbx
	movq	%rbx, -64(%rbp)
	jmp	.Lfloat_toString80
.Lfloat_toString79:
	movq	-64(%rbp), %r14
	movq	12(%r14), %rbx
	movq	%rbx, -64(%rbp)
.Lfloat_toString80:
	pushq	%rdi
	pushq	%rdx
	movq	$0, %rdx
	movq	-64(%rbp), %r14
	movq	12(%r14), %rdi
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
	movl	-40(%rbp), %edi
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
	movl	-20(%rbp), %edi
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
	movl	-40(%rbp), %edi
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
	movl	-44(%rbp), %ebx
	movl	%ebx, -80(%rbp)
	movl	$0, %ebx
	movl	%ebx, -88(%rbp)
	jmp	.Lfloat_toString87
.Lfloat_toString86:
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movq	-64(%rbp), %r14
	movl	0(%r14), %edi
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
	pushq	%rsi
	movl	-80(%rbp), %eax
	movl	%eax, %esi
	call	math.exp
	popq	%rsi
	popq	%rdi
	popq	%rdx
	movl	%eax, %ebx
	movl	%ebx, -92(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	-92(%rbp), %edx
	movl	-84(%rbp), %edi
	add	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -84(%rbp)
.Lfloat_toString88:
	movq	-64(%rbp), %r14
	movq	4(%r14), %rbx
	movq	%rbx, -64(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movl	-80(%rbp), %edi
	sub	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -80(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movl	-88(%rbp), %edi
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
	movl	-80(%rbp), %edi
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
	pushq	%rsi
	movl	-24(%rbp), %eax
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
	movl	-80(%rbp), %edi
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
	movq	-64(%rbp), %r14
	movl	0(%r14), %edi
	cmpl	%edx, %edi
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.Lfloat_toString91
	pushq	%rdi
	pushq	%rdx
	movl	-80(%rbp), %edx
	movl	-96(%rbp), %edi
	add	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -96(%rbp)
.Lfloat_toString91:
	movq	-64(%rbp), %r14
	movq	4(%r14), %rbx
	movq	%rbx, -64(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	-80(%rbp), %eax
	movl	-92(%rbp), %ecx
	cltd
	idiv	%ecx
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -80(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movl	-88(%rbp), %edi
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
	movl	-88(%rbp), %edi
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
	pushq	%rdi
	movl	-84(%rbp), %eax
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
	pushq	%rdi
	movl	-96(%rbp), %eax
	movl	%eax, %edi
	call	io.printInt
	popq	%rdi
	popq	%rdx
	pushq	%rdx
	pushq	%rdi
	movl	-84(%rbp), %eax
	movl	%eax, %edi
	lea	-106(%rbp), %rax
	pushq	%rsi
	movq	%rax, %rax
	movq	%rax, %rsi
	call	int_toString
	popq	%rsi
	popq	%rdi
	popq	%rdx
	pushq	%rdx
	pushq	%rdi
	movq	-32(%rbp), %rax
	movq	%rax, %rdi
	lea	-106(%rbp), %rax
	pushq	%rsi
	movq	%rax, %rax
	movq	%rax, %rsi
	pushq	%rdx
	movq	-16(%rbp), %rax
	movq	%rax, %rdx
	call	str_concat
	popq	%rdx
	popq	%rsi
	popq	%rdi
	popq	%rdx
	pushq	%rdx
	pushq	%rdi
	movq	-16(%rbp), %rax
	movq	%rax, %rdi
	pushq	%rsi
	movq	$.strfloat_toString93, %rax
	movq	%rax, %rsi
	pushq	%rdx
	movq	-16(%rbp), %rax
	movq	%rax, %rdx
	call	str_concat
	popq	%rdx
	popq	%rsi
	popq	%rdi
	popq	%rdx
	pushq	%rdx
	pushq	%rdi
	movl	-96(%rbp), %eax
	movl	%eax, %edi
	lea	-106(%rbp), %rax
	pushq	%rsi
	movq	%rax, %rax
	movq	%rax, %rsi
	call	int_toString
	popq	%rsi
	popq	%rdi
	popq	%rdx
	pushq	%rdx
	pushq	%rdi
	movq	-16(%rbp), %rax
	movq	%rax, %rdi
	lea	-106(%rbp), %rax
	pushq	%rsi
	movq	%rax, %rax
	movq	%rax, %rsi
	pushq	%rdx
	movq	-16(%rbp), %rax
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
	movl	-36(%rbp), %edx
	movq	-16(%rbp), %rdi
	add	%rdx, %rdi
	movq	%rdi, %rax
	popq	%rdx
	popq	%rdi
	movq	%rax, %rbx
	movq	%rbx, -16(%rbp)
	movq	-24(%rbp), %rbx
	movq	%rbx, -44(%rbp)
	movl	$0, %ebx
	movl	%ebx, -48(%rbp)
	jmp	.Lstr_subString95
.Lstr_subString94:
	movq	-16(%rbp), %rax
	movb	(%rax), %al
	movb	%al, %bl
	movb	%bl, -49(%rbp)
	movq	-44(%rbp), %rax
	movb	-49(%rbp), %bl
	movb	%bl, (%rax)
	pushq	%rdi
	pushq	%rdx
	movq	$1, %rdx
	movq	-44(%rbp), %rdi
	add	%rdx, %rdi
	movq	%rdi, %rax
	popq	%rdx
	popq	%rdi
	movq	%rax, %rbx
	movq	%rbx, -44(%rbp)
	pushq	%rdi
	pushq	%rdx
	movq	$1, %rdx
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
	movl	-48(%rbp), %edi
	add	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -48(%rbp)
.Lstr_subString95:
	pushq	%rdi
	pushq	%rdx
	movl	-8(%rbp), %edx
	movl	-48(%rbp), %edi
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
	movq	-24(%rbp), %rax
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
	pushq	%rdi
	movq	-8(%rbp), %rax
	movq	%rax, %rdi
	call	len
	popq	%rdi
	popq	%rdx
	movl	%eax, %ebx
	movl	%ebx, -20(%rbp)
	movq	-16(%rbp), %rbx
	movq	%rbx, -28(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movl	-20(%rbp), %edi
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
	movl	-32(%rbp), %edx
	movq	-8(%rbp), %rdi
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
	movl	-32(%rbp), %edi
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
	movl	-32(%rbp), %edi
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
	movq	-28(%rbp), %rax
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
	movq	-8(%rbp), %rdi
	add	%rdx, %rdi
	movq	%rdi, %rax
	popq	%rdx
	popq	%rdi
	movq	%rax, %rbx
	movq	%rbx, -8(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movl	-16(%rbp), %edi
	add	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -16(%rbp)
.Lstr_at99:
	pushq	%rdi
	pushq	%rdx
	movl	-12(%rbp), %edx
	movl	-16(%rbp), %edi
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
	movb	-13(%rbp), %al
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
	pushq	%rdi
	movq	-8(%rbp), %rax
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
	pushq	%rdi
	movq	-8(%rbp), %rax
	movq	%rax, %rdi
	pushq	%rsi
	movl	-32(%rbp), %eax
	movl	%eax, %esi
	call	str_at
	popq	%rsi
	popq	%rdi
	popq	%rdx
	movb	%al, %bl
	movb	%bl, -33(%rbp)
	pushq	%rdx
	movq	-16(%rbp), %r15
	pushq	%rdi
	movb	-33(%rbp), %al
	movb	%al, %dil
	pushq	%rsi
	movl	-32(%rbp), %eax
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
	movl	%ebx, -37(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movl	-37(%rbp), %edi
	cmpl	%edx, %edi
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.Lstr_forEach102
	movl	-37(%rbp), %eax
	leave
	ret
.Lstr_forEach102:
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
.Lstr_forEach101:
	pushq	%rdi
	pushq	%rdx
	movl	-28(%rbp), %edx
	movl	-32(%rbp), %edi
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
	movb	-1(%rbp), %dil
	cmpb	%dl, %dil
	setle	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %dl
	pushq	%rdi
	pushq	%rdx
	movb	$48, %dl
	movb	-1(%rbp), %dil
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
	pushq	%rdi
	movb	-17(%rbp), %al
	movb	%al, %dil
	call	io.toInt
	popq	%rdi
	popq	%rdx
	movl	%eax, %ebx
	movl	%ebx, -21(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	$16, %edx
	movl	-12(%rbp), %edi
	imul	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -25(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	-25(%rbp), %edx
	movl	-21(%rbp), %edi
	add	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -12(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movq	-8(%rbp), %rdi
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
	movl	-16(%rbp), %edi
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
	movb	-17(%rbp), %dil
	cmpb	%dl, %dil
	setne	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$1, %al
	je	.Lhex_toInt103
	movl	-12(%rbp), %eax
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

