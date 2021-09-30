.global	main


.text

.strmain4:
	.asciz	 "\n"
.strmain3:
	.asciz	 "Div Result: "
.strmain2:
	.asciz	 "please intput the second Number: "
.strmain1:
	.asciz	 "Please input the first Number: "
.strmain0:
	.asciz	 "How Many digits in each number?(2 digits): "
main:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$16, %rsp
	movq	$.strmain0, %rax
	movq	%rax, %rdi
	call	print
	movl	$2, %eax
	movl	%eax, %edi
	call	pullInt
	movl	%eax, -12(%rbp)
	movq	$.strmain1, %rax
	movq	%rax, %rdi
	call	print
	movl	-12(%rbp), %eax
	movl	%eax, %edi
	call	pullInt
	movl	%eax, -4(%rbp)
	movq	$.strmain2, %rax
	movq	%rax, %rdi
	call	print
	movl	-12(%rbp), %eax
	movl	%eax, %edi
	call	pullInt
	movl	%eax, -8(%rbp)
	movq	$.strmain3, %rax
	movq	%rax, %rdi
	call	print
	movl	-4(%rbp), %eax
	cltd
	idivl	-8(%rbp)
	movl	%eax, -16(%rbp)
	movl	-16(%rbp), %eax
	movl	%eax, %edi
	movl	-12(%rbp), %eax
	movl	%eax, %esi
	call	printInt
	movq	$.strmain4, %rax
	movq	%rax, %rdi
	call	print
	movl	$0, %eax
	leave
	ret
