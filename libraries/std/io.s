.global	clear
.global	printFloat
.global	printChar
.global	printLong
.global	printHex
.global	printInt
.global	pullInt
.global	toInt
.global	toChar
.global	print
.global	clearInput
.global	readString


.text

readString:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$16, %rsp
	movq	%rdi, -8(%rbp)
	movb	$32, %bl
	movb	%bl, -9(%rbp)
	movb	$10, %bl
	movb	%bl, -10(%rbp)
	movb	$0, %bl
	movb	%bl, -11(%rbp)
	jmp	.LreadString1
.LreadString0:
	lea	-9(%rbp), %rax
	movq	$1, %rdi
	movq	%rax, %rsi
	movq	$1, %rdx
	movq	$0, %rax
	syscall	
	movq	-8(%rbp), %rax
	movb	-9(%rbp), %bl
	movb	%bl, (%rax)
	movb	-9(%rbp), %al
	movb	-10(%rbp), %cl
	cmpb	%cl, %al
	jne	.LreadString2
	movq	-8(%rbp), %rax
	movb	-11(%rbp), %bl
	movb	%bl, (%rax)
.LreadString2:
	mov	$1, %edx
	mov	-8(%rbp), %rax
	add	%rdx, %rax
	movl	%eax, %ebx
	movl	%ebx, -8(%rbp)
.LreadString1:
	movb	-9(%rbp), %al
	movb	-10(%rbp), %cl
	cmpb	%cl, %al
	jne	.LreadString0
	movq	-8(%rbp), %rax
	movb	-11(%rbp), %bl
	movb	%bl, (%rax)
	movl	$0, %eax
	leave
	ret
	leave
	ret
clearInput:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$16, %rsp
	movl	$10, %eax
	movl	%eax, %edi
	call	ascii
	movb	%al, %bl
	movb	%bl, -1(%rbp)
	movb	$32, %bl
	movb	%bl, -2(%rbp)
	jmp	.LclearInput6
.LclearInput5:
	lea	-2(%rbp), %rax
	movq	$1, %rdi
	movq	%rax, %rsi
	movq	$1, %rdx
	movq	$0, %rax
	syscall	
.LclearInput6:
	movb	-2(%rbp), %al
	movb	-1(%rbp), %cl
	cmpb	%cl, %al
	jne	.LclearInput5
	movl	$0, %eax
	leave
	ret
	leave
	ret
print:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$16, %rsp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rax
	movq	%rax, %rdi
	call	len
	movl	%eax, %ebx
	movl	%ebx, -12(%rbp)
	movl	$1, %eax
	movl	%eax, %edi
	movq	-8(%rbp), %rax
	movq	%rax, %rsi
	movl	-12(%rbp), %eax
	movl	%eax, %edx
	call	sys_write
	movl	$0, %eax
	leave
	ret
	leave
	ret
toChar:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$16, %rsp
	movl	%edi, -4(%rbp)
	movb	$48, %bl
	movb	%bl, -5(%rbp)
	movl	-4(%rbp), %eax
	movl	$9, %ecx
	cmpl	%ecx, %eax
	jle	.LtoChar7
	movb	$88, %bl
	movb	%bl, -5(%rbp)
.LtoChar7:
	movl	-4(%rbp), %eax
	movl	$0, %ecx
	cmpl	%ecx, %eax
	jne	.LtoChar10
	movb	$48, %bl
	movb	%bl, -5(%rbp)
.LtoChar10:
	movl	-4(%rbp), %eax
	movl	$1, %ecx
	cmpl	%ecx, %eax
	jne	.LtoChar13
	movb	$49, %bl
	movb	%bl, -5(%rbp)
.LtoChar13:
	movl	-4(%rbp), %eax
	movl	$2, %ecx
	cmpl	%ecx, %eax
	jne	.LtoChar16
	movb	$50, %bl
	movb	%bl, -5(%rbp)
.LtoChar16:
	movl	-4(%rbp), %eax
	movl	$3, %ecx
	cmpl	%ecx, %eax
	jne	.LtoChar19
	movb	$51, %bl
	movb	%bl, -5(%rbp)
.LtoChar19:
	movl	-4(%rbp), %eax
	movl	$4, %ecx
	cmpl	%ecx, %eax
	jne	.LtoChar22
	movb	$52, %bl
	movb	%bl, -5(%rbp)
.LtoChar22:
	movl	-4(%rbp), %eax
	movl	$5, %ecx
	cmpl	%ecx, %eax
	jne	.LtoChar25
	movb	$53, %bl
	movb	%bl, -5(%rbp)
.LtoChar25:
	movl	-4(%rbp), %eax
	movl	$6, %ecx
	cmpl	%ecx, %eax
	jne	.LtoChar28
	movb	$54, %bl
	movb	%bl, -5(%rbp)
.LtoChar28:
	movl	-4(%rbp), %eax
	movl	$7, %ecx
	cmpl	%ecx, %eax
	jne	.LtoChar31
	movb	$55, %bl
	movb	%bl, -5(%rbp)
.LtoChar31:
	movl	-4(%rbp), %eax
	movl	$8, %ecx
	cmpl	%ecx, %eax
	jne	.LtoChar34
	movb	$56, %bl
	movb	%bl, -5(%rbp)
.LtoChar34:
	movl	-4(%rbp), %eax
	movl	$9, %ecx
	cmpl	%ecx, %eax
	jne	.LtoChar37
	movb	$57, %bl
	movb	%bl, -5(%rbp)
.LtoChar37:
	movl	-4(%rbp), %eax
	movl	$10, %ecx
	cmpl	%ecx, %eax
	jne	.LtoChar40
	movb	$65, %bl
	movb	%bl, -5(%rbp)
.LtoChar40:
	movl	-4(%rbp), %eax
	movl	$11, %ecx
	cmpl	%ecx, %eax
	jne	.LtoChar43
	movb	$66, %bl
	movb	%bl, -5(%rbp)
.LtoChar43:
	movl	-4(%rbp), %eax
	movl	$12, %ecx
	cmpl	%ecx, %eax
	jne	.LtoChar46
	movb	$67, %bl
	movb	%bl, -5(%rbp)
.LtoChar46:
	movl	-4(%rbp), %eax
	movl	$13, %ecx
	cmpl	%ecx, %eax
	jne	.LtoChar49
	movb	$68, %bl
	movb	%bl, -5(%rbp)
.LtoChar49:
	movl	-4(%rbp), %eax
	movl	$14, %ecx
	cmpl	%ecx, %eax
	jne	.LtoChar52
	movb	$69, %bl
	movb	%bl, -5(%rbp)
.LtoChar52:
	movl	-4(%rbp), %eax
	movl	$15, %ecx
	cmpl	%ecx, %eax
	jne	.LtoChar55
	movb	$70, %bl
	movb	%bl, -5(%rbp)
.LtoChar55:
	movb	-5(%rbp), %al
	leave
	ret
	leave
	ret
toInt:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$16, %rsp
	movb	%dil, -1(%rbp)
	movb	-1(%rbp), %al
	movb	$48, %cl
	cmpb	%cl, %al
	jne	.LtoInt58
	movl	$0, %eax
	leave
	ret
.LtoInt58:
	movb	-1(%rbp), %al
	movb	$49, %cl
	cmpb	%cl, %al
	jne	.LtoInt61
	movl	$1, %eax
	leave
	ret
.LtoInt61:
	movb	-1(%rbp), %al
	movb	$50, %cl
	cmpb	%cl, %al
	jne	.LtoInt64
	movl	$2, %eax
	leave
	ret
.LtoInt64:
	movb	-1(%rbp), %al
	movb	$51, %cl
	cmpb	%cl, %al
	jne	.LtoInt67
	movl	$3, %eax
	leave
	ret
.LtoInt67:
	movb	-1(%rbp), %al
	movb	$52, %cl
	cmpb	%cl, %al
	jne	.LtoInt70
	movl	$4, %eax
	leave
	ret
.LtoInt70:
	movb	-1(%rbp), %al
	movb	$53, %cl
	cmpb	%cl, %al
	jne	.LtoInt73
	movl	$5, %eax
	leave
	ret
.LtoInt73:
	movb	-1(%rbp), %al
	movb	$54, %cl
	cmpb	%cl, %al
	jne	.LtoInt76
	movl	$6, %eax
	leave
	ret
.LtoInt76:
	movb	-1(%rbp), %al
	movb	$55, %cl
	cmpb	%cl, %al
	jne	.LtoInt79
	movl	$7, %eax
	leave
	ret
.LtoInt79:
	movb	-1(%rbp), %al
	movb	$56, %cl
	cmpb	%cl, %al
	jne	.LtoInt82
	movl	$8, %eax
	leave
	ret
.LtoInt82:
	movb	-1(%rbp), %al
	movb	$57, %cl
	cmpb	%cl, %al
	jne	.LtoInt85
	movl	$9, %eax
	leave
	ret
.LtoInt85:
	movl	$-1, %eax
	leave
	ret
	leave
	ret
pullInt:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$32, %rsp
	movl	%edi, -4(%rbp)
	mov	$1, %edx
	mov	-4(%rbp), %eax
	sub	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -8(%rbp)
	movl	$0, %ebx
	movl	%ebx, -12(%rbp)
	movl	$0, %ebx
	movl	%ebx, -16(%rbp)
	movl	$0, %ebx
	movl	%ebx, -20(%rbp)
	jmp	.LpullInt89
.LpullInt88:
	lea	-21(%rbp), %rax
	movq	$1, %rdi
	movq	%rax, %rsi
	movq	$1, %rdx
	movq	$0, %rax
	syscall	
	movb	-21(%rbp), %al
	movb	%al, %dil
	call	toInt
	movl	%eax, %ebx
	movl	%ebx, -16(%rbp)
	movl	$10, %eax
	movl	%eax, %edi
	movl	-8(%rbp), %eax
	movl	%eax, %esi
	call	exp
	movl	%eax, %ebx
	movl	%ebx, -20(%rbp)
	mov	-16(%rbp), %edx
	mov	-20(%rbp), %eax
	imul	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -16(%rbp)
	mov	-16(%rbp), %edx
	mov	-12(%rbp), %eax
	add	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -12(%rbp)
	mov	$1, %edx
	mov	-8(%rbp), %eax
	sub	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -8(%rbp)
.LpullInt89:
	movl	-8(%rbp), %eax
	movl	$-1, %ecx
	cmpl	%ecx, %eax
	jg	.LpullInt88
	call	clearInput
	movl	-12(%rbp), %eax
	leave
	ret
	leave
	ret
printInt:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$32, %rsp
	movl	%edi, -4(%rbp)
	movl	$0, %ebx
	movl	%ebx, -8(%rbp)
	movl	$10, %ebx
	movl	%ebx, -12(%rbp)
	movl	$0, %ebx
	movl	%ebx, -16(%rbp)
	movl	$0, %ebx
	movl	%ebx, -20(%rbp)
	movl	-4(%rbp), %eax
	movl	$0, %ecx
	cmpl	%ecx, %eax
	jne	.LprintInt90
	movq	$.strprintInt93, %rax
	movq	%rax, %rdi
	call	print
	movl	$0, %eax
	leave
	ret
.LprintInt90:
	movl	-4(%rbp), %eax
	movl	$0, %ecx
	cmpl	%ecx, %eax
	jge	.LprintInt94
	movq	$.strprintInt97, %rax
	movq	%rax, %rdi
	call	print
	mov	$-1, %edx
	mov	-4(%rbp), %eax
	imul	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -4(%rbp)
.LprintInt94:
	mov	$1, %edx
	mov	-4(%rbp), %eax
	imul	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -8(%rbp)
	jmp	.LprintInt99
.LprintInt98:
	mov	-8(%rbp), %eax
	cltd
	idivl	-12(%rbp)
	movl	%eax, %ebx
	movl	%ebx, -8(%rbp)
	mov	$1, %edx
	mov	-20(%rbp), %eax
	add	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -20(%rbp)
.LprintInt99:
	movl	-8(%rbp), %eax
	movl	$0, %ecx
	cmpl	%ecx, %eax
	jne	.LprintInt98
	mov	$1, %edx
	mov	-20(%rbp), %eax
	sub	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -20(%rbp)
	jmp	.LprintInt101
.LprintInt100:
	movl	$10, %eax
	movl	%eax, %edi
	movl	-20(%rbp), %eax
	movl	%eax, %esi
	call	exp
	movl	%eax, %ebx
	movl	%ebx, -16(%rbp)
	mov	-4(%rbp), %eax
	cltd
	idivl	-16(%rbp)
	movl	%eax, %ebx
	movl	%ebx, -8(%rbp)
	movl	-8(%rbp), %eax
	movl	%eax, %edi
	call	toChar
	movb	%al, %bl
	movb	%bl, -21(%rbp)
	lea	-21(%rbp), %rax
	movq	$1, %rdi
	movq	%rax, %rsi
	movq	$1, %rdx
	movq	$1, %rax
	syscall	
	mov	-16(%rbp), %edx
	mov	-8(%rbp), %eax
	imul	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -8(%rbp)
	mov	-8(%rbp), %edx
	mov	-4(%rbp), %eax
	sub	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -4(%rbp)
	mov	$1, %edx
	mov	-20(%rbp), %eax
	sub	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -20(%rbp)
.LprintInt101:
	movl	-20(%rbp), %eax
	movl	$-1, %ecx
	cmpl	%ecx, %eax
	jg	.LprintInt100
	movl	$0, %eax
	leave
	ret
	leave
	ret
printHex:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$32, %rsp
	movq	%rdi, -8(%rbp)
	movq	$0, %rbx
	movq	%rbx, -16(%rbp)
	movl	$16, %ebx
	movl	%ebx, -20(%rbp)
	movl	$0, %ebx
	movl	%ebx, -24(%rbp)
	movl	$0, %ebx
	movl	%ebx, -28(%rbp)
	movq	-8(%rbp), %rax
	movq	$0, %rcx
	cmpq	%rcx, %rax
	jne	.LprintHex102
	movq	$.strprintHex105, %rax
	movq	%rax, %rdi
	call	print
	movl	$0, %eax
	leave
	ret
.LprintHex102:
	movq	-8(%rbp), %rax
	movq	$0, %rcx
	cmpq	%rcx, %rax
	jge	.LprintHex106
	movq	$.strprintHex109, %rax
	movq	%rax, %rdi
	call	print
	mov	$-1, %edx
	mov	-8(%rbp), %rax
	imul	%rdx, %rax
	movl	%eax, %ebx
	movl	%ebx, -8(%rbp)
.LprintHex106:
	movq	-8(%rbp), %rbx
	movq	%rbx, -16(%rbp)
	jmp	.LprintHex111
.LprintHex110:
	mov	-16(%rbp), %rax
	cltd
	idivl	-20(%rbp)
	movl	%eax, %ebx
	movl	%ebx, -16(%rbp)
	mov	$1, %edx
	mov	-28(%rbp), %eax
	add	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -28(%rbp)
.LprintHex111:
	movq	-16(%rbp), %rax
	movq	$0, %rcx
	cmpq	%rcx, %rax
	jne	.LprintHex110
	mov	$1, %edx
	mov	-28(%rbp), %eax
	sub	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -28(%rbp)
	jmp	.LprintHex113
.LprintHex112:
	movl	$16, %eax
	movl	%eax, %edi
	movl	-28(%rbp), %eax
	movl	%eax, %esi
	call	exp
	movl	%eax, %ebx
	movl	%ebx, -24(%rbp)
	mov	-8(%rbp), %rax
	cltd
	idivl	-24(%rbp)
	movl	%eax, %ebx
	movl	%ebx, -16(%rbp)
	movq	-16(%rbp), %rax
	movq	%rax, %rdi
	call	toChar
	movb	%al, %bl
	movb	%bl, -29(%rbp)
	lea	-29(%rbp), %rax
	movq	$1, %rdi
	movq	%rax, %rsi
	movq	$1, %rdx
	movq	$1, %rax
	syscall	
	mov	-24(%rbp), %edx
	mov	-16(%rbp), %rax
	imul	%rdx, %rax
	movl	%eax, %ebx
	movl	%ebx, -16(%rbp)
	mov	-16(%rbp), %rdx
	mov	-8(%rbp), %rax
	sub	%rdx, %rax
	movl	%eax, %ebx
	movl	%ebx, -8(%rbp)
	mov	$1, %edx
	mov	-28(%rbp), %eax
	sub	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -28(%rbp)
.LprintHex113:
	movl	-28(%rbp), %eax
	movl	$-1, %ecx
	cmpl	%ecx, %eax
	jg	.LprintHex112
	movl	$0, %eax
	leave
	ret
	leave
	ret
printLong:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$48, %rsp
	movq	%rdi, -8(%rbp)
	movq	$0, %rbx
	movq	%rbx, -16(%rbp)
	movq	$10, %rbx
	movq	%rbx, -24(%rbp)
	movl	$0, %ebx
	movl	%ebx, -28(%rbp)
	movl	$0, %ebx
	movl	%ebx, -32(%rbp)
	movq	-8(%rbp), %rax
	movq	$0, %rcx
	cmpq	%rcx, %rax
	jne	.LprintLong114
	movq	$.strprintLong117, %rax
	movq	%rax, %rdi
	call	print
	movl	$0, %eax
	leave
	ret
.LprintLong114:
	movq	-8(%rbp), %rax
	movq	$0, %rcx
	cmpq	%rcx, %rax
	jge	.LprintLong118
	movq	$.strprintLong121, %rax
	movq	%rax, %rdi
	call	print
	mov	$-1, %edx
	mov	-8(%rbp), %rax
	imul	%rdx, %rax
	movl	%eax, %ebx
	movl	%ebx, -8(%rbp)
.LprintLong118:
	mov	$1, %edx
	mov	-8(%rbp), %rax
	imul	%rdx, %rax
	movl	%eax, %ebx
	movl	%ebx, -16(%rbp)
	jmp	.LprintLong123
.LprintLong122:
	mov	-16(%rbp), %rax
	cltd
	idivl	-24(%rbp)
	movl	%eax, %ebx
	movl	%ebx, -16(%rbp)
	mov	$1, %edx
	mov	-32(%rbp), %eax
	add	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -32(%rbp)
.LprintLong123:
	movq	-16(%rbp), %rax
	movq	$0, %rcx
	cmpq	%rcx, %rax
	jne	.LprintLong122
	mov	$1, %edx
	mov	-32(%rbp), %eax
	sub	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -32(%rbp)
	jmp	.LprintLong125
.LprintLong124:
	movl	$10, %eax
	movl	%eax, %edi
	movl	-32(%rbp), %eax
	movl	%eax, %esi
	call	exp
	movl	%eax, %ebx
	movl	%ebx, -28(%rbp)
	mov	-8(%rbp), %rax
	cltd
	idivl	-28(%rbp)
	movl	%eax, %ebx
	movl	%ebx, -16(%rbp)
	movq	-16(%rbp), %rax
	movq	%rax, %rdi
	call	toChar
	movb	%al, %bl
	movb	%bl, -33(%rbp)
	lea	-33(%rbp), %rax
	movq	$1, %rdi
	movq	%rax, %rsi
	movq	$1, %rdx
	movq	$1, %rax
	syscall	
	mov	-28(%rbp), %edx
	mov	-16(%rbp), %rax
	imul	%rdx, %rax
	movl	%eax, %ebx
	movl	%ebx, -16(%rbp)
	mov	-16(%rbp), %rdx
	mov	-8(%rbp), %rax
	sub	%rdx, %rax
	movl	%eax, %ebx
	movl	%ebx, -8(%rbp)
	mov	$1, %edx
	mov	-32(%rbp), %eax
	sub	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -32(%rbp)
.LprintLong125:
	movl	-32(%rbp), %eax
	movl	$-1, %ecx
	cmpl	%ecx, %eax
	jg	.LprintLong124
	movl	$0, %eax
	leave
	ret
	leave
	ret
printChar:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$16, %rsp
	movb	%dil, -1(%rbp)
	movl	$1, %eax
	movl	%eax, %edi
	lea	-1(%rbp), %rax
	movq	%rax, %rax
	movq	%rax, %rsi
	movl	$1, %eax
	movl	%eax, %edx
	call	sys_write
	movl	$0, %eax
	leave
	ret
	leave
	ret
printFloat:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$96, %rsp
	movq	%rdi, -8(%rbp)
	movl	%esi, -12(%rbp)
	movq	-8(%rbp), %rax
	movq	$0, %rcx
	cmpq	%rcx, %rax
	jne	.LprintFloat126
	movl	$-1, %eax
	leave
	ret
.LprintFloat126:
	movl	$6, %ebx
	movl	%ebx, -16(%rbp)
	movl	$0, %eax
	movl	%eax, %edi
	call	ascii
	movb	%al, %bl
	movb	%bl, -17(%rbp)
	movq	-8(%rbp), %rax
	movl	(%rax), %eax
	movl	%eax, %ebx
	movl	%ebx, -21(%rbp)
	movl	-21(%rbp), %eax
	movl	$0, %ecx
	cmpl	%ecx, %eax
	jne	.LprintFloat129
	movl	$0, %eax
	movl	%eax, %edi
	call	printInt
	movl	$0, %eax
	leave
	ret
.LprintFloat129:
	mov	$31, %edx
	mov	-21(%rbp), %eax
	movl	%edx, %ecx
	sarl	%cl, %eax
	movl	%eax, %ebx
	movl	%ebx, -25(%rbp)
	mov	$23, %edx
	mov	-21(%rbp), %eax
	movl	%edx, %ecx
	sarl	%cl, %eax
	movl	%eax, %ebx
	movl	%ebx, -29(%rbp)
	mov	$255, %edx
	mov	-29(%rbp), %eax
	andl	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -29(%rbp)
	mov	$127, %edx
	mov	-29(%rbp), %eax
	sub	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -29(%rbp)
	mov	$8388607, %edx
	mov	-21(%rbp), %eax
	andl	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -33(%rbp)
	movl	$0, %eax
	movl	%eax, %edi
	call	newBit
	movq	%rax, %rbx
	movq	%rbx, -41(%rbp)
	movq	-41(%rbp), %rbx
	movq	%rbx, -49(%rbp)
	movl	$0, %ebx
	movl	%ebx, -53(%rbp)
	jmp	.LprintFloat133
.LprintFloat132:
	mov	$1, %edx
	mov	-33(%rbp), %eax
	andl	%edx, %eax
	movl	%eax, %eax
	movl	%eax, %edi
	call	newBit
	movq	%rax, %rbx
	movq	%rbx, -61(%rbp)
	movq	-49(%rbp), %rdx
	movq	-61(%rbp), %rbx
	movq	%rbx, 12(%rdx)
	movq	-61(%rbp), %rdx
	movq	-49(%rbp), %rbx
	movq	%rbx, 4(%rdx)
	movq	-61(%rbp), %rbx
	movq	%rbx, -49(%rbp)
	mov	$1, %edx
	mov	-33(%rbp), %eax
	movl	%edx, %ecx
	sarl	%cl, %eax
	movl	%eax, %ebx
	movl	%ebx, -33(%rbp)
	mov	$1, %edx
	mov	-53(%rbp), %eax
	add	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -53(%rbp)
.LprintFloat133:
	movl	-53(%rbp), %eax
	movl	$23, %ecx
	cmpl	%ecx, %eax
	jl	.LprintFloat132
	movl	$1, %eax
	movl	%eax, %edi
	call	newBit
	movq	%rax, %rbx
	movq	%rbx, -61(%rbp)
	movq	-49(%rbp), %rdx
	movq	-61(%rbp), %rbx
	movq	%rbx, 12(%rdx)
	movq	-61(%rbp), %rdx
	movq	-49(%rbp), %rbx
	movq	%rbx, 4(%rdx)
	movq	-61(%rbp), %rbx
	movq	%rbx, -49(%rbp)
	movq	-41(%rbp), %rbx
	movq	%rbx, -49(%rbp)
	jmp	.LprintFloat135
.LprintFloat134:
	movq	-49(%rbp), %rdx
	movq	12(%rdx), %rbx
	movq	%rbx, -49(%rbp)
.LprintFloat135:
	movq	-49(%rbp), %rdx
	movq	12(%rdx), %rax
	movq	$0, %rcx
	cmpq	%rcx, %rax
	jne	.LprintFloat134
	movl	-25(%rbp), %eax
	movl	$0, %ecx
	cmpl	%ecx, %eax
	je	.LprintFloat136
	movb	$45, %bl
	movb	%bl, -17(%rbp)
.LprintFloat136:
	movl	-12(%rbp), %eax
	movl	$1, %ecx
	cmpl	%ecx, %eax
	jne	.LprintFloat139
	movl	-25(%rbp), %eax
	movl	$0, %ecx
	cmpl	%ecx, %eax
	jne	.LprintFloat142
	movb	$43, %bl
	movb	%bl, -17(%rbp)
.LprintFloat142:
.LprintFloat139:
	movl	-29(%rbp), %ebx
	movl	%ebx, -65(%rbp)
	movl	$0, %ebx
	movl	%ebx, -73(%rbp)
	jmp	.LprintFloat146
.LprintFloat145:
	movq	-49(%rbp), %rdx
	movl	0(%rdx), %eax
	movl	$1, %ecx
	cmpl	%ecx, %eax
	jne	.LprintFloat147
	movl	$2, %eax
	movl	%eax, %edi
	movl	-65(%rbp), %eax
	movl	%eax, %esi
	call	exp
	movl	%eax, %ebx
	movl	%ebx, -77(%rbp)
	mov	-77(%rbp), %edx
	mov	-69(%rbp), %eax
	add	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -69(%rbp)
.LprintFloat147:
	movq	-49(%rbp), %rdx
	movq	4(%rdx), %rbx
	movq	%rbx, -49(%rbp)
	mov	$1, %edx
	mov	-65(%rbp), %eax
	sub	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -65(%rbp)
	mov	$1, %edx
	mov	-73(%rbp), %eax
	add	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -73(%rbp)
.LprintFloat146:
	movl	-65(%rbp), %eax
	movl	$-1, %ecx
	cmpl	%ecx, %eax
	jg	.LprintFloat145
	movl	$10, %eax
	movl	%eax, %edi
	movl	-16(%rbp), %eax
	movl	%eax, %esi
	call	exp
	movl	%eax, %ebx
	movl	%ebx, -65(%rbp)
	mov	$5, %edx
	mov	-65(%rbp), %eax
	imul	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -65(%rbp)
	movl	$2, %ebx
	movl	%ebx, -77(%rbp)
	movl	$0, %ebx
	movl	%ebx, -81(%rbp)
	jmp	.LprintFloat151
.LprintFloat150:
	movq	-49(%rbp), %rdx
	movl	0(%rdx), %eax
	movl	$1, %ecx
	cmpl	%ecx, %eax
	jne	.LprintFloat152
	mov	-65(%rbp), %edx
	mov	-81(%rbp), %eax
	add	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -81(%rbp)
.LprintFloat152:
	movq	-49(%rbp), %rdx
	movq	4(%rdx), %rbx
	movq	%rbx, -49(%rbp)
	mov	-65(%rbp), %eax
	cltd
	idivl	-77(%rbp)
	movl	%eax, %ebx
	movl	%ebx, -65(%rbp)
	mov	$1, %edx
	mov	-73(%rbp), %eax
	add	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -73(%rbp)
.LprintFloat151:
	movl	-73(%rbp), %eax
	movl	$23, %ecx
	cmpl	%ecx, %eax
	jl	.LprintFloat150
	lea	-17(%rbp), %rax
	movq	$1, %rdi
	movq	%rax, %rsi
	movq	$1, %rdx
	movq	$1, %rax
	syscall	
	movl	-69(%rbp), %eax
	movl	%eax, %edi
	call	printInt
	movq	$.strprintFloat155, %rax
	movq	%rax, %rdi
	call	print
	movl	-81(%rbp), %eax
	movl	%eax, %edi
	call	printInt
	movl	$0, %eax
	leave
	ret
	leave
	ret
clear:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$16, %rsp
	movq	$.strclear156, %rax
	movq	%rax, %rdi
	call	print
	movl	$0, %eax
	leave
	ret
	leave
	ret


.data

.strclear156:
	.asciz	 "\\033[H\\033[J"
.strprintFloat155:
	.asciz	 "."
.strprintLong121:
	.asciz	 "-"
.strprintLong117:
	.asciz	 "0"
.strprintHex109:
	.asciz	 "-"
.strprintHex105:
	.asciz	 "0"
.strprintInt97:
	.asciz	 "-"
.strprintInt93:
	.asciz	 "0"


.bss

