.global	printInt
.global	pullInt
.global	toInt
.global	toChar
.global	print
.global	clearInput


.text

clearInput:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$16, %rsp
	movl	$10, -4(%rbp)
	movb	$32, -8(%rbp)
	jmp	.LclearInput1
.LclearInput0:
	lea	-8(%rbp), %rax
	movq	$1, %rdi
	movq	%rax, %rsi
	movq	$1, %rdx
	movq	$0, %rax
	syscall	
.LclearInput1:
	movb	-8(%rbp), %al
	movb	-4(%rbp), %cl
	cmpb	%cl, %al
	jne	.LclearInput0
	movl	$0, %eax
	leave
	ret
print:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$16, %rsp
	movq	%rdi, -8(%rbp)
	movl	$0, -12(%rbp)
	jmp	.Lprint3
.Lprint2:
	movq	$1, %rdi
	movq	-8(%rbp), %rsi
	movq	$1, %rdx
	movq	$1, %rax
	syscall	
	mov	-8(%rbp), %rdx
	mov	$1, %eax
	add	%edx, %eax
	movl	%eax, -8(%rbp)
.Lprint3:
	movq	-8(%rbp), %rax
	movb	(%rax), %al
	movb	%al, %al
	movb	-12(%rbp), %cl
	cmpb	%cl, %al
	jne	.Lprint2
	movl	$0, %eax
	leave
	ret
toChar:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$16, %rsp
	movl	%edi, -4(%rbp)
	movb	$48, -8(%rbp)
	movl	-4(%rbp), %eax
	movl	$9, %ecx
	cmpl	%ecx, %eax
	jl	.LtoChar4
	movb	$65, -8(%rbp)
.LtoChar4:
	movl	-4(%rbp), %eax
	movl	$0, %ecx
	cmpl	%ecx, %eax
	jne	.LtoChar5
	movb	$48, -8(%rbp)
.LtoChar5:
	movl	-4(%rbp), %eax
	movl	$1, %ecx
	cmpl	%ecx, %eax
	jne	.LtoChar6
	movb	$49, -8(%rbp)
.LtoChar6:
	movl	-4(%rbp), %eax
	movl	$2, %ecx
	cmpl	%ecx, %eax
	jne	.LtoChar7
	movb	$50, -8(%rbp)
.LtoChar7:
	movl	-4(%rbp), %eax
	movl	$3, %ecx
	cmpl	%ecx, %eax
	jne	.LtoChar8
	movb	$51, -8(%rbp)
.LtoChar8:
	movl	-4(%rbp), %eax
	movl	$4, %ecx
	cmpl	%ecx, %eax
	jne	.LtoChar9
	movb	$52, -8(%rbp)
.LtoChar9:
	movl	-4(%rbp), %eax
	movl	$5, %ecx
	cmpl	%ecx, %eax
	jne	.LtoChar10
	movb	$53, -8(%rbp)
.LtoChar10:
	movl	-4(%rbp), %eax
	movl	$6, %ecx
	cmpl	%ecx, %eax
	jne	.LtoChar11
	movb	$54, -8(%rbp)
.LtoChar11:
	movl	-4(%rbp), %eax
	movl	$7, %ecx
	cmpl	%ecx, %eax
	jne	.LtoChar12
	movb	$55, -8(%rbp)
.LtoChar12:
	movl	-4(%rbp), %eax
	movl	$8, %ecx
	cmpl	%ecx, %eax
	jne	.LtoChar13
	movb	$56, -8(%rbp)
.LtoChar13:
	movl	-4(%rbp), %eax
	movl	$9, %ecx
	cmpl	%ecx, %eax
	jne	.LtoChar14
	movb	$57, -8(%rbp)
.LtoChar14:
	movb	-8(%rbp), %al
	leave
	ret
toInt:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$16, %rsp
	movb	%dil, -4(%rbp)
	movb	-4(%rbp), %al
	movb	$48, %cl
	cmpb	%cl, %al
	jne	.LtoInt15
	movl	$0, %eax
	leave
	ret
.LtoInt15:
	movb	-4(%rbp), %al
	movb	$49, %cl
	cmpb	%cl, %al
	jne	.LtoInt16
	movl	$1, %eax
	leave
	ret
.LtoInt16:
	movb	-4(%rbp), %al
	movb	$50, %cl
	cmpb	%cl, %al
	jne	.LtoInt17
	movl	$2, %eax
	leave
	ret
.LtoInt17:
	movb	-4(%rbp), %al
	movb	$51, %cl
	cmpb	%cl, %al
	jne	.LtoInt18
	movl	$3, %eax
	leave
	ret
.LtoInt18:
	movb	-4(%rbp), %al
	movb	$52, %cl
	cmpb	%cl, %al
	jne	.LtoInt19
	movl	$4, %eax
	leave
	ret
.LtoInt19:
	movb	-4(%rbp), %al
	movb	$53, %cl
	cmpb	%cl, %al
	jne	.LtoInt20
	movl	$5, %eax
	leave
	ret
.LtoInt20:
	movb	-4(%rbp), %al
	movb	$54, %cl
	cmpb	%cl, %al
	jne	.LtoInt21
	movl	$6, %eax
	leave
	ret
.LtoInt21:
	movb	-4(%rbp), %al
	movb	$55, %cl
	cmpb	%cl, %al
	jne	.LtoInt22
	movl	$7, %eax
	leave
	ret
.LtoInt22:
	movb	-4(%rbp), %al
	movb	$56, %cl
	cmpb	%cl, %al
	jne	.LtoInt23
	movl	$8, %eax
	leave
	ret
.LtoInt23:
	movb	-4(%rbp), %al
	movb	$57, %cl
	cmpb	%cl, %al
	jne	.LtoInt24
	movl	$9, %eax
	leave
	ret
.LtoInt24:
	movl	$0, %eax
	leave
	ret
pullInt:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$32, %rsp
	movl	%edi, -4(%rbp)
	movl	$1, %edx
	movl	-4(%rbp), %eax
	sub	%edx, %eax
	movl	%eax, -8(%rbp)
	movl	$0, -12(%rbp)
	movl	$0, -16(%rbp)
	movl	$0, -20(%rbp)
	jmp	.LpullInt26
.LpullInt25:
	lea	-24(%rbp), %rax
	movq	$1, %rdi
	movq	%rax, %rsi
	movq	$1, %rdx
	movq	$0, %rax
	syscall	
	movl	-24(%rbp), %eax
	movl	%eax, %edi
	call	toInt
	movl	%eax, -16(%rbp)
	movl	$10, %eax
	movl	%eax, %edi
	movl	-8(%rbp), %eax
	movl	%eax, %esi
	call	exp
	movl	%eax, -20(%rbp)
	movl	-20(%rbp), %edx
	movl	-16(%rbp), %eax
	imul	%edx, %eax
	movl	%eax, -16(%rbp)
	mov	-12(%rbp), %edx
	mov	-16(%rbp), %eax
	add	%edx, %eax
	movl	%eax, -12(%rbp)
	movl	$1, %edx
	movl	-8(%rbp), %eax
	sub	%edx, %eax
	movl	%eax, -8(%rbp)
.LpullInt26:
	movl	-8(%rbp), %eax
	movl	$-1, %ecx
	cmpl	%ecx, %eax
	jg	.LpullInt25
	call	clearInput
	movl	-12(%rbp), %eax
	leave
	ret
printInt:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$32, %rsp
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	movl	$1, %edx
	movl	-8(%rbp), %eax
	sub	%edx, %eax
	movl	%eax, -12(%rbp)
	movl	$0, -16(%rbp)
	movl	$0, -20(%rbp)
	movl	$0, -24(%rbp)
	jmp	.LprintInt28
.LprintInt27:
	movl	$10, %eax
	movl	%eax, %edi
	movl	-12(%rbp), %eax
	movl	%eax, %esi
	call	exp
	movl	%eax, -20(%rbp)
	movl	-4(%rbp), %eax
	cltd
	idivl	-20(%rbp)
	movl	%eax, -16(%rbp)
	movl	-16(%rbp), %eax
	movl	%eax, %edi
	call	toChar
	movb	%al, -24(%rbp)
	lea	-24(%rbp), %rax
	movq	$1, %rdi
	movq	%rax, %rsi
	movq	$1, %rdx
	movq	$1, %rax
	syscall	
	movl	-16(%rbp), %edx
	movl	-20(%rbp), %eax
	imul	%edx, %eax
	movl	%eax, -16(%rbp)
	movl	-16(%rbp), %edx
	movl	-4(%rbp), %eax
	sub	%edx, %eax
	movl	%eax, -4(%rbp)
	movl	$1, %edx
	movl	-12(%rbp), %eax
	sub	%edx, %eax
	movl	%eax, -12(%rbp)
.LprintInt28:
	movl	-12(%rbp), %eax
	movl	$-1, %ecx
	cmpl	%ecx, %eax
	jg	.LprintInt27
	movl	$0, %eax
	leave
	ret
