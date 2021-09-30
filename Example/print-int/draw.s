.global	grafLin


.text

.strgrafLin14:
	.asciz	 "\n"
.strgrafLin13:
	.asciz	 "-"
.strgrafLin10:
	.asciz	 "-"
.strgrafLin9:
	.asciz	 "\n"
.strgrafLin8:
	.asciz	 "-"
.strgrafLin6:
	.asciz	 "*"
.strgrafLin2:
	.asciz	 "|"
grafLin:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$32, %rsp
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	movl	%edx, -12(%rbp)
	movl	-12(%rbp), %edx
	movl	$1, %eax
	imul	%edx, %eax
	movl	%eax, -16(%rbp)
	movl	$0, -20(%rbp)
	jmp	.LgrafLin1
.LgrafLin0:
	movl	$0, -20(%rbp)
	movl	$.strgrafLin2, %eax
	movl	%eax, %edi
	call	print
	jmp	.LgrafLin4
.LgrafLin3:
	movl	-20(%rbp), %edx
	movl	-4(%rbp), %eax
	imul	%edx, %eax
	movl	%eax, -24(%rbp)
	mov	-24(%rbp), %edx
	mov	-8(%rbp), %eax
	add	%edx, %eax
	movl	%eax, -28(%rbp)
	movl	-16(%rbp), %eax
	movl	-28(%rbp), %ecx
	cmpl	%ecx, %eax
	jne	.LgrafLin5
	movl	$.strgrafLin6, %eax
	movl	%eax, %edi
	call	print
.LgrafLin5:
	movl	-16(%rbp), %eax
	movl	-28(%rbp), %ecx
	cmpl	%ecx, %eax
	je	.LgrafLin7
	movl	$.strgrafLin8, %eax
	movl	%eax, %edi
	call	print
.LgrafLin7:
	mov	-20(%rbp), %edx
	mov	$1, %eax
	add	%edx, %eax
	movl	%eax, -20(%rbp)
.LgrafLin4:
	movl	-20(%rbp), %eax
	movl	-12(%rbp), %ecx
	cmpl	%ecx, %eax
	jle	.LgrafLin3
	movl	$1, %edx
	movl	-16(%rbp), %eax
	sub	%edx, %eax
	movl	%eax, -16(%rbp)
	movl	$.strgrafLin9, %eax
	movl	%eax, %edi
	call	print
.LgrafLin1:
	movl	-16(%rbp), %eax
	movl	$-1, %ecx
	cmpl	%ecx, %eax
	jg	.LgrafLin0
	movl	$0, -16(%rbp)
	movl	$.strgrafLin10, %eax
	movl	%eax, %edi
	call	print
	jmp	.LgrafLin12
.LgrafLin11:
	movl	$.strgrafLin13, %eax
	movl	%eax, %edi
	call	print
	mov	-16(%rbp), %edx
	mov	$1, %eax
	add	%edx, %eax
	movl	%eax, -16(%rbp)
.LgrafLin12:
	movl	-16(%rbp), %eax
	movl	-12(%rbp), %ecx
	cmpl	%ecx, %eax
	jle	.LgrafLin11
	movl	$.strgrafLin14, %eax
	movl	%eax, %edi
	call	print
	movl	$0, %eax
	leave
	ret
