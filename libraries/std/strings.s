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
	movq	%rax, -12(%rbp)
	movq	-12(%rbp), %rax
	movb	(%rax), %al
	movb	%al, %al
	leave
	ret
len:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$16, %rsp
	movq	%rdi, -8(%rbp)
	movl	$0, %eax
	movl	%eax, %edi
	call	ascii
	movb	%al, -9(%rbp)
	movl	$0, -13(%rbp)
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
str_comp:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$32, %rsp
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	movq	-8(%rbp), %rax
	movq	%rax, %rdi
	call	len
	movl	%eax, -20(%rbp)
	movq	-16(%rbp), %rax
	movq	%rax, %rdi
	call	len
	movl	%eax, -24(%rbp)
	movl	-20(%rbp), %eax
	movl	-24(%rbp), %ecx
	cmpl	%ecx, %eax
	je	.Lstr_comp2
	movl	$0, %eax
	leave
	ret
.Lstr_comp2:
	movl	$0, -28(%rbp)
	jmp	.Lstr_comp4
.Lstr_comp3:
	mov	$1, %edx
	mov	-28(%rbp), %eax
	add	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -28(%rbp)
	movq	-8(%rbp), %rax
	movb	(%rax), %al
	movb	%al, -29(%rbp)
	movq	-16(%rbp), %rax
	movb	(%rax), %al
	movb	%al, -30(%rbp)
	movb	-29(%rbp), %al
	movb	-30(%rbp), %cl
	cmpb	%cl, %al
	je	.Lstr_comp5
	movl	$0, %eax
	leave
	ret
.Lstr_comp5:
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
.Lstr_comp4:
	movl	-28(%rbp), %eax
	movl	-20(%rbp), %ecx
	cmpl	%ecx, %eax
	jl	.Lstr_comp3
	movl	$1, %eax
	leave
	ret
str_concat:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$32, %rsp
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	movq	%rdx, -24(%rbp)
	movl	$0, %eax
	movl	%eax, %edi
	call	ascii
	movb	%al, -25(%rbp)
	jmp	.Lstr_concat7
.Lstr_concat6:
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
.Lstr_concat7:
	movq	-8(%rbp), %rax
	movb	(%rax), %al
	movb	%al, %al
	movb	-25(%rbp), %cl
	cmpb	%cl, %al
	jne	.Lstr_concat6
	jmp	.Lstr_concat9
.Lstr_concat8:
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
.Lstr_concat9:
	movq	-16(%rbp), %rax
	movb	(%rax), %al
	movb	%al, %al
	movb	-25(%rbp), %cl
	cmpb	%cl, %al
	jne	.Lstr_concat8
	movq	-24(%rbp), %rax
	movb	-25(%rbp), %bl
	movb	%bl, (%rax)
	movl	$0, %eax
	leave
	ret
str_toInt:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$48, %rsp
	movq	%rdi, -8(%rbp)
	movl	$1, -12(%rbp)
	movq	-8(%rbp), %rax
	movb	(%rax), %al
	movb	%al, -13(%rbp)
	movb	-13(%rbp), %al
	movb	$45, %cl
	cmpb	%cl, %al
	jne	.Lstr_toInt10
	movl	$-1, %ebx
	movl	%ebx, -12(%rbp)
	mov	$1, %rdx
	mov	-8(%rbp), %rax
	add	%rdx, %rax
	movl	%eax, %ebx
	movl	%ebx, -8(%rbp)
.Lstr_toInt10:
	movl	$0, %eax
	movl	%eax, %edi
	call	ascii
	movb	%al, -14(%rbp)
	movl	$0, -18(%rbp)
	movq	-8(%rbp), %rax
	movq	%rax, %rdi
	call	len
	movl	%eax, -22(%rbp)
	movl	$0, -26(%rbp)
	mov	$1, %edx
	mov	-22(%rbp), %eax
	sub	%edx, %eax
	movl	%eax, -30(%rbp)
	jmp	.Lstr_toInt12
.Lstr_toInt11:
	movl	$10, %eax
	movl	%eax, %edi
	movl	-30(%rbp), %eax
	movl	%eax, %esi
	call	exp
	movl	%eax, %ebx
	movl	%ebx, -18(%rbp)
	movq	-8(%rbp), %rax
	movb	(%rax), %al
	movb	%al, -31(%rbp)
	movb	-31(%rbp), %al
	movb	%al, %dil
	call	toInt
	movl	%eax, -35(%rbp)
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
.Lstr_toInt12:
	movl	-30(%rbp), %eax
	movl	$-1, %ecx
	cmpl	%ecx, %eax
	jg	.Lstr_toInt11
	mov	-12(%rbp), %edx
	mov	-26(%rbp), %eax
	imul	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -26(%rbp)
	movl	-26(%rbp), %eax
	leave
	ret
int_toString:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$32, %rsp
	movl	%edi, -4(%rbp)
	movq	%rsi, -12(%rbp)
	movl	$0, -16(%rbp)
	movl	$10, -20(%rbp)
	movl	$0, -24(%rbp)
	movl	$0, -28(%rbp)
	movl	$0, %eax
	movl	%eax, %edi
	call	ascii
	movb	%al, -29(%rbp)
	movl	-4(%rbp), %eax
	movl	$0, %ecx
	cmpl	%ecx, %eax
	jne	.Lint_toString13
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
.Lint_toString13:
	movl	-4(%rbp), %eax
	movl	$0, %ecx
	cmpl	%ecx, %eax
	jge	.Lint_toString14
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
.Lint_toString14:
	mov	$1, %edx
	mov	-4(%rbp), %eax
	imul	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -16(%rbp)
	jmp	.Lint_toString16
.Lint_toString15:
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
.Lint_toString16:
	movl	-16(%rbp), %eax
	movl	$0, %ecx
	cmpl	%ecx, %eax
	jne	.Lint_toString15
	mov	$1, %edx
	mov	-28(%rbp), %eax
	sub	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -28(%rbp)
	movb	$32, -30(%rbp)
	jmp	.Lint_toString18
.Lint_toString17:
	movl	$10, %eax
	movl	%eax, %edi
	movl	-28(%rbp), %eax
	movl	%eax, %esi
	call	exp
	movl	%eax, %ebx
	movl	%ebx, -24(%rbp)
	mov	-4(%rbp), %eax
	cltd
	idivl	-24(%rbp)
	movl	%eax, %ebx
	movl	%ebx, -16(%rbp)
	movl	-16(%rbp), %eax
	movl	%eax, %edi
	call	toChar
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
.Lint_toString18:
	movl	-28(%rbp), %eax
	movl	$-1, %ecx
	cmpl	%ecx, %eax
	jg	.Lint_toString17
	movq	-12(%rbp), %rax
	movb	-29(%rbp), %bl
	movb	%bl, (%rax)
	movl	$0, %eax
	leave
	ret
str_find:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$32, %rsp
	movb	%dil, -1(%rbp)
	movq	%rsi, -9(%rbp)
	movl	$0, -13(%rbp)
	movq	-9(%rbp), %rax
	movq	%rax, %rdi
	call	len
	movl	%eax, -17(%rbp)
	jmp	.Lstr_find20
.Lstr_find19:
	mov	-13(%rbp), %edx
	mov	-9(%rbp), %rax
	add	%rdx, %rax
	movl	%eax, -25(%rbp)
	movq	-25(%rbp), %rax
	movb	(%rax), %al
	movb	%al, %al
	movb	-1(%rbp), %cl
	cmpb	%cl, %al
	jne	.Lstr_find21
	movl	-13(%rbp), %eax
	leave
	ret
.Lstr_find21:
	mov	$1, %edx
	mov	-13(%rbp), %eax
	add	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -13(%rbp)
.Lstr_find20:
	movl	-13(%rbp), %eax
	movl	-17(%rbp), %ecx
	cmpl	%ecx, %eax
	jl	.Lstr_find19
	movl	$-1, %eax
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
	movl	$0, %eax
	movl	%eax, %edi
	call	ascii
	movb	%al, -26(%rbp)
	movq	-8(%rbp), %rax
	movb	(%rax), %al
	movb	%al, -27(%rbp)
	jmp	.Lstr_split23
.Lstr_split22:
	movb	-27(%rbp), %al
	movb	-26(%rbp), %cl
	cmpb	%cl, %al
	jne	.Lstr_split24
	movl	$-1, %eax
	leave
	ret
.Lstr_split24:
	movb	-27(%rbp), %al
	movb	-9(%rbp), %cl
	cmpb	%cl, %al
	je	.Lstr_split25
	movq	-17(%rbp), %rax
	movb	-27(%rbp), %bl
	movb	%bl, (%rax)
.Lstr_split25:
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
.Lstr_split23:
	movb	-27(%rbp), %al
	movb	-9(%rbp), %cl
	cmpb	%cl, %al
	jne	.Lstr_split22
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
	jne	.Lstr_split26
	movl	$-1, %eax
	leave
	ret
.Lstr_split26:
	movq	$.strstr_split27, %rax
	movq	%rax, %rdi
	movq	-8(%rbp), %rax
	movq	%rax, %rsi
	movq	-25(%rbp), %rax
	movq	%rax, %rdx
	call	str_concat
	movl	$0, %eax
	leave
	ret
charToFloat:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$16, %rsp
	movb	%dil, -1(%rbp)
	movss	.floatcharToFloat28, %xmm0
	movss	%xmm0, -5(%rbp)
	movb	-1(%rbp), %al
	movb	$48, %cl
	cmpb	%cl, %al
	jne	.LcharToFloat29
	movss	.floatcharToFloat30, %xmm0
	movss	%xmm0, %xmm0
	movss	%xmm0, -5(%rbp)
.LcharToFloat29:
	movb	-1(%rbp), %al
	movb	$49, %cl
	cmpb	%cl, %al
	jne	.LcharToFloat31
	movss	.floatcharToFloat32, %xmm0
	movss	%xmm0, %xmm0
	movss	%xmm0, -5(%rbp)
.LcharToFloat31:
	movb	-1(%rbp), %al
	movb	$50, %cl
	cmpb	%cl, %al
	jne	.LcharToFloat33
	movss	.floatcharToFloat34, %xmm0
	movss	%xmm0, %xmm0
	movss	%xmm0, -5(%rbp)
.LcharToFloat33:
	movb	-1(%rbp), %al
	movb	$51, %cl
	cmpb	%cl, %al
	jne	.LcharToFloat35
	movss	.floatcharToFloat36, %xmm0
	movss	%xmm0, %xmm0
	movss	%xmm0, -5(%rbp)
.LcharToFloat35:
	movb	-1(%rbp), %al
	movb	$52, %cl
	cmpb	%cl, %al
	jne	.LcharToFloat37
	movss	.floatcharToFloat38, %xmm0
	movss	%xmm0, %xmm0
	movss	%xmm0, -5(%rbp)
.LcharToFloat37:
	movb	-1(%rbp), %al
	movb	$53, %cl
	cmpb	%cl, %al
	jne	.LcharToFloat39
	movss	.floatcharToFloat40, %xmm0
	movss	%xmm0, %xmm0
	movss	%xmm0, -5(%rbp)
.LcharToFloat39:
	movb	-1(%rbp), %al
	movb	$54, %cl
	cmpb	%cl, %al
	jne	.LcharToFloat41
	movss	.floatcharToFloat42, %xmm0
	movss	%xmm0, %xmm0
	movss	%xmm0, -5(%rbp)
.LcharToFloat41:
	movb	-1(%rbp), %al
	movb	$55, %cl
	cmpb	%cl, %al
	jne	.LcharToFloat43
	movss	.floatcharToFloat44, %xmm0
	movss	%xmm0, %xmm0
	movss	%xmm0, -5(%rbp)
.LcharToFloat43:
	movb	-1(%rbp), %al
	movb	$56, %cl
	cmpb	%cl, %al
	jne	.LcharToFloat45
	movss	.floatcharToFloat46, %xmm0
	movss	%xmm0, %xmm0
	movss	%xmm0, -5(%rbp)
.LcharToFloat45:
	movb	-1(%rbp), %al
	movb	$57, %cl
	cmpb	%cl, %al
	jne	.LcharToFloat47
	movss	.floatcharToFloat48, %xmm0
	movss	%xmm0, %xmm0
	movss	%xmm0, -5(%rbp)
.LcharToFloat47:
	movl	-5(%rbp), %eax
	leave
	ret
str_toFloat:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$96, %rsp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rax
	movq	%rax, %rdi
	movb	$46, %al
	movb	%al, %sil
	lea	-28(%rbp), %rax
	movq	%rax, %rax
	movq	%rax, %rdx
	lea	-48(%rbp), %rax
	movq	%rax, %rax
	movq	%rax, %rcx
	call	str_split
	lea	-28(%rbp), %rax
	movq	%rax, %rax
	movq	%rax, %rdi
	call	len
	movl	%eax, -52(%rbp)
	mov	$1, %edx
	mov	-52(%rbp), %eax
	sub	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -52(%rbp)
	movss	.floatstr_toFloat49, %xmm0
	movss	%xmm0, -56(%rbp)
	movl	$0, -60(%rbp)
	movss	.floatstr_toFloat50, %xmm0
	movss	%xmm0, -64(%rbp)
	jmp	.Lstr_toFloat52
.Lstr_toFloat51:
	movss	.floatstr_toFloat54, %xmm0
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
.Lstr_toFloat52:
	movl	-60(%rbp), %eax
	movl	-52(%rbp), %ecx
	cmpl	%ecx, %eax
	jl	.Lstr_toFloat51
	movl	$0, %ebx
	movl	%ebx, -60(%rbp)
	movss	.floatstr_toFloat55, %xmm0
	movss	%xmm0, -68(%rbp)
	movl	$0, %eax
	movl	%eax, %edi
	call	ascii
	movb	%al, -69(%rbp)
	movq	-8(%rbp), %rax
	movb	(%rax), %al
	movb	%al, -70(%rbp)
	movss	.floatstr_toFloat56, %xmm0
	movss	%xmm0, -74(%rbp)
	jmp	.Lstr_toFloat58
.Lstr_toFloat57:
	movb	-70(%rbp), %al
	movb	$46, %cl
	cmpb	%cl, %al
	jne	.Lstr_toFloat59
	movss	.floatstr_toFloat60, %xmm0
	movss	%xmm0, %xmm0
	movss	%xmm0, -64(%rbp)
.Lstr_toFloat59:
	movb	-70(%rbp), %al
	movb	$46, %cl
	cmpb	%cl, %al
	je	.Lstr_toFloat61
	movb	-70(%rbp), %al
	movb	%al, %dil
	call	charToFloat
	movl	%eax, %ebx
	movl	%ebx, -68(%rbp)
	movss	.floatstr_toFloat63, %xmm0
	movss	%xmm0, %xmm1
	movss	-56(%rbp), %xmm0
	addss	%xmm1, %xmm0
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
.Lstr_toFloat58:
	movb	-70(%rbp), %al
	movb	-69(%rbp), %cl
	cmpb	%cl, %al
	jne	.Lstr_toFloat57
	lea	-56(%rbp), %rax
	movq	%rax, -86(%rbp)
	movss	.floatstr_toFloat64, %xmm0
	movss	%xmm0, -90(%rbp)
	movss	-90(%rbp), %xmm1
	movss	-90(%rbp), %xmm0
	mulss	%xmm1, %xmm0
	movss	%xmm0, %xmm0
	movss	%xmm0, -90(%rbp)
	movq	-86(%rbp), %rax
	movl	(%rax), %eax
	movl	%eax, -94(%rbp)
	movl	-94(%rbp), %eax
	leave
	ret
sPrintFloat:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$16, %rsp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rax
	movq	%rax, %rdi
	call	printFloat
	movl	$0, %eax
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
	jne	.Lfloat_toString65
	movl	$-1, %eax
	leave
	ret
.Lfloat_toString65:
	movl	$6, -24(%rbp)
	movq	$.strfloat_toString66, -32(%rbp)
	movq	-8(%rbp), %rax
	movl	(%rax), %eax
	movl	%eax, -36(%rbp)
	movl	-36(%rbp), %eax
	movl	$0, %ecx
	cmpl	%ecx, %eax
	jne	.Lfloat_toString67
	movl	$0, %eax
	movl	%eax, %edi
	call	printInt
	movl	$0, %eax
	leave
	ret
.Lfloat_toString67:
	mov	$31, %edx
	mov	-36(%rbp), %eax
	movl	%edx, %ecx
	sarl	%cl, %eax
	movl	%eax, -40(%rbp)
	mov	$23, %edx
	mov	-36(%rbp), %eax
	movl	%edx, %ecx
	sarl	%cl, %eax
	movl	%eax, -44(%rbp)
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
	movl	%eax, -48(%rbp)
	movl	$0, %eax
	movl	%eax, %edi
	call	newBit
	movq	%rax, -56(%rbp)
	movq	-56(%rbp), %rbx
	movq	%rbx, -64(%rbp)
	movl	$0, -68(%rbp)
	jmp	.Lfloat_toString69
.Lfloat_toString68:
	mov	$1, %edx
	mov	-48(%rbp), %eax
	andl	%edx, %eax
	movl	%eax, %eax
	movl	%eax, %edi
	call	newBit
	movq	%rax, -76(%rbp)
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
.Lfloat_toString69:
	movl	-68(%rbp), %eax
	movl	$23, %ecx
	cmpl	%ecx, %eax
	jl	.Lfloat_toString68
	movl	$1, %eax
	movl	%eax, %edi
	call	newBit
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
	movq	-64(%rbp), %rdx
	movq	12(%rdx), %rax
	movq	$0, %rcx
	cmpq	%rcx, %rax
	jne	.Lfloat_toString70
	movl	-40(%rbp), %eax
	movl	$0, %ecx
	cmpl	%ecx, %eax
	je	.Lfloat_toString72
	movq	$.strfloat_toString73, %rbx
	movq	%rbx, -32(%rbp)
.Lfloat_toString72:
	movl	-20(%rbp), %eax
	movl	$1, %ecx
	cmpl	%ecx, %eax
	jne	.Lfloat_toString74
	movl	-40(%rbp), %eax
	movl	$0, %ecx
	cmpl	%ecx, %eax
	jne	.Lfloat_toString75
	movq	$.strfloat_toString76, %rbx
	movq	%rbx, -32(%rbp)
.Lfloat_toString75:
.Lfloat_toString74:
	movl	-44(%rbp), %ebx
	movl	%ebx, -80(%rbp)
	movl	$0, -88(%rbp)
	jmp	.Lfloat_toString78
.Lfloat_toString77:
	movq	-64(%rbp), %rdx
	movl	0(%rdx), %eax
	movl	$1, %ecx
	cmpl	%ecx, %eax
	jne	.Lfloat_toString79
	movl	$2, %eax
	movl	%eax, %edi
	movl	-80(%rbp), %eax
	movl	%eax, %esi
	call	exp
	movl	%eax, -92(%rbp)
	mov	-92(%rbp), %edx
	mov	-84(%rbp), %eax
	add	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -84(%rbp)
.Lfloat_toString79:
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
.Lfloat_toString78:
	movl	-80(%rbp), %eax
	movl	$-1, %ecx
	cmpl	%ecx, %eax
	jg	.Lfloat_toString77
	movl	$10, %eax
	movl	%eax, %edi
	movl	-24(%rbp), %eax
	movl	%eax, %esi
	call	exp
	movl	%eax, %ebx
	movl	%ebx, -80(%rbp)
	mov	$5, %edx
	mov	-80(%rbp), %eax
	imul	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -80(%rbp)
	movl	$2, -96(%rbp)
	movl	$0, -100(%rbp)
	jmp	.Lfloat_toString81
.Lfloat_toString80:
	movq	-64(%rbp), %rdx
	movl	0(%rdx), %eax
	movl	$1, %ecx
	cmpl	%ecx, %eax
	jne	.Lfloat_toString82
	mov	-80(%rbp), %edx
	mov	-100(%rbp), %eax
	add	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -100(%rbp)
.Lfloat_toString82:
	movq	-64(%rbp), %rdx
	movq	4(%rdx), %rbx
	movq	%rbx, -64(%rbp)
	mov	-80(%rbp), %eax
	cltd
	idivl	-96(%rbp)
	movl	%eax, %ebx
	movl	%ebx, -80(%rbp)
	mov	$1, %edx
	mov	-88(%rbp), %eax
	add	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -88(%rbp)
.Lfloat_toString81:
	movl	-88(%rbp), %eax
	movl	$23, %ecx
	cmpl	%ecx, %eax
	jl	.Lfloat_toString80
	lea	-32(%rbp), %rax
	movq	$1, %rdi
	movq	%rax, %rsi
	movq	$1, %rdx
	movq	$1, %rax
	syscall	
	movl	-84(%rbp), %eax
	movl	%eax, %edi
	call	printInt
	movq	$.strfloat_toString83, %rax
	movq	%rax, %rdi
	call	print
	movl	-100(%rbp), %eax
	movl	%eax, %edi
	call	printInt
	movl	-84(%rbp), %eax
	movl	%eax, %edi
	lea	-110(%rbp), %rax
	movq	%rax, %rax
	movq	%rax, %rsi
	call	int_toString
	movq	-32(%rbp), %rax
	movq	%rax, %rdi
	lea	-110(%rbp), %rax
	movq	%rax, %rax
	movq	%rax, %rsi
	movq	-16(%rbp), %rax
	movq	%rax, %rdx
	call	str_concat
	movq	-16(%rbp), %rax
	movq	%rax, %rdi
	movq	$.strfloat_toString84, %rax
	movq	%rax, %rsi
	movq	-16(%rbp), %rax
	movq	%rax, %rdx
	call	str_concat
	movl	-100(%rbp), %eax
	movl	%eax, %edi
	lea	-110(%rbp), %rax
	movq	%rax, %rax
	movq	%rax, %rsi
	call	int_toString
	movq	-16(%rbp), %rax
	movq	%rax, %rdi
	lea	-110(%rbp), %rax
	movq	%rax, %rax
	movq	%rax, %rsi
	movq	-16(%rbp), %rax
	movq	%rax, %rdx
	call	str_concat
	movl	$0, %eax
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

