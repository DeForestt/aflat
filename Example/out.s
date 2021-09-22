.global	main


.text

.strmain0:
	.asciz	 "this is a print"
main:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$16, %rsp
	movl	$1, %eax
	movl	%eax, %edi
	movl	$0, %eax
	movl	%eax, %esi
	movl	$20, %eax
	movl	%eax, %edx
	call	grafLin
	movl	$.strmain0, %eax
	movl	%eax, %edi
	call	print
	movl	$0, %eax
	leave
	ret
