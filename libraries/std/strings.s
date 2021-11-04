.global	str_split
.global	str_find
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
	jmp	.Lstr_find14
.Lstr_find13:
	mov	-13(%rbp), %edx
	mov	-9(%rbp), %rax
	add	%rdx, %rax
	movl	%eax, -25(%rbp)
	movq	-25(%rbp), %rax
	movb	(%rax), %al
	movb	%al, %al
	movb	-1(%rbp), %cl
	cmpb	%cl, %al
	jne	.Lstr_find15
	movl	-13(%rbp), %eax
	leave
	ret
.Lstr_find15:
	mov	$1, %edx
	mov	-13(%rbp), %eax
	add	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -13(%rbp)
.Lstr_find14:
	movl	-13(%rbp), %eax
	movl	-17(%rbp), %ecx
	cmpl	%ecx, %eax
	jl	.Lstr_find13
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
	jmp	.Lstr_split17
.Lstr_split16:
	movb	-27(%rbp), %al
	movb	-26(%rbp), %cl
	cmpb	%cl, %al
	jne	.Lstr_split18
	movl	$-1, %eax
	leave
	ret
.Lstr_split18:
	movb	-27(%rbp), %al
	movb	-9(%rbp), %cl
	cmpb	%cl, %al
	je	.Lstr_split19
	movq	-17(%rbp), %rax
	movb	-27(%rbp), %bl
	movb	%bl, (%rax)
.Lstr_split19:
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
.Lstr_split17:
	movb	-27(%rbp), %al
	movb	-9(%rbp), %cl
	cmpb	%cl, %al
	jne	.Lstr_split16
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
	jne	.Lstr_split20
	movl	$-1, %eax
	leave
	ret
.Lstr_split20:
	movq	$.strstr_split21, %rax
	movq	%rax, %rdi
	movq	-8(%rbp), %rax
	movq	%rax, %rsi
	movq	-25(%rbp), %rax
	movq	%rax, %rdx
	call	str_concat
	movl	$0, %eax
	leave
	ret


.data

.strstr_split21:
	.asciz	 ""


.bss

