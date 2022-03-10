.global	str_forEach
.global	str_at
.global	str_reverse
.global	str_subString
.global	float_toString
.global	sPrintFloat
.global	str_toFloat
.global	charToFloat
.global	str_split
.global	str_find
.global	int_toString
.global	str_toInt
.global	str_concat
.global	str_comp
.global	len
.global	ascii


.text

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
	leave
	ret
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
	movl	$1, %edx
	movl	-13(%rbp), %eax
	add	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -13(%rbp)
	movl	$1, %edx
	movq	-8(%rbp), %rax
	add	%rdx, %rax
	movq	%rax, %rbx
	movq	%rbx, -8(%rbp)
.Llen1:
	movb	-9(%rbp), %dl
	movq	-8(%rbp), %rax
	movb	(%rax), %al
	movb	%al, %al
	cmpb	%dl, %al
	setne	%al
	movb	%al, %al
	cmpb	$1, %al
	je	.Llen0
	movl	-13(%rbp), %eax
	leave
	ret
	leave
	ret
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
	movl	-24(%rbp), %edx
	movl	-20(%rbp), %eax
	cmpl	%edx, %eax
	setne	%al
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
	movl	$1, %edx
	movl	-28(%rbp), %eax
	add	%edx, %eax
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
	movb	-30(%rbp), %dl
	movb	-29(%rbp), %al
	cmpb	%dl, %al
	setne	%al
	movb	%al, %al
	cmpb	$0, %al
	je	.Lstr_comp5
	movl	$0, %eax
	leave
	ret
.Lstr_comp5:
	movl	$1, %edx
	movq	-16(%rbp), %rax
	add	%rdx, %rax
	movq	%rax, %rbx
	movq	%rbx, -16(%rbp)
	movl	$1, %edx
	movq	-8(%rbp), %rax
	add	%rdx, %rax
	movq	%rax, %rbx
	movq	%rbx, -8(%rbp)
.Lstr_comp4:
	movl	-20(%rbp), %edx
	movl	-28(%rbp), %eax
	cmpl	%edx, %eax
	setl	%al
	movb	%al, %al
	cmpb	$1, %al
	je	.Lstr_comp3
	movl	$1, %eax
	leave
	ret
	leave
	ret
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
	movl	$1, %edx
	movq	-8(%rbp), %rax
	add	%rdx, %rax
	movq	%rax, %rbx
	movq	%rbx, -8(%rbp)
	movl	$1, %edx
	movq	-24(%rbp), %rax
	add	%rdx, %rax
	movq	%rax, %rbx
	movq	%rbx, -24(%rbp)
.Lstr_concat7:
	movb	-25(%rbp), %dl
	movq	-8(%rbp), %rax
	movb	(%rax), %al
	movb	%al, %al
	cmpb	%dl, %al
	setne	%al
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
	movl	$1, %edx
	movq	-16(%rbp), %rax
	add	%rdx, %rax
	movq	%rax, %rbx
	movq	%rbx, -16(%rbp)
	movl	$1, %edx
	movq	-24(%rbp), %rax
	add	%rdx, %rax
	movq	%rax, %rbx
	movq	%rbx, -24(%rbp)
.Lstr_concat9:
	movb	-25(%rbp), %dl
	movq	-16(%rbp), %rax
	movb	(%rax), %al
	movb	%al, %al
	cmpb	%dl, %al
	setne	%al
	movb	%al, %al
	cmpb	$1, %al
	je	.Lstr_concat8
	movq	-24(%rbp), %rax
	movb	-25(%rbp), %bl
	movb	%bl, (%rax)
	movl	$0, %eax
	leave
	ret
	leave
	ret
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
	movb	$45, %dl
	movb	-13(%rbp), %al
	cmpb	%dl, %al
	sete	%al
	movb	%al, %al
	cmpb	$0, %al
	je	.Lstr_toInt10
	movl	$-1, %ebx
	movl	%ebx, -12(%rbp)
	movq	$1, %rdx
	movq	-8(%rbp), %rax
	add	%rdx, %rax
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
	movl	$1, %edx
	movl	-22(%rbp), %eax
	sub	%edx, %eax
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
	call	exp
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
	call	toInt
	popq	%rdi
	popq	%rdx
	movl	%eax, %ebx
	movl	%ebx, -35(%rbp)
	movl	-35(%rbp), %edx
	movl	-18(%rbp), %eax
	imul	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -18(%rbp)
	movl	-18(%rbp), %edx
	movl	-26(%rbp), %eax
	add	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -26(%rbp)
	movl	$1, %edx
	movl	-30(%rbp), %eax
	sub	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -30(%rbp)
	movl	$1, %edx
	movq	-8(%rbp), %rax
	add	%rdx, %rax
	movq	%rax, %rbx
	movq	%rbx, -8(%rbp)
.Lstr_toInt12:
	movl	$-1, %edx
	movl	-30(%rbp), %eax
	cmpl	%edx, %eax
	setg	%al
	movb	%al, %al
	cmpb	$1, %al
	je	.Lstr_toInt11
	movl	-12(%rbp), %edx
	movl	-26(%rbp), %eax
	imul	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -26(%rbp)
	movl	-26(%rbp), %eax
	leave
	ret
	leave
	ret
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
	movl	$0, %edx
	movl	-4(%rbp), %eax
	cmpl	%edx, %eax
	sete	%al
	movb	%al, %al
	cmpb	$0, %al
	je	.Lint_toString13
	movq	-12(%rbp), %rax
	movb	$48, %bl
	movb	%bl, (%rax)
	movq	-12(%rbp), %rax
	movb	-29(%rbp), %bl
	movb	%bl, (%rax)
	movq	$1, %rdx
	movq	-12(%rbp), %rax
	add	%rdx, %rax
	movq	%rax, %rbx
	movq	%rbx, -12(%rbp)
	movl	$0, %eax
	leave
	ret
.Lint_toString13:
	movl	$0, %edx
	movl	-4(%rbp), %eax
	cmpl	%edx, %eax
	setl	%al
	movb	%al, %al
	cmpb	$0, %al
	je	.Lint_toString14
	movq	-12(%rbp), %rax
	movb	$45, %bl
	movb	%bl, (%rax)
	movq	$1, %rdx
	movq	-12(%rbp), %rax
	add	%rdx, %rax
	movq	%rax, %rbx
	movq	%rbx, -12(%rbp)
	movl	$-1, %edx
	movl	-4(%rbp), %eax
	imul	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -4(%rbp)
.Lint_toString14:
	movl	$1, %edx
	movl	-4(%rbp), %eax
	imul	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -16(%rbp)
	jmp	.Lint_toString16
.Lint_toString15:
	movl	-16(%rbp), %eax
	cltd
	idivl	-20(%rbp)
	movl	%eax, %ebx
	movl	%ebx, -16(%rbp)
	movl	$1, %edx
	movl	-28(%rbp), %eax
	add	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -28(%rbp)
.Lint_toString16:
	movl	$0, %edx
	movl	-16(%rbp), %eax
	cmpl	%edx, %eax
	setne	%al
	movb	%al, %al
	cmpb	$1, %al
	je	.Lint_toString15
	movl	$1, %edx
	movl	-28(%rbp), %eax
	sub	%edx, %eax
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
	call	exp
	popq	%rsi
	popq	%rdi
	popq	%rdx
	movl	%eax, %ebx
	movl	%ebx, -24(%rbp)
	movl	-4(%rbp), %eax
	cltd
	idivl	-24(%rbp)
	movl	%eax, %ebx
	movl	%ebx, -16(%rbp)
	pushq	%rdx
	pushq	%rdi
	movl	-16(%rbp), %eax
	movl	%eax, %edi
	call	toChar
	popq	%rdi
	popq	%rdx
	movb	%al, %bl
	movb	%bl, -30(%rbp)
	movq	-12(%rbp), %rax
	movb	-30(%rbp), %bl
	movb	%bl, (%rax)
	movq	$1, %rdx
	movq	-12(%rbp), %rax
	add	%rdx, %rax
	movq	%rax, %rbx
	movq	%rbx, -12(%rbp)
	movl	-24(%rbp), %edx
	movl	-16(%rbp), %eax
	imul	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -16(%rbp)
	movl	-16(%rbp), %edx
	movl	-4(%rbp), %eax
	sub	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -4(%rbp)
	movl	$1, %edx
	movl	-28(%rbp), %eax
	sub	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -28(%rbp)
.Lint_toString18:
	movl	$-1, %edx
	movl	-28(%rbp), %eax
	cmpl	%edx, %eax
	setg	%al
	movb	%al, %al
	cmpb	$1, %al
	je	.Lint_toString17
	movq	-12(%rbp), %rax
	movb	-29(%rbp), %bl
	movb	%bl, (%rax)
	movl	$0, %eax
	leave
	ret
	leave
	ret
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
	jmp	.Lstr_find20
.Lstr_find19:
	movl	$1, %edx
	movq	-8(%rbp), %rax
	add	%rdx, %rax
	movq	%rax, %rbx
	movq	%rbx, -8(%rbp)
	movq	-8(%rbp), %rax
	movb	(%rax), %al
	movb	%al, %bl
	movb	%bl, -18(%rbp)
	movb	-9(%rbp), %dl
	movb	-18(%rbp), %al
	cmpb	%dl, %al
	sete	%al
	movb	%al, %al
	cmpb	$0, %al
	je	.Lstr_find21
	movl	-13(%rbp), %eax
	leave
	ret
.Lstr_find21:
	movl	$1, %edx
	movl	-13(%rbp), %eax
	add	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -13(%rbp)
.Lstr_find20:
	movl	-17(%rbp), %edx
	movl	-13(%rbp), %eax
	cmpl	%edx, %eax
	setl	%al
	movb	%al, %al
	cmpb	$1, %al
	je	.Lstr_find19
	movl	$-1, %eax
	leave
	ret
	leave
	ret
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
	jmp	.Lstr_split23
.Lstr_split22:
	movb	-26(%rbp), %dl
	movb	-27(%rbp), %al
	cmpb	%dl, %al
	sete	%al
	movb	%al, %al
	cmpb	$0, %al
	je	.Lstr_split24
	movl	$-1, %eax
	leave
	ret
.Lstr_split24:
	movb	-9(%rbp), %dl
	movb	-27(%rbp), %al
	cmpb	%dl, %al
	setne	%al
	movb	%al, %al
	cmpb	$0, %al
	je	.Lstr_split25
	movq	-17(%rbp), %rax
	movb	-27(%rbp), %bl
	movb	%bl, (%rax)
.Lstr_split25:
	movl	$1, %edx
	movq	-8(%rbp), %rax
	add	%rdx, %rax
	movq	%rax, %rbx
	movq	%rbx, -8(%rbp)
	movl	$1, %edx
	movq	-17(%rbp), %rax
	add	%rdx, %rax
	movq	%rax, %rbx
	movq	%rbx, -17(%rbp)
	movq	-8(%rbp), %rax
	movb	(%rax), %al
	movb	%al, %bl
	movb	%bl, -27(%rbp)
.Lstr_split23:
	movb	-9(%rbp), %dl
	movb	-27(%rbp), %al
	cmpb	%dl, %al
	setne	%al
	movb	%al, %al
	cmpb	$1, %al
	je	.Lstr_split22
	movq	-17(%rbp), %rax
	movb	-26(%rbp), %bl
	movb	%bl, (%rax)
	movl	$1, %edx
	movq	-8(%rbp), %rax
	add	%rdx, %rax
	movq	%rax, %rbx
	movq	%rbx, -8(%rbp)
	movb	-26(%rbp), %dl
	movq	-8(%rbp), %rax
	movb	(%rax), %al
	movb	%al, %al
	cmpb	%dl, %al
	sete	%al
	movb	%al, %al
	cmpb	$0, %al
	je	.Lstr_split26
	movl	$-1, %eax
	leave
	ret
.Lstr_split26:
	pushq	%rdx
	pushq	%rdi
	movq	$.strstr_split27, %rax
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
	leave
	ret
charToFloat:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$16, %rsp
	movb	%dil, -1(%rbp)
	movss	.floatcharToFloat28, %xmm0
	movss	%xmm0, %xmm0
	movss	%xmm0, -5(%rbp)
	movb	$48, %dl
	movb	-1(%rbp), %al
	cmpb	%dl, %al
	sete	%al
	movb	%al, %al
	cmpb	$0, %al
	je	.LcharToFloat29
	movss	.floatcharToFloat30, %xmm0
	movss	%xmm0, %xmm0
	movss	%xmm0, -5(%rbp)
.LcharToFloat29:
	movb	$49, %dl
	movb	-1(%rbp), %al
	cmpb	%dl, %al
	sete	%al
	movb	%al, %al
	cmpb	$0, %al
	je	.LcharToFloat31
	movss	.floatcharToFloat32, %xmm0
	movss	%xmm0, %xmm0
	movss	%xmm0, -5(%rbp)
.LcharToFloat31:
	movb	$50, %dl
	movb	-1(%rbp), %al
	cmpb	%dl, %al
	sete	%al
	movb	%al, %al
	cmpb	$0, %al
	je	.LcharToFloat33
	movss	.floatcharToFloat34, %xmm0
	movss	%xmm0, %xmm0
	movss	%xmm0, -5(%rbp)
.LcharToFloat33:
	movb	$51, %dl
	movb	-1(%rbp), %al
	cmpb	%dl, %al
	sete	%al
	movb	%al, %al
	cmpb	$0, %al
	je	.LcharToFloat35
	movss	.floatcharToFloat36, %xmm0
	movss	%xmm0, %xmm0
	movss	%xmm0, -5(%rbp)
.LcharToFloat35:
	movb	$52, %dl
	movb	-1(%rbp), %al
	cmpb	%dl, %al
	sete	%al
	movb	%al, %al
	cmpb	$0, %al
	je	.LcharToFloat37
	movss	.floatcharToFloat38, %xmm0
	movss	%xmm0, %xmm0
	movss	%xmm0, -5(%rbp)
.LcharToFloat37:
	movb	$53, %dl
	movb	-1(%rbp), %al
	cmpb	%dl, %al
	sete	%al
	movb	%al, %al
	cmpb	$0, %al
	je	.LcharToFloat39
	movss	.floatcharToFloat40, %xmm0
	movss	%xmm0, %xmm0
	movss	%xmm0, -5(%rbp)
.LcharToFloat39:
	movb	$54, %dl
	movb	-1(%rbp), %al
	cmpb	%dl, %al
	sete	%al
	movb	%al, %al
	cmpb	$0, %al
	je	.LcharToFloat41
	movss	.floatcharToFloat42, %xmm0
	movss	%xmm0, %xmm0
	movss	%xmm0, -5(%rbp)
.LcharToFloat41:
	movb	$55, %dl
	movb	-1(%rbp), %al
	cmpb	%dl, %al
	sete	%al
	movb	%al, %al
	cmpb	$0, %al
	je	.LcharToFloat43
	movss	.floatcharToFloat44, %xmm0
	movss	%xmm0, %xmm0
	movss	%xmm0, -5(%rbp)
.LcharToFloat43:
	movb	$56, %dl
	movb	-1(%rbp), %al
	cmpb	%dl, %al
	sete	%al
	movb	%al, %al
	cmpb	$0, %al
	je	.LcharToFloat45
	movss	.floatcharToFloat46, %xmm0
	movss	%xmm0, %xmm0
	movss	%xmm0, -5(%rbp)
.LcharToFloat45:
	movb	$57, %dl
	movb	-1(%rbp), %al
	cmpb	%dl, %al
	sete	%al
	movb	%al, %al
	cmpb	$0, %al
	je	.LcharToFloat47
	movss	.floatcharToFloat48, %xmm0
	movss	%xmm0, %xmm0
	movss	%xmm0, -5(%rbp)
.LcharToFloat47:
	movl	-5(%rbp), %eax
	leave
	ret
	leave
	ret
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
	movl	$1, %edx
	movl	-52(%rbp), %eax
	sub	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -52(%rbp)
	movss	.floatstr_toFloat49, %xmm0
	movss	%xmm0, %xmm0
	movss	%xmm0, -56(%rbp)
	movl	$0, %ebx
	movl	%ebx, -60(%rbp)
	movss	.floatstr_toFloat50, %xmm0
	movss	%xmm0, %xmm0
	movss	%xmm0, -64(%rbp)
	jmp	.Lstr_toFloat52
.Lstr_toFloat51:
	movss	.floatstr_toFloat54, %xmm0
	movss	%xmm0, %xmm1
	movss	-64(%rbp), %xmm0
	mulss	%xmm1, %xmm0
	movss	%xmm0, %xmm0
	movss	%xmm0, -64(%rbp)
	movl	$1, %edx
	movl	-60(%rbp), %eax
	add	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -60(%rbp)
.Lstr_toFloat52:
	movl	-52(%rbp), %edx
	movl	-60(%rbp), %eax
	cmpl	%edx, %eax
	setl	%al
	movb	%al, %al
	cmpb	$1, %al
	je	.Lstr_toFloat51
	movl	$0, %ebx
	movl	%ebx, -60(%rbp)
	movss	.floatstr_toFloat55, %xmm0
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
	movss	.floatstr_toFloat56, %xmm0
	movss	%xmm0, %xmm0
	movss	%xmm0, -74(%rbp)
	jmp	.Lstr_toFloat58
.Lstr_toFloat57:
	movb	$46, %dl
	movb	-70(%rbp), %al
	cmpb	%dl, %al
	sete	%al
	movb	%al, %al
	cmpb	$0, %al
	je	.Lstr_toFloat59
	movss	.floatstr_toFloat60, %xmm0
	movss	%xmm0, %xmm0
	movss	%xmm0, -64(%rbp)
.Lstr_toFloat59:
	movb	$46, %dl
	movb	-70(%rbp), %al
	cmpb	%dl, %al
	setne	%al
	movb	%al, %al
	cmpb	$0, %al
	je	.Lstr_toFloat61
	pushq	%rdx
	pushq	%rdi
	movb	-70(%rbp), %al
	movb	%al, %dil
	call	charToFloat
	popq	%rdi
	popq	%rdx
	movl	%eax, %ebx
	movl	%ebx, -68(%rbp)
	movss	.floatstr_toFloat63, %xmm0
	movss	%xmm0, %xmm1
	movss	-56(%rbp), %xmm0
	addss	%xmm1, %xmm0
	movss	%xmm0, %xmm0
	movss	%xmm0, -78(%rbp)
	movss	-64(%rbp), %xmm1
	movss	-68(%rbp), %xmm0
	mulss	%xmm1, %xmm0
	movss	%xmm0, %xmm0
	movss	%xmm0, -56(%rbp)
	movss	-78(%rbp), %xmm1
	movss	-56(%rbp), %xmm0
	addss	%xmm1, %xmm0
	movss	%xmm0, %xmm0
	movss	%xmm0, -56(%rbp)
	movss	-74(%rbp), %xmm1
	movss	-64(%rbp), %xmm0
	divss	%xmm1, %xmm0
	movss	%xmm0, %xmm0
	movss	%xmm0, -64(%rbp)
.Lstr_toFloat61:
	movl	$1, %edx
	movq	-8(%rbp), %rax
	add	%rdx, %rax
	movq	%rax, %rbx
	movq	%rbx, -8(%rbp)
	movq	-8(%rbp), %rax
	movb	(%rax), %al
	movb	%al, %bl
	movb	%bl, -70(%rbp)
	movl	$1, %edx
	movl	-60(%rbp), %eax
	add	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -60(%rbp)
.Lstr_toFloat58:
	movb	-69(%rbp), %dl
	movb	-70(%rbp), %al
	cmpb	%dl, %al
	setne	%al
	movb	%al, %al
	cmpb	$1, %al
	je	.Lstr_toFloat57
	lea	-56(%rbp), %rax
	movq	%rax, %rbx
	movq	%rbx, -82(%rbp)
	movss	.floatstr_toFloat64, %xmm0
	movss	%xmm0, %xmm0
	movss	%xmm0, -86(%rbp)
	movss	-86(%rbp), %xmm1
	movss	-86(%rbp), %xmm0
	mulss	%xmm1, %xmm0
	movss	%xmm0, %xmm0
	movss	%xmm0, -86(%rbp)
	movq	-82(%rbp), %rax
	movl	(%rax), %eax
	movl	%eax, %ebx
	movl	%ebx, -90(%rbp)
	movl	-90(%rbp), %eax
	leave
	ret
	leave
	ret
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
	call	printFloat
	popq	%rdi
	popq	%rdx
	movl	$0, %eax
	leave
	ret
	leave
	ret
float_toString:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$112, %rsp
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	movl	%edx, -20(%rbp)
	movq	$0, %rdx
	movq	-8(%rbp), %rax
	cmpq	%rdx, %rax
	sete	%al
	movb	%al, %al
	cmpb	$0, %al
	je	.Lfloat_toString65
	movl	$-1, %eax
	leave
	ret
.Lfloat_toString65:
	movl	$6, %ebx
	movl	%ebx, -24(%rbp)
	movq	$.strfloat_toString66, %rbx
	movq	%rbx, -32(%rbp)
	movq	-8(%rbp), %rax
	movl	(%rax), %eax
	movl	%eax, %ebx
	movl	%ebx, -36(%rbp)
	movl	$0, %edx
	movl	-36(%rbp), %eax
	cmpl	%edx, %eax
	sete	%al
	movb	%al, %al
	cmpb	$0, %al
	je	.Lfloat_toString67
	pushq	%rdx
	pushq	%rdi
	movl	$0, %eax
	movl	%eax, %edi
	call	printInt
	popq	%rdi
	popq	%rdx
	movl	$0, %eax
	leave
	ret
.Lfloat_toString67:
	movl	$31, %edx
	movl	-36(%rbp), %eax
	movl	%edx, %ecx
	sarl	%cl, %eax
	movl	%eax, %ebx
	movl	%ebx, -40(%rbp)
	movl	$23, %edx
	movl	-36(%rbp), %eax
	movl	%edx, %ecx
	sarl	%cl, %eax
	movl	%eax, %ebx
	movl	%ebx, -44(%rbp)
	movl	$255, %edx
	movl	-44(%rbp), %eax
	andl	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -44(%rbp)
	movl	$127, %edx
	movl	-44(%rbp), %eax
	sub	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -44(%rbp)
	movl	$8388607, %edx
	movl	-36(%rbp), %eax
	andl	%edx, %eax
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
	jmp	.Lfloat_toString69
.Lfloat_toString68:
	pushq	%rdx
	movl	$1, %edx
	movl	-48(%rbp), %eax
	andl	%edx, %eax
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
	movl	$1, %edx
	movl	-48(%rbp), %eax
	movl	%edx, %ecx
	sarl	%cl, %eax
	movl	%eax, %ebx
	movl	%ebx, -48(%rbp)
	movl	$1, %edx
	movl	-68(%rbp), %eax
	add	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -68(%rbp)
.Lfloat_toString69:
	movl	$23, %edx
	movl	-68(%rbp), %eax
	cmpl	%edx, %eax
	setl	%al
	movb	%al, %al
	cmpb	$1, %al
	je	.Lfloat_toString68
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
	jmp	.Lfloat_toString71
.Lfloat_toString70:
	movq	-64(%rbp), %rdx
	movq	12(%rdx), %rbx
	movq	%rbx, -64(%rbp)
.Lfloat_toString71:
	movq	$0, %rdx
	movq	-64(%rbp), %rdx
	movq	12(%rdx), %rax
	cmpq	%rdx, %rax
	setne	%al
	movb	%al, %al
	cmpb	$1, %al
	je	.Lfloat_toString70
	movl	$0, %edx
	movl	-40(%rbp), %eax
	cmpl	%edx, %eax
	setne	%al
	movb	%al, %al
	cmpb	$0, %al
	je	.Lfloat_toString72
	movq	$.strfloat_toString73, %rbx
	movq	%rbx, -32(%rbp)
.Lfloat_toString72:
	movl	$1, %edx
	movl	-20(%rbp), %eax
	cmpl	%edx, %eax
	sete	%al
	movb	%al, %al
	cmpb	$0, %al
	je	.Lfloat_toString74
	movl	$0, %edx
	movl	-40(%rbp), %eax
	cmpl	%edx, %eax
	sete	%al
	movb	%al, %al
	cmpb	$0, %al
	je	.Lfloat_toString75
	movq	$.strfloat_toString76, %rbx
	movq	%rbx, -32(%rbp)
.Lfloat_toString75:
.Lfloat_toString74:
	movl	-44(%rbp), %ebx
	movl	%ebx, -80(%rbp)
	movl	$0, %ebx
	movl	%ebx, -88(%rbp)
	jmp	.Lfloat_toString78
.Lfloat_toString77:
	movl	$1, %edx
	movq	-64(%rbp), %rdx
	movl	0(%rdx), %eax
	cmpl	%edx, %eax
	sete	%al
	movb	%al, %al
	cmpb	$0, %al
	je	.Lfloat_toString79
	pushq	%rdx
	pushq	%rdi
	movl	$2, %eax
	movl	%eax, %edi
	pushq	%rsi
	movl	-80(%rbp), %eax
	movl	%eax, %esi
	call	exp
	popq	%rsi
	popq	%rdi
	popq	%rdx
	movl	%eax, %ebx
	movl	%ebx, -92(%rbp)
	movl	-92(%rbp), %edx
	movl	-84(%rbp), %eax
	add	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -84(%rbp)
.Lfloat_toString79:
	movq	-64(%rbp), %rdx
	movq	4(%rdx), %rbx
	movq	%rbx, -64(%rbp)
	movl	$1, %edx
	movl	-80(%rbp), %eax
	sub	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -80(%rbp)
	movl	$1, %edx
	movl	-88(%rbp), %eax
	add	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -88(%rbp)
.Lfloat_toString78:
	movl	$-1, %edx
	movl	-80(%rbp), %eax
	cmpl	%edx, %eax
	setg	%al
	movb	%al, %al
	cmpb	$1, %al
	je	.Lfloat_toString77
	pushq	%rdx
	pushq	%rdi
	movl	$10, %eax
	movl	%eax, %edi
	pushq	%rsi
	movl	-24(%rbp), %eax
	movl	%eax, %esi
	call	exp
	popq	%rsi
	popq	%rdi
	popq	%rdx
	movl	%eax, %ebx
	movl	%ebx, -80(%rbp)
	movl	$5, %edx
	movl	-80(%rbp), %eax
	imul	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -80(%rbp)
	movl	$2, %ebx
	movl	%ebx, -92(%rbp)
	movl	$0, %ebx
	movl	%ebx, -96(%rbp)
	jmp	.Lfloat_toString81
.Lfloat_toString80:
	movl	$1, %edx
	movq	-64(%rbp), %rdx
	movl	0(%rdx), %eax
	cmpl	%edx, %eax
	sete	%al
	movb	%al, %al
	cmpb	$0, %al
	je	.Lfloat_toString82
	movl	-80(%rbp), %edx
	movl	-96(%rbp), %eax
	add	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -96(%rbp)
.Lfloat_toString82:
	movq	-64(%rbp), %rdx
	movq	4(%rdx), %rbx
	movq	%rbx, -64(%rbp)
	movl	-80(%rbp), %eax
	cltd
	idivl	-92(%rbp)
	movl	%eax, %ebx
	movl	%ebx, -80(%rbp)
	movl	$1, %edx
	movl	-88(%rbp), %eax
	add	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -88(%rbp)
.Lfloat_toString81:
	movl	$23, %edx
	movl	-88(%rbp), %eax
	cmpl	%edx, %eax
	setl	%al
	movb	%al, %al
	cmpb	$1, %al
	je	.Lfloat_toString80
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
	call	printInt
	popq	%rdi
	popq	%rdx
	pushq	%rdx
	pushq	%rdi
	movq	$.strfloat_toString83, %rax
	movq	%rax, %rdi
	call	print
	popq	%rdi
	popq	%rdx
	pushq	%rdx
	pushq	%rdi
	movl	-96(%rbp), %eax
	movl	%eax, %edi
	call	printInt
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
	movq	$.strfloat_toString84, %rax
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
	leave
	ret
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
	movl	-36(%rbp), %edx
	movq	-16(%rbp), %rax
	add	%rdx, %rax
	movq	%rax, %rbx
	movq	%rbx, -16(%rbp)
	movq	-24(%rbp), %rbx
	movq	%rbx, -44(%rbp)
	movl	$0, %ebx
	movl	%ebx, -48(%rbp)
	jmp	.Lstr_subString86
.Lstr_subString85:
	movq	-16(%rbp), %rax
	movb	(%rax), %al
	movb	%al, %bl
	movb	%bl, -49(%rbp)
	movq	-44(%rbp), %rax
	movb	-49(%rbp), %bl
	movb	%bl, (%rax)
	movq	$1, %rdx
	movq	-44(%rbp), %rax
	add	%rdx, %rax
	movq	%rax, %rbx
	movq	%rbx, -44(%rbp)
	movq	$1, %rdx
	movq	-16(%rbp), %rax
	add	%rdx, %rax
	movq	%rax, %rbx
	movq	%rbx, -16(%rbp)
	movl	$1, %edx
	movl	-48(%rbp), %eax
	add	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -48(%rbp)
.Lstr_subString86:
	movl	-8(%rbp), %edx
	movl	-48(%rbp), %eax
	cmpl	%edx, %eax
	setl	%al
	movb	%al, %al
	cmpb	$1, %al
	je	.Lstr_subString85
	movq	-44(%rbp), %rax
	movb	$0, %bl
	movb	%bl, (%rax)
	movq	-24(%rbp), %rax
	leave
	ret
	leave
	ret
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
	movl	$1, %edx
	movl	-20(%rbp), %eax
	sub	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -32(%rbp)
	jmp	.Lstr_reverse88
.Lstr_reverse87:
	movl	-32(%rbp), %edx
	movq	-8(%rbp), %rax
	add	%rdx, %rax
	movq	%rax, %rbx
	movq	%rbx, -40(%rbp)
	movq	-40(%rbp), %rax
	movb	(%rax), %al
	movb	%al, %bl
	movb	%bl, -41(%rbp)
	movq	-16(%rbp), %rax
	movb	-41(%rbp), %bl
	movb	%bl, (%rax)
	movl	$1, %edx
	movq	-16(%rbp), %rax
	add	%rdx, %rax
	movq	%rax, %rbx
	movq	%rbx, -16(%rbp)
	movl	$1, %edx
	movl	-32(%rbp), %eax
	sub	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -32(%rbp)
.Lstr_reverse88:
	movl	$-1, %edx
	movl	-32(%rbp), %eax
	cmpl	%edx, %eax
	setg	%al
	movb	%al, %al
	cmpb	$1, %al
	je	.Lstr_reverse87
	movq	-16(%rbp), %rax
	movb	$0, %bl
	movb	%bl, (%rax)
	movq	-28(%rbp), %rax
	leave
	ret
	leave
	ret
str_at:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$16, %rsp
	movq	%rdi, -8(%rbp)
	movl	%esi, -12(%rbp)
	movl	$0, %ebx
	movl	%ebx, -16(%rbp)
	jmp	.Lstr_at90
.Lstr_at89:
	movq	$1, %rdx
	movq	-8(%rbp), %rax
	add	%rdx, %rax
	movq	%rax, %rbx
	movq	%rbx, -8(%rbp)
	movl	$1, %edx
	movl	-16(%rbp), %eax
	add	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -16(%rbp)
.Lstr_at90:
	movl	-12(%rbp), %edx
	movl	-16(%rbp), %eax
	cmpl	%edx, %eax
	setl	%al
	movb	%al, %al
	cmpb	$1, %al
	je	.Lstr_at89
	movq	-8(%rbp), %rax
	movb	(%rax), %al
	movb	%al, %bl
	movb	%bl, -13(%rbp)
	movb	-13(%rbp), %al
	leave
	ret
	leave
	ret
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
	jmp	.Lstr_forEach92
.Lstr_forEach91:
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
	movl	$1, %edx
	movl	-37(%rbp), %eax
	cmpl	%edx, %eax
	sete	%al
	movb	%al, %al
	cmpb	$0, %al
	je	.Lstr_forEach93
	movl	-37(%rbp), %eax
	leave
	ret
.Lstr_forEach93:
	movl	$1, %edx
	movl	-32(%rbp), %eax
	add	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -32(%rbp)
.Lstr_forEach92:
	movl	-28(%rbp), %edx
	movl	-32(%rbp), %eax
	cmpl	%edx, %eax
	setl	%al
	movb	%al, %al
	cmpb	$1, %al
	je	.Lstr_forEach91
	movl	$0, %eax
	leave
	ret
	leave
	ret


.data

.strfloat_toString84:
	.asciz	 "."
.strfloat_toString83:
	.asciz	 "."
.strfloat_toString76:
	.asciz	 "+"
.strfloat_toString73:
	.asciz	 "-"
.strfloat_toString66:
	.asciz	 ""
.floatstr_toFloat64:
	.float	0.0
.floatstr_toFloat63:
	.float	0.00
.floatstr_toFloat60:
	.float	0.1
.floatstr_toFloat56:
	.float	10.0
.floatstr_toFloat55:
	.float	0.00
.floatstr_toFloat54:
	.float	10.0
.floatstr_toFloat50:
	.float	1.0
.floatstr_toFloat49:
	.float	0.00
.floatcharToFloat48:
	.float	9.00
.floatcharToFloat46:
	.float	8.00
.floatcharToFloat44:
	.float	7.00
.floatcharToFloat42:
	.float	6.00
.floatcharToFloat40:
	.float	5.00
.floatcharToFloat38:
	.float	4.00
.floatcharToFloat36:
	.float	3.00
.floatcharToFloat34:
	.float	2.00
.floatcharToFloat32:
	.float	1.00
.floatcharToFloat30:
	.float	0.00
.floatcharToFloat28:
	.float	0.00
.strstr_split27:
	.asciz	 ""


.bss

