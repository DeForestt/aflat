.global	main


.text

.strmain4:
	.asciz	 " every month.\n"
.strmain3:
	.asciz	 "You will need to save $"
.strmain2:
	.asciz	 "How many months do you have to save? \n(Please fill all 3 digits eg '012'): "
.strmain1:
	.asciz	 "\n\n\nHow many months do you have to Save?\n(Please fill all 7 digits eg '0010000')?: "
.strmain0:
	.asciz	 "***SAVINGS GOAL CALCULATOR***"
main:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$32, %rsp
	movq	$.strmain0, %rax
	movq	%rax, %rdi
	call	print
	movq	$.strmain1, %rax
	movq	%rax, %rdi
	call	print
	movl	$7, %eax
	movl	%eax, %edi
	call	pullInt
	movl	%eax, -4(%rbp)
	movq	$.strmain2, %rax
	movq	%rax, %rdi
	call	print
	movl	$3, %eax
	movl	%eax, %edi
	call	pullInt
	movl	%eax, -8(%rbp)
	movq	$.strmain3, %rax
	movq	%rax, %rdi
	call	print
	movl	-12(%rbp), %eax
	movl	%eax, %edi
	movl	$5, %eax
	movl	%eax, %esi
	call	printInt
	movq	$.strmain4, %rax
	movq	%rax, %rdi
	call	print
	movl	$1000, -16(%rbp)
	movl	-4(%rbp), %eax
	cltd
	idivl	-16(%rbp)
	movl	%eax, -20(%rbp)
	movl	-20(%rbp), %eax
	cltd
	idivl	-8(%rbp)
	movl	%eax, %eax
	movl	%eax, %edi
	movl	$0, %eax
	movl	%eax, %esi
	movl	-20(%rbp), %eax
	movl	%eax, %edx
	call	graphLin
	movl	$0, %eax
	leave
	ret
