.global	graphLin


.text

.strgraphLin14:
	.asciz	 "\n"
.strgraphLin13:
	.asciz	 "-"
.strgraphLin10:
	.asciz	 "-"
.strgraphLin9:
	.asciz	 "\n"
.strgraphLin8:
	.asciz	 "-"
.strgraphLin6:
	.asciz	 "*"
.strgraphLin2:
	.asciz	 "|"
graphLin:
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
	jmp	.LgraphLin1
.LgraphLin0:
	movl	$0, -20(%rbp)
	movq	$.strgraphLin2, %rax
	movq	%rax, %rdi
	call	print
	jmp	.LgraphLin4
.LgraphLin3:
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
	jne	.LgraphLin5
	movq	$.strgraphLin6, %rax
	movq	%rax, %rdi
	call	print
.LgraphLin5:
	movl	-16(%rbp), %eax
	movl	-28(%rbp), %ecx
	cmpl	%ecx, %eax
	je	.LgraphLin7
	movq	$.strgraphLin8, %rax
	movq	%rax, %rdi
	call	print
.LgraphLin7:
	mov	-20(%rbp), %edx
	mov	$1, %eax
	add	%edx, %eax
	movl	%eax, -20(%rbp)
.LgraphLin4:
	movl	-20(%rbp), %eax
	movl	-12(%rbp), %ecx
	cmpl	%ecx, %eax
	jl	.LgraphLin3
	movl	$1, %edx
	movl	-16(%rbp), %eax
	sub	%edx, %eax
	movl	%eax, -16(%rbp)
	movq	$.strgraphLin9, %rax
	movq	%rax, %rdi
	call	print
.LgraphLin1:
	movl	-16(%rbp), %eax
	movl	$-1, %ecx
	cmpl	%ecx, %eax
	jg	.LgraphLin0
	movl	$0, -16(%rbp)
	movq	$.strgraphLin10, %rax
	movq	%rax, %rdi
	call	print
	jmp	.LgraphLin12
.LgraphLin11:
	movq	$.strgraphLin13, %rax
	movq	%rax, %rdi
	call	print
	mov	-16(%rbp), %edx
	mov	$1, %eax
	add	%edx, %eax
	movl	%eax, -16(%rbp)
.LgraphLin12:
	movl	-16(%rbp), %eax
	movl	-12(%rbp), %ecx
	cmpl	%ecx, %eax
	jl	.LgraphLin11
	movq	$.strgraphLin14, %rax
	movq	%rax, %rdi
	call	print
	movl	$0, %eax
	leave
	ret
