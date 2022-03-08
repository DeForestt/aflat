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
	subq	$16, %rsp
	movq	%rdi, -8(%rbp)
	pushq	%rdi
	movl	$0, %eax
	movl	%eax, %edi
	call	ascii
	popq	%rdi
	movb	%al, %bl
	movb	%bl, -9(%rbp)
	movl	$0, %ebx
	movl	%ebx, -13(%rbp)
	jmp	.Llen1
.Llen0:
	mov	$1, %edx
	mov	-13(%rbp), %eax
	add	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -13(%rbp)
	mov	$1, %edx
	mov	-8(%rbp), %rax
	add	%rdx, %rax
	movl	%eax, %ebx
	movl	%ebx, -8(%rbp)
.Llen1:
	movq	-8(%rbp), %rax
	movb	(%rax), %al
	movb	%al, %al
	movb	-9(%rbp), %cl
	cmpb	%cl, %al
	jne	.Llen0
	movl	-13(%rbp), %eax
	leave
	ret
	leave
	ret
str_comp:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$32, %rsp
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	pushq	%rdi
	movq	-8(%rbp), %rax
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
	movl	-20(%rbp), %eax
	movl	-24(%rbp), %ecx
	cmpl	%ecx, %eax
	je	.Lstr_comp2
	movl	$0, %eax
	leave
	ret
.Lstr_comp2:
	movl	$0, %ebx
	movl	%ebx, -28(%rbp)
	jmp	.Lstr_comp6
.Lstr_comp5:
	mov	$1, %edx
	mov	-28(%rbp), %eax
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
	movb	-29(%rbp), %al
	movb	-30(%rbp), %cl
	cmpb	%cl, %al
	je	.Lstr_comp7
	movl	$0, %eax
	leave
	ret
.Lstr_comp7:
	mov	$1, %edx
	mov	-16(%rbp), %rax
	add	%rdx, %rax
	movl	%eax, %ebx
	movl	%ebx, -16(%rbp)
	mov	$1, %edx
	mov	-8(%rbp), %rax
	add	%rdx, %rax
	movl	%eax, %ebx
	movl	%ebx, -8(%rbp)
.Lstr_comp6:
	movl	-28(%rbp), %eax
	movl	-20(%rbp), %ecx
	cmpl	%ecx, %eax
	jl	.Lstr_comp5
	movl	$1, %eax
	leave
	ret
	leave
	ret
str_concat:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$32, %rsp
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	movq	%rdx, -24(%rbp)
	movb	$0, %bl
	movb	%bl, -25(%rbp)
	jmp	.Lstr_concat11
.Lstr_concat10:
	movq	-8(%rbp), %rax
	movb	(%rax), %al
	movb	%al, %bl
	movb	%bl, -26(%rbp)
	movq	-24(%rbp), %rax
	movb	-26(%rbp), %bl
	movb	%bl, (%rax)
	mov	$1, %edx
	mov	-8(%rbp), %rax
	add	%rdx, %rax
	movl	%eax, %ebx
	movl	%ebx, -8(%rbp)
	mov	$1, %edx
	mov	-24(%rbp), %rax
	add	%rdx, %rax
	movl	%eax, %ebx
	movl	%ebx, -24(%rbp)
.Lstr_concat11:
	movq	-8(%rbp), %rax
	movb	(%rax), %al
	movb	%al, %al
	movb	-25(%rbp), %cl
	cmpb	%cl, %al
	jne	.Lstr_concat10
	jmp	.Lstr_concat13
.Lstr_concat12:
	movq	-16(%rbp), %rax
	movb	(%rax), %al
	movb	%al, %bl
	movb	%bl, -26(%rbp)
	movq	-24(%rbp), %rax
	movb	-26(%rbp), %bl
	movb	%bl, (%rax)
	mov	$1, %edx
	mov	-16(%rbp), %rax
	add	%rdx, %rax
	movl	%eax, %ebx
	movl	%ebx, -16(%rbp)
	mov	$1, %edx
	mov	-24(%rbp), %rax
	add	%rdx, %rax
	movl	%eax, %ebx
	movl	%ebx, -24(%rbp)
.Lstr_concat13:
	movq	-16(%rbp), %rax
	movb	(%rax), %al
	movb	%al, %al
	movb	-25(%rbp), %cl
	cmpb	%cl, %al
	jne	.Lstr_concat12
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
	subq	$48, %rsp
	movq	%rdi, -8(%rbp)
	movl	$1, %ebx
	movl	%ebx, -12(%rbp)
	movq	-8(%rbp), %rax
	movb	(%rax), %al
	movb	%al, %bl
	movb	%bl, -13(%rbp)
	movb	-13(%rbp), %al
	movb	$45, %cl
	cmpb	%cl, %al
	jne	.Lstr_toInt14
	movl	$-1, %ebx
	movl	%ebx, -12(%rbp)
	mov	$1, %rdx
	mov	-8(%rbp), %rax
	add	%rdx, %rax
	movl	%eax, %ebx
	movl	%ebx, -8(%rbp)
.Lstr_toInt14:
	pushq	%rdi
	movl	$0, %eax
	movl	%eax, %edi
	call	ascii
	popq	%rdi
	movb	%al, %bl
	movb	%bl, -14(%rbp)
	movl	$0, %ebx
	movl	%ebx, -18(%rbp)
	pushq	%rdi
	movq	-8(%rbp), %rax
	movq	%rax, %rdi
	call	len
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -22(%rbp)
	movl	$0, %ebx
	movl	%ebx, -26(%rbp)
	mov	$1, %edx
	mov	-22(%rbp), %eax
	sub	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -30(%rbp)
	jmp	.Lstr_toInt18
.Lstr_toInt17:
	pushq	%rdi
	movl	$10, %eax
	movl	%eax, %edi
	pushq	%rsi
	movl	-30(%rbp), %eax
	movl	%eax, %esi
	call	exp
	popq	%rsi
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -18(%rbp)
	movq	-8(%rbp), %rax
	movb	(%rax), %al
	movb	%al, %bl
	movb	%bl, -31(%rbp)
	pushq	%rdi
	movb	-31(%rbp), %al
	movb	%al, %dil
	call	toInt
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -35(%rbp)
	mov	-35(%rbp), %edx
	mov	-18(%rbp), %eax
	imul	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -18(%rbp)
	mov	-18(%rbp), %edx
	mov	-26(%rbp), %eax
	add	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -26(%rbp)
	mov	$1, %edx
	mov	-30(%rbp), %eax
	sub	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -30(%rbp)
	mov	$1, %edx
	mov	-8(%rbp), %rax
	add	%rdx, %rax
	movl	%eax, %ebx
	movl	%ebx, -8(%rbp)
.Lstr_toInt18:
	movl	-30(%rbp), %eax
	movl	$-1, %ecx
	cmpl	%ecx, %eax
	jg	.Lstr_toInt17
	mov	-12(%rbp), %edx
	mov	-26(%rbp), %eax
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
	pushq	%rdi
	movl	$0, %eax
	movl	%eax, %edi
	call	ascii
	popq	%rdi
	movb	%al, %bl
	movb	%bl, -29(%rbp)
	movl	-4(%rbp), %eax
	movl	$0, %ecx
	cmpl	%ecx, %eax
	jne	.Lint_toString19
	movq	-12(%rbp), %rax
	movb	$48, %bl
	movb	%bl, (%rax)
	movq	-12(%rbp), %rax
	movb	-29(%rbp), %bl
	movb	%bl, (%rax)
	mov	$1, %rdx
	mov	-12(%rbp), %rax
	add	%rdx, %rax
	movl	%eax, %ebx
	movl	%ebx, -12(%rbp)
	movl	$0, %eax
	leave
	ret
.Lint_toString19:
	movl	-4(%rbp), %eax
	movl	$0, %ecx
	cmpl	%ecx, %eax
	jge	.Lint_toString22
	movq	-12(%rbp), %rax
	movb	$45, %bl
	movb	%bl, (%rax)
	mov	$1, %rdx
	mov	-12(%rbp), %rax
	add	%rdx, %rax
	movl	%eax, %ebx
	movl	%ebx, -12(%rbp)
	mov	$-1, %edx
	mov	-4(%rbp), %eax
	imul	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -4(%rbp)
.Lint_toString22:
	mov	$1, %edx
	mov	-4(%rbp), %eax
	imul	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -16(%rbp)
	jmp	.Lint_toString26
.Lint_toString25:
	mov	-16(%rbp), %eax
	cltd
	idivl	-20(%rbp)
	movl	%eax, %ebx
	movl	%ebx, -16(%rbp)
	mov	$1, %edx
	mov	-28(%rbp), %eax
	add	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -28(%rbp)
.Lint_toString26:
	movl	-16(%rbp), %eax
	movl	$0, %ecx
	cmpl	%ecx, %eax
	jne	.Lint_toString25
	mov	$1, %edx
	mov	-28(%rbp), %eax
	sub	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -28(%rbp)
	movb	$32, %bl
	movb	%bl, -30(%rbp)
	jmp	.Lint_toString28
.Lint_toString27:
	pushq	%rdi
	movl	$10, %eax
	movl	%eax, %edi
	pushq	%rsi
	movl	-28(%rbp), %eax
	movl	%eax, %esi
	call	exp
	popq	%rsi
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -24(%rbp)
	mov	-4(%rbp), %eax
	cltd
	idivl	-24(%rbp)
	movl	%eax, %ebx
	movl	%ebx, -16(%rbp)
	pushq	%rdi
	movl	-16(%rbp), %eax
	movl	%eax, %edi
	call	toChar
	popq	%rdi
	movb	%al, %bl
	movb	%bl, -30(%rbp)
	movq	-12(%rbp), %rax
	movb	-30(%rbp), %bl
	movb	%bl, (%rax)
	mov	$1, %rdx
	mov	-12(%rbp), %rax
	add	%rdx, %rax
	movl	%eax, %ebx
	movl	%ebx, -12(%rbp)
	mov	-24(%rbp), %edx
	mov	-16(%rbp), %eax
	imul	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -16(%rbp)
	mov	-16(%rbp), %edx
	mov	-4(%rbp), %eax
	sub	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -4(%rbp)
	mov	$1, %edx
	mov	-28(%rbp), %eax
	sub	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -28(%rbp)
.Lint_toString28:
	movl	-28(%rbp), %eax
	movl	$-1, %ecx
	cmpl	%ecx, %eax
	jg	.Lint_toString27
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
	subq	$32, %rsp
	movq	%rdi, -8(%rbp)
	movb	%sil, -9(%rbp)
	movl	$0, %ebx
	movl	%ebx, -13(%rbp)
	pushq	%rdi
	movq	-8(%rbp), %rax
	movq	%rax, %rdi
	call	len
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -17(%rbp)
	jmp	.Lstr_find30
.Lstr_find29:
	mov	$1, %edx
	mov	-8(%rbp), %rax
	add	%rdx, %rax
	movl	%eax, %ebx
	movl	%ebx, -8(%rbp)
	movq	-8(%rbp), %rax
	movb	(%rax), %al
	movb	%al, %bl
	movb	%bl, -18(%rbp)
	movb	-18(%rbp), %al
	movb	-9(%rbp), %cl
	cmpb	%cl, %al
	jne	.Lstr_find31
	movl	-13(%rbp), %eax
	leave
	ret
.Lstr_find31:
	mov	$1, %edx
	mov	-13(%rbp), %eax
	add	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -13(%rbp)
.Lstr_find30:
	movl	-13(%rbp), %eax
	movl	-17(%rbp), %ecx
	cmpl	%ecx, %eax
	jl	.Lstr_find29
	movl	$-1, %eax
	leave
	ret
	leave
	ret
str_split:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$32, %rsp
	movq	%rdi, -8(%rbp)
	movb	%sil, -9(%rbp)
	movq	%rdx, -17(%rbp)
	movq	%rcx, -25(%rbp)
	pushq	%rdi
	movl	$0, %eax
	movl	%eax, %edi
	call	ascii
	popq	%rdi
	movb	%al, %bl
	movb	%bl, -26(%rbp)
	movq	-8(%rbp), %rax
	movb	(%rax), %al
	movb	%al, %bl
	movb	%bl, -27(%rbp)
	jmp	.Lstr_split35
.Lstr_split34:
	movb	-27(%rbp), %al
	movb	-26(%rbp), %cl
	cmpb	%cl, %al
	jne	.Lstr_split36
	movl	$-1, %eax
	leave
	ret
.Lstr_split36:
	movb	-27(%rbp), %al
	movb	-9(%rbp), %cl
	cmpb	%cl, %al
	je	.Lstr_split39
	movq	-17(%rbp), %rax
	movb	-27(%rbp), %bl
	movb	%bl, (%rax)
.Lstr_split39:
	mov	$1, %edx
	mov	-8(%rbp), %rax
	add	%rdx, %rax
	movl	%eax, %ebx
	movl	%ebx, -8(%rbp)
	mov	$1, %edx
	mov	-17(%rbp), %rax
	add	%rdx, %rax
	movl	%eax, %ebx
	movl	%ebx, -17(%rbp)
	movq	-8(%rbp), %rax
	movb	(%rax), %al
	movb	%al, %bl
	movb	%bl, -27(%rbp)
.Lstr_split35:
	movb	-27(%rbp), %al
	movb	-9(%rbp), %cl
	cmpb	%cl, %al
	jne	.Lstr_split34
	movq	-17(%rbp), %rax
	movb	-26(%rbp), %bl
	movb	%bl, (%rax)
	mov	$1, %edx
	mov	-8(%rbp), %rax
	add	%rdx, %rax
	movl	%eax, %ebx
	movl	%ebx, -8(%rbp)
	movq	-8(%rbp), %rax
	movb	(%rax), %al
	movb	%al, %al
	movb	-26(%rbp), %cl
	cmpb	%cl, %al
	jne	.Lstr_split42
	movl	$-1, %eax
	leave
	ret
.Lstr_split42:
	pushq	%rdi
	movq	$.strstr_split45, %rax
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
	movl	$0, %eax
	leave
	ret
	leave
	ret
charToFloat:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$16, %rsp
	movb	%dil, -1(%rbp)
	movss	.floatcharToFloat46, %xmm0
	movss	%xmm0, %xmm0
	movss	%xmm0, -5(%rbp)
	movb	-1(%rbp), %al
	movb	$48, %cl
	cmpb	%cl, %al
	jne	.LcharToFloat47
	movss	.floatcharToFloat50, %xmm0
	movss	%xmm0, %xmm0
	movss	%xmm0, -5(%rbp)
.LcharToFloat47:
	movb	-1(%rbp), %al
	movb	$49, %cl
	cmpb	%cl, %al
	jne	.LcharToFloat51
	movss	.floatcharToFloat54, %xmm0
	movss	%xmm0, %xmm0
	movss	%xmm0, -5(%rbp)
.LcharToFloat51:
	movb	-1(%rbp), %al
	movb	$50, %cl
	cmpb	%cl, %al
	jne	.LcharToFloat55
	movss	.floatcharToFloat58, %xmm0
	movss	%xmm0, %xmm0
	movss	%xmm0, -5(%rbp)
.LcharToFloat55:
	movb	-1(%rbp), %al
	movb	$51, %cl
	cmpb	%cl, %al
	jne	.LcharToFloat59
	movss	.floatcharToFloat62, %xmm0
	movss	%xmm0, %xmm0
	movss	%xmm0, -5(%rbp)
.LcharToFloat59:
	movb	-1(%rbp), %al
	movb	$52, %cl
	cmpb	%cl, %al
	jne	.LcharToFloat63
	movss	.floatcharToFloat66, %xmm0
	movss	%xmm0, %xmm0
	movss	%xmm0, -5(%rbp)
.LcharToFloat63:
	movb	-1(%rbp), %al
	movb	$53, %cl
	cmpb	%cl, %al
	jne	.LcharToFloat67
	movss	.floatcharToFloat70, %xmm0
	movss	%xmm0, %xmm0
	movss	%xmm0, -5(%rbp)
.LcharToFloat67:
	movb	-1(%rbp), %al
	movb	$54, %cl
	cmpb	%cl, %al
	jne	.LcharToFloat71
	movss	.floatcharToFloat74, %xmm0
	movss	%xmm0, %xmm0
	movss	%xmm0, -5(%rbp)
.LcharToFloat71:
	movb	-1(%rbp), %al
	movb	$55, %cl
	cmpb	%cl, %al
	jne	.LcharToFloat75
	movss	.floatcharToFloat78, %xmm0
	movss	%xmm0, %xmm0
	movss	%xmm0, -5(%rbp)
.LcharToFloat75:
	movb	-1(%rbp), %al
	movb	$56, %cl
	cmpb	%cl, %al
	jne	.LcharToFloat79
	movss	.floatcharToFloat82, %xmm0
	movss	%xmm0, %xmm0
	movss	%xmm0, -5(%rbp)
.LcharToFloat79:
	movb	-1(%rbp), %al
	movb	$57, %cl
	cmpb	%cl, %al
	jne	.LcharToFloat83
	movss	.floatcharToFloat86, %xmm0
	movss	%xmm0, %xmm0
	movss	%xmm0, -5(%rbp)
.LcharToFloat83:
	movl	-5(%rbp), %eax
	leave
	ret
	leave
	ret
str_toFloat:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$96, %rsp
	movq	%rdi, -8(%rbp)
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
	lea	-28(%rbp), %rax
	pushq	%rdi
	movq	%rax, %rax
	movq	%rax, %rdi
	call	len
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -52(%rbp)
	mov	$1, %edx
	mov	-52(%rbp), %eax
	sub	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -52(%rbp)
	movss	.floatstr_toFloat87, %xmm0
	movss	%xmm0, %xmm0
	movss	%xmm0, -56(%rbp)
	movl	$0, %ebx
	movl	%ebx, -60(%rbp)
	movss	.floatstr_toFloat88, %xmm0
	movss	%xmm0, %xmm0
	movss	%xmm0, -64(%rbp)
	jmp	.Lstr_toFloat90
.Lstr_toFloat89:
	movss	.floatstr_toFloat92, %xmm0
	movss	%xmm0, %xmm1
	movss	-64(%rbp), %xmm0
	mulss	%xmm1, %xmm0
	movss	%xmm0, %xmm0
	movss	%xmm0, -64(%rbp)
	mov	$1, %edx
	mov	-60(%rbp), %eax
	add	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -60(%rbp)
.Lstr_toFloat90:
	movl	-60(%rbp), %eax
	movl	-52(%rbp), %ecx
	cmpl	%ecx, %eax
	jl	.Lstr_toFloat89
	movl	$0, %ebx
	movl	%ebx, -60(%rbp)
	movss	.floatstr_toFloat93, %xmm0
	movss	%xmm0, %xmm0
	movss	%xmm0, -68(%rbp)
	pushq	%rdi
	movl	$0, %eax
	movl	%eax, %edi
	call	ascii
	popq	%rdi
	movb	%al, %bl
	movb	%bl, -69(%rbp)
	movq	-8(%rbp), %rax
	movb	(%rax), %al
	movb	%al, %bl
	movb	%bl, -70(%rbp)
	movss	.floatstr_toFloat94, %xmm0
	movss	%xmm0, %xmm0
	movss	%xmm0, -74(%rbp)
	jmp	.Lstr_toFloat96
.Lstr_toFloat95:
	movb	-70(%rbp), %al
	movb	$46, %cl
	cmpb	%cl, %al
	jne	.Lstr_toFloat97
	movss	.floatstr_toFloat100, %xmm0
	movss	%xmm0, %xmm0
	movss	%xmm0, -64(%rbp)
.Lstr_toFloat97:
	movb	-70(%rbp), %al
	movb	$46, %cl
	cmpb	%cl, %al
	je	.Lstr_toFloat101
	pushq	%rdi
	movb	-70(%rbp), %al
	movb	%al, %dil
	call	charToFloat
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -68(%rbp)
	movss	.floatstr_toFloat105, %xmm0
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
.Lstr_toFloat101:
	mov	$1, %edx
	mov	-8(%rbp), %rax
	add	%rdx, %rax
	movl	%eax, %ebx
	movl	%ebx, -8(%rbp)
	movq	-8(%rbp), %rax
	movb	(%rax), %al
	movb	%al, %bl
	movb	%bl, -70(%rbp)
	mov	$1, %edx
	mov	-60(%rbp), %eax
	add	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -60(%rbp)
.Lstr_toFloat96:
	movb	-70(%rbp), %al
	movb	-69(%rbp), %cl
	cmpb	%cl, %al
	jne	.Lstr_toFloat95
	lea	-56(%rbp), %rax
	movq	%rax, %rbx
	movq	%rbx, -82(%rbp)
	movss	.floatstr_toFloat106, %xmm0
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
	subq	$16, %rsp
	movq	%rdi, -8(%rbp)
	pushq	%rdi
	movq	-8(%rbp), %rax
	movq	%rax, %rdi
	call	printFloat
	popq	%rdi
	movl	$0, %eax
	leave
	ret
	leave
	ret
float_toString:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$112, %rsp
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	movl	%edx, -20(%rbp)
	movq	-8(%rbp), %rax
	movq	$0, %rcx
	cmpq	%rcx, %rax
	jne	.Lfloat_toString107
	movl	$-1, %eax
	leave
	ret
.Lfloat_toString107:
	movl	$6, %ebx
	movl	%ebx, -24(%rbp)
	movq	$.strfloat_toString110, %rbx
	movq	%rbx, -32(%rbp)
	movq	-8(%rbp), %rax
	movl	(%rax), %eax
	movl	%eax, %ebx
	movl	%ebx, -36(%rbp)
	movl	-36(%rbp), %eax
	movl	$0, %ecx
	cmpl	%ecx, %eax
	jne	.Lfloat_toString111
	pushq	%rdi
	movl	$0, %eax
	movl	%eax, %edi
	call	printInt
	popq	%rdi
	movl	$0, %eax
	leave
	ret
.Lfloat_toString111:
	mov	$31, %edx
	mov	-36(%rbp), %eax
	movl	%edx, %ecx
	sarl	%cl, %eax
	movl	%eax, %ebx
	movl	%ebx, -40(%rbp)
	mov	$23, %edx
	mov	-36(%rbp), %eax
	movl	%edx, %ecx
	sarl	%cl, %eax
	movl	%eax, %ebx
	movl	%ebx, -44(%rbp)
	mov	$255, %edx
	mov	-44(%rbp), %eax
	andl	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -44(%rbp)
	mov	$127, %edx
	mov	-44(%rbp), %eax
	sub	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -44(%rbp)
	mov	$8388607, %edx
	mov	-36(%rbp), %eax
	andl	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -48(%rbp)
	pushq	%rdi
	movl	$0, %eax
	movl	%eax, %edi
	call	newBit
	popq	%rdi
	movq	%rax, %rbx
	movq	%rbx, -56(%rbp)
	movq	-56(%rbp), %rbx
	movq	%rbx, -64(%rbp)
	movl	$0, %ebx
	movl	%ebx, -68(%rbp)
	jmp	.Lfloat_toString115
.Lfloat_toString114:
	mov	$1, %edx
	mov	-48(%rbp), %eax
	andl	%edx, %eax
	pushq	%rdi
	movl	%eax, %eax
	movl	%eax, %edi
	call	newBit
	popq	%rdi
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
	mov	$1, %edx
	mov	-48(%rbp), %eax
	movl	%edx, %ecx
	sarl	%cl, %eax
	movl	%eax, %ebx
	movl	%ebx, -48(%rbp)
	mov	$1, %edx
	mov	-68(%rbp), %eax
	add	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -68(%rbp)
.Lfloat_toString115:
	movl	-68(%rbp), %eax
	movl	$23, %ecx
	cmpl	%ecx, %eax
	jl	.Lfloat_toString114
	pushq	%rdi
	movl	$1, %eax
	movl	%eax, %edi
	call	newBit
	popq	%rdi
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
	jmp	.Lfloat_toString117
.Lfloat_toString116:
	movq	-64(%rbp), %rdx
	movq	12(%rdx), %rbx
	movq	%rbx, -64(%rbp)
.Lfloat_toString117:
	movq	-64(%rbp), %rdx
	movq	12(%rdx), %rax
	movq	$0, %rcx
	cmpq	%rcx, %rax
	jne	.Lfloat_toString116
	movl	-40(%rbp), %eax
	movl	$0, %ecx
	cmpl	%ecx, %eax
	je	.Lfloat_toString118
	movq	$.strfloat_toString121, %rbx
	movq	%rbx, -32(%rbp)
.Lfloat_toString118:
	movl	-20(%rbp), %eax
	movl	$1, %ecx
	cmpl	%ecx, %eax
	jne	.Lfloat_toString122
	movl	-40(%rbp), %eax
	movl	$0, %ecx
	cmpl	%ecx, %eax
	jne	.Lfloat_toString125
	movq	$.strfloat_toString128, %rbx
	movq	%rbx, -32(%rbp)
.Lfloat_toString125:
.Lfloat_toString122:
	movl	-44(%rbp), %ebx
	movl	%ebx, -80(%rbp)
	movl	$0, %ebx
	movl	%ebx, -88(%rbp)
	jmp	.Lfloat_toString130
.Lfloat_toString129:
	movq	-64(%rbp), %rdx
	movl	0(%rdx), %eax
	movl	$1, %ecx
	cmpl	%ecx, %eax
	jne	.Lfloat_toString131
	pushq	%rdi
	movl	$2, %eax
	movl	%eax, %edi
	pushq	%rsi
	movl	-80(%rbp), %eax
	movl	%eax, %esi
	call	exp
	popq	%rsi
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -92(%rbp)
	mov	-92(%rbp), %edx
	mov	-84(%rbp), %eax
	add	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -84(%rbp)
.Lfloat_toString131:
	movq	-64(%rbp), %rdx
	movq	4(%rdx), %rbx
	movq	%rbx, -64(%rbp)
	mov	$1, %edx
	mov	-80(%rbp), %eax
	sub	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -80(%rbp)
	mov	$1, %edx
	mov	-88(%rbp), %eax
	add	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -88(%rbp)
.Lfloat_toString130:
	movl	-80(%rbp), %eax
	movl	$-1, %ecx
	cmpl	%ecx, %eax
	jg	.Lfloat_toString129
	pushq	%rdi
	movl	$10, %eax
	movl	%eax, %edi
	pushq	%rsi
	movl	-24(%rbp), %eax
	movl	%eax, %esi
	call	exp
	popq	%rsi
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -80(%rbp)
	mov	$5, %edx
	mov	-80(%rbp), %eax
	imul	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -80(%rbp)
	movl	$2, %ebx
	movl	%ebx, -92(%rbp)
	movl	$0, %ebx
	movl	%ebx, -96(%rbp)
	jmp	.Lfloat_toString135
.Lfloat_toString134:
	movq	-64(%rbp), %rdx
	movl	0(%rdx), %eax
	movl	$1, %ecx
	cmpl	%ecx, %eax
	jne	.Lfloat_toString136
	mov	-80(%rbp), %edx
	mov	-96(%rbp), %eax
	add	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -96(%rbp)
.Lfloat_toString136:
	movq	-64(%rbp), %rdx
	movq	4(%rdx), %rbx
	movq	%rbx, -64(%rbp)
	mov	-80(%rbp), %eax
	cltd
	idivl	-92(%rbp)
	movl	%eax, %ebx
	movl	%ebx, -80(%rbp)
	mov	$1, %edx
	mov	-88(%rbp), %eax
	add	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -88(%rbp)
.Lfloat_toString135:
	movl	-88(%rbp), %eax
	movl	$23, %ecx
	cmpl	%ecx, %eax
	jl	.Lfloat_toString134
	lea	-32(%rbp), %rax
	movq	$1, %rdi
	movq	%rax, %rsi
	movq	$1, %rdx
	movq	$1, %rax
	syscall	
	pushq	%rdi
	movl	-84(%rbp), %eax
	movl	%eax, %edi
	call	printInt
	popq	%rdi
	pushq	%rdi
	movq	$.strfloat_toString139, %rax
	movq	%rax, %rdi
	call	print
	popq	%rdi
	pushq	%rdi
	movl	-96(%rbp), %eax
	movl	%eax, %edi
	call	printInt
	popq	%rdi
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
	pushq	%rdi
	movq	-16(%rbp), %rax
	movq	%rax, %rdi
	pushq	%rsi
	movq	$.strfloat_toString140, %rax
	movq	%rax, %rsi
	pushq	%rdx
	movq	-16(%rbp), %rax
	movq	%rax, %rdx
	call	str_concat
	popq	%rdx
	popq	%rsi
	popq	%rdi
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
	movl	$0, %eax
	leave
	ret
	leave
	ret
str_subString:
	pushq	%rbp
	movq	%rsp, %rbp
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
	mov	-36(%rbp), %edx
	mov	-16(%rbp), %rax
	add	%rdx, %rax
	movl	%eax, %ebx
	movl	%ebx, -16(%rbp)
	movq	-24(%rbp), %rbx
	movq	%rbx, -44(%rbp)
	movl	$0, %ebx
	movl	%ebx, -48(%rbp)
	jmp	.Lstr_subString142
.Lstr_subString141:
	movq	-16(%rbp), %rax
	movb	(%rax), %al
	movb	%al, %bl
	movb	%bl, -49(%rbp)
	movq	-44(%rbp), %rax
	movb	-49(%rbp), %bl
	movb	%bl, (%rax)
	mov	$1, %rdx
	mov	-44(%rbp), %rax
	add	%rdx, %rax
	movl	%eax, %ebx
	movl	%ebx, -44(%rbp)
	mov	$1, %rdx
	mov	-16(%rbp), %rax
	add	%rdx, %rax
	movl	%eax, %ebx
	movl	%ebx, -16(%rbp)
	mov	$1, %edx
	mov	-48(%rbp), %eax
	add	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -48(%rbp)
.Lstr_subString142:
	movl	-48(%rbp), %eax
	movl	-8(%rbp), %ecx
	cmpl	%ecx, %eax
	jl	.Lstr_subString141
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
	subq	$48, %rsp
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	pushq	%rdi
	movq	-8(%rbp), %rax
	movq	%rax, %rdi
	call	len
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -20(%rbp)
	movq	-16(%rbp), %rbx
	movq	%rbx, -28(%rbp)
	mov	$1, %edx
	mov	-20(%rbp), %eax
	sub	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -32(%rbp)
	jmp	.Lstr_reverse144
.Lstr_reverse143:
	mov	-32(%rbp), %edx
	mov	-8(%rbp), %rax
	add	%rdx, %rax
	movl	%eax, %ebx
	movl	%ebx, -40(%rbp)
	movq	-40(%rbp), %rax
	movb	(%rax), %al
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
	mov	-32(%rbp), %eax
	sub	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -32(%rbp)
.Lstr_reverse144:
	movl	-32(%rbp), %eax
	movl	$-1, %ecx
	cmpl	%ecx, %eax
	jg	.Lstr_reverse143
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
	subq	$16, %rsp
	movq	%rdi, -8(%rbp)
	movl	%esi, -12(%rbp)
	movl	$0, %ebx
	movl	%ebx, -16(%rbp)
	jmp	.Lstr_at146
.Lstr_at145:
	mov	$1, %rdx
	mov	-8(%rbp), %rax
	add	%rdx, %rax
	movl	%eax, %ebx
	movl	%ebx, -8(%rbp)
	mov	$1, %edx
	mov	-16(%rbp), %eax
	add	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -16(%rbp)
.Lstr_at146:
	movl	-16(%rbp), %eax
	movl	-12(%rbp), %ecx
	cmpl	%ecx, %eax
	jl	.Lstr_at145
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
	subq	$48, %rsp
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	movq	%rdx, -24(%rbp)
	pushq	%rdi
	movq	-8(%rbp), %rax
	movq	%rax, %rdi
	call	len
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -28(%rbp)
	movl	$0, %ebx
	movl	%ebx, -32(%rbp)
	jmp	.Lstr_forEach148
.Lstr_forEach147:
	pushq	%rdi
	movq	-8(%rbp), %rax
	movq	%rax, %rdi
	pushq	%rsi
	movl	-32(%rbp), %eax
	movl	%eax, %esi
	call	str_at
	popq	%rsi
	popq	%rdi
	movb	%al, %bl
	movb	%bl, -33(%rbp)
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
	movl	%eax, %ebx
	movl	%ebx, -37(%rbp)
	movl	-37(%rbp), %eax
	movl	$1, %ecx
	cmpl	%ecx, %eax
	jne	.Lstr_forEach149
	movl	-37(%rbp), %eax
	leave
	ret
.Lstr_forEach149:
	mov	$1, %edx
	mov	-32(%rbp), %eax
	add	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -32(%rbp)
.Lstr_forEach148:
	movl	-32(%rbp), %eax
	movl	-28(%rbp), %ecx
	cmpl	%ecx, %eax
	jl	.Lstr_forEach147
	movl	$0, %eax
	leave
	ret
	leave
	ret


.data

.strfloat_toString140:
	.asciz	 "."
.strfloat_toString139:
	.asciz	 "."
.strfloat_toString128:
	.asciz	 "+"
.strfloat_toString121:
	.asciz	 "-"
.strfloat_toString110:
	.asciz	 ""
.floatstr_toFloat106:
	.float	0.0
.floatstr_toFloat105:
	.float	0.00
.floatstr_toFloat100:
	.float	0.1
.floatstr_toFloat94:
	.float	10.0
.floatstr_toFloat93:
	.float	0.00
.floatstr_toFloat92:
	.float	10.0
.floatstr_toFloat88:
	.float	1.0
.floatstr_toFloat87:
	.float	0.00
.floatcharToFloat86:
	.float	9.00
.floatcharToFloat82:
	.float	8.00
.floatcharToFloat78:
	.float	7.00
.floatcharToFloat74:
	.float	6.00
.floatcharToFloat70:
	.float	5.00
.floatcharToFloat66:
	.float	4.00
.floatcharToFloat62:
	.float	3.00
.floatcharToFloat58:
	.float	2.00
.floatcharToFloat54:
	.float	1.00
.floatcharToFloat50:
	.float	0.00
.floatcharToFloat46:
	.float	0.00
.strstr_split45:
	.asciz	 ""


.bss

