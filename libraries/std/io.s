.global	io.clear
.global	io.printFloat
.global	io.printChar
.global	io.printLong
.global	io.printHex
.global	io.printInt
.global	io.pullInt
.global	io.toInt
.global	io.toChar
.global	io.print
.global	io.readString


.text

io.readString:
readString:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
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
	pushq	%rdi
	pushq	%rdx
	movb	-10(%rbp), %dl
	movb	-9(%rbp), %dil
	cmpb	%dl, %dil
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.LreadString2
	movq	-8(%rbp), %rax
	movb	-11(%rbp), %bl
	movb	%bl, (%rax)
.LreadString2:
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movq	-8(%rbp), %rdi
	add	%rdx, %rdi
	movq	%rdi, %rax
	popq	%rdx
	popq	%rdi
	movq	%rax, %rbx
	movq	%rbx, -8(%rbp)
.LreadString1:
	pushq	%rdi
	pushq	%rdx
	movb	-10(%rbp), %dl
	movb	-9(%rbp), %dil
	cmpb	%dl, %dil
	setne	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$1, %al
	je	.LreadString0
	movq	-8(%rbp), %rax
	movb	-11(%rbp), %bl
	movb	%bl, (%rax)
	movl	$0, %eax
	leave
	ret
clearInput:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$16, %rsp
	pushq	%rdx
	pushq	%rdi
	movl	$10, %eax
	movl	%eax, %edi
	call	strings.ascii
	popq	%rdi
	popq	%rdx
	movb	%al, %bl
	movb	%bl, -1(%rbp)
	movb	$32, %bl
	movb	%bl, -2(%rbp)
	jmp	.LclearInput4
.LclearInput3:
	lea	-2(%rbp), %rax
	movq	$1, %rdi
	movq	%rax, %rsi
	movq	$1, %rdx
	movq	$0, %rax
	syscall	
.LclearInput4:
	pushq	%rdi
	pushq	%rdx
	movb	-1(%rbp), %dl
	movb	-2(%rbp), %dil
	cmpb	%dl, %dil
	setne	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$1, %al
	je	.LclearInput3
	movl	$0, %eax
	leave
	ret
io.print:
print:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$16, %rsp
	movq	%rdi, -8(%rbp)
	pushq	%rdx
	movl	-12(%rbp), %r15d
	pushq	%rdi
	movq	-8(%rbp), %rax
	movq	%rax, %rdi
	call	*%r15d
	popq	%rdi
	popq	%rdx
	movl	%eax, %ebx
	movl	%ebx, -12(%rbp)
	pushq	%rdx
	pushq	%rdi
	movl	$1, %eax
	movl	%eax, %edi
	pushq	%rsi
	movq	-8(%rbp), %rax
	movq	%rax, %rsi
	pushq	%rdx
	movl	-12(%rbp), %eax
	movl	%eax, %edx
	call	sys_write
	popq	%rdx
	popq	%rsi
	popq	%rdi
	popq	%rdx
	movl	$0, %eax
	leave
	ret
io.toChar:
toChar:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$16, %rsp
	movl	%edi, -4(%rbp)
	movb	$48, %bl
	movb	%bl, -5(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	$9, %edx
	movl	-4(%rbp), %edi
	cmpl	%edx, %edi
	setg	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.LtoChar5
	movb	$88, %bl
	movb	%bl, -5(%rbp)
.LtoChar5:
	pushq	%rdi
	pushq	%rdx
	movl	$0, %edx
	movl	-4(%rbp), %edi
	cmpl	%edx, %edi
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.LtoChar6
	movb	$48, %bl
	movb	%bl, -5(%rbp)
.LtoChar6:
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movl	-4(%rbp), %edi
	cmpl	%edx, %edi
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.LtoChar7
	movb	$49, %bl
	movb	%bl, -5(%rbp)
.LtoChar7:
	pushq	%rdi
	pushq	%rdx
	movl	$2, %edx
	movl	-4(%rbp), %edi
	cmpl	%edx, %edi
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.LtoChar8
	movb	$50, %bl
	movb	%bl, -5(%rbp)
.LtoChar8:
	pushq	%rdi
	pushq	%rdx
	movl	$3, %edx
	movl	-4(%rbp), %edi
	cmpl	%edx, %edi
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.LtoChar9
	movb	$51, %bl
	movb	%bl, -5(%rbp)
.LtoChar9:
	pushq	%rdi
	pushq	%rdx
	movl	$4, %edx
	movl	-4(%rbp), %edi
	cmpl	%edx, %edi
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.LtoChar10
	movb	$52, %bl
	movb	%bl, -5(%rbp)
.LtoChar10:
	pushq	%rdi
	pushq	%rdx
	movl	$5, %edx
	movl	-4(%rbp), %edi
	cmpl	%edx, %edi
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.LtoChar11
	movb	$53, %bl
	movb	%bl, -5(%rbp)
.LtoChar11:
	pushq	%rdi
	pushq	%rdx
	movl	$6, %edx
	movl	-4(%rbp), %edi
	cmpl	%edx, %edi
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.LtoChar12
	movb	$54, %bl
	movb	%bl, -5(%rbp)
.LtoChar12:
	pushq	%rdi
	pushq	%rdx
	movl	$7, %edx
	movl	-4(%rbp), %edi
	cmpl	%edx, %edi
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.LtoChar13
	movb	$55, %bl
	movb	%bl, -5(%rbp)
.LtoChar13:
	pushq	%rdi
	pushq	%rdx
	movl	$8, %edx
	movl	-4(%rbp), %edi
	cmpl	%edx, %edi
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.LtoChar14
	movb	$56, %bl
	movb	%bl, -5(%rbp)
.LtoChar14:
	pushq	%rdi
	pushq	%rdx
	movl	$9, %edx
	movl	-4(%rbp), %edi
	cmpl	%edx, %edi
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.LtoChar15
	movb	$57, %bl
	movb	%bl, -5(%rbp)
.LtoChar15:
	pushq	%rdi
	pushq	%rdx
	movl	$10, %edx
	movl	-4(%rbp), %edi
	cmpl	%edx, %edi
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.LtoChar16
	movb	$65, %bl
	movb	%bl, -5(%rbp)
.LtoChar16:
	pushq	%rdi
	pushq	%rdx
	movl	$11, %edx
	movl	-4(%rbp), %edi
	cmpl	%edx, %edi
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.LtoChar17
	movb	$66, %bl
	movb	%bl, -5(%rbp)
.LtoChar17:
	pushq	%rdi
	pushq	%rdx
	movl	$12, %edx
	movl	-4(%rbp), %edi
	cmpl	%edx, %edi
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.LtoChar18
	movb	$67, %bl
	movb	%bl, -5(%rbp)
.LtoChar18:
	pushq	%rdi
	pushq	%rdx
	movl	$13, %edx
	movl	-4(%rbp), %edi
	cmpl	%edx, %edi
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.LtoChar19
	movb	$68, %bl
	movb	%bl, -5(%rbp)
.LtoChar19:
	pushq	%rdi
	pushq	%rdx
	movl	$14, %edx
	movl	-4(%rbp), %edi
	cmpl	%edx, %edi
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.LtoChar20
	movb	$69, %bl
	movb	%bl, -5(%rbp)
.LtoChar20:
	pushq	%rdi
	pushq	%rdx
	movl	$15, %edx
	movl	-4(%rbp), %edi
	cmpl	%edx, %edi
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.LtoChar21
	movb	$70, %bl
	movb	%bl, -5(%rbp)
.LtoChar21:
	movb	-5(%rbp), %al
	leave
	ret
io.toInt:
toInt:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$16, %rsp
	movb	%dil, -1(%rbp)
	pushq	%rdi
	pushq	%rdx
	movb	$48, %dl
	movb	-1(%rbp), %dil
	cmpb	%dl, %dil
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.LtoInt22
	movl	$0, %eax
	leave
	ret
.LtoInt22:
	pushq	%rdi
	pushq	%rdx
	movb	$49, %dl
	movb	-1(%rbp), %dil
	cmpb	%dl, %dil
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.LtoInt23
	movl	$1, %eax
	leave
	ret
.LtoInt23:
	pushq	%rdi
	pushq	%rdx
	movb	$50, %dl
	movb	-1(%rbp), %dil
	cmpb	%dl, %dil
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.LtoInt24
	movl	$2, %eax
	leave
	ret
.LtoInt24:
	pushq	%rdi
	pushq	%rdx
	movb	$51, %dl
	movb	-1(%rbp), %dil
	cmpb	%dl, %dil
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.LtoInt25
	movl	$3, %eax
	leave
	ret
.LtoInt25:
	pushq	%rdi
	pushq	%rdx
	movb	$52, %dl
	movb	-1(%rbp), %dil
	cmpb	%dl, %dil
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.LtoInt26
	movl	$4, %eax
	leave
	ret
.LtoInt26:
	pushq	%rdi
	pushq	%rdx
	movb	$53, %dl
	movb	-1(%rbp), %dil
	cmpb	%dl, %dil
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.LtoInt27
	movl	$5, %eax
	leave
	ret
.LtoInt27:
	pushq	%rdi
	pushq	%rdx
	movb	$54, %dl
	movb	-1(%rbp), %dil
	cmpb	%dl, %dil
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.LtoInt28
	movl	$6, %eax
	leave
	ret
.LtoInt28:
	pushq	%rdi
	pushq	%rdx
	movb	$55, %dl
	movb	-1(%rbp), %dil
	cmpb	%dl, %dil
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.LtoInt29
	movl	$7, %eax
	leave
	ret
.LtoInt29:
	pushq	%rdi
	pushq	%rdx
	movb	$56, %dl
	movb	-1(%rbp), %dil
	cmpb	%dl, %dil
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.LtoInt30
	movl	$8, %eax
	leave
	ret
.LtoInt30:
	pushq	%rdi
	pushq	%rdx
	movb	$57, %dl
	movb	-1(%rbp), %dil
	cmpb	%dl, %dil
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.LtoInt31
	movl	$9, %eax
	leave
	ret
.LtoInt31:
	movl	$-1, %eax
	leave
	ret
io.pullInt:
pullInt:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$32, %rsp
	movl	%edi, -4(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movl	-4(%rbp), %edi
	sub	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -8(%rbp)
	movl	$0, %ebx
	movl	%ebx, -12(%rbp)
	movl	$0, %ebx
	movl	%ebx, -16(%rbp)
	movl	$0, %ebx
	movl	%ebx, -20(%rbp)
	jmp	.LpullInt33
.LpullInt32:
	lea	-21(%rbp), %rax
	movq	$1, %rdi
	movq	%rax, %rsi
	movq	$1, %rdx
	movq	$0, %rax
	syscall	
	pushq	%rdx
	pushq	%rdi
	movb	-21(%rbp), %al
	movb	%al, %dil
	call	toInt
	popq	%rdi
	popq	%rdx
	movl	%eax, %ebx
	movl	%ebx, -16(%rbp)
	pushq	%rdx
	pushq	%rdi
	movl	$10, %eax
	movl	%eax, %edi
	pushq	%rsi
	movl	-8(%rbp), %eax
	movl	%eax, %esi
	call	exp
	popq	%rsi
	popq	%rdi
	popq	%rdx
	movl	%eax, %ebx
	movl	%ebx, -20(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	-16(%rbp), %edx
	movl	-20(%rbp), %edi
	imul	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -16(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	-16(%rbp), %edx
	movl	-12(%rbp), %edi
	add	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -12(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movl	-8(%rbp), %edi
	sub	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -8(%rbp)
.LpullInt33:
	pushq	%rdi
	pushq	%rdx
	movl	$-1, %edx
	movl	-8(%rbp), %edi
	cmpl	%edx, %edi
	setg	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$1, %al
	je	.LpullInt32
	pushq	%rdx
	call	clearInput
	popq	%rdx
	movl	-12(%rbp), %eax
	leave
	ret
io.printInt:
printInt:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
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
	pushq	%rdi
	pushq	%rdx
	movl	$0, %edx
	movl	-4(%rbp), %edi
	cmpl	%edx, %edi
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.LprintInt34
	pushq	%rdx
	pushq	%rdi
	movq	$.strprintInt35, %rax
	movq	%rax, %rdi
	call	print
	popq	%rdi
	popq	%rdx
	movl	$0, %eax
	leave
	ret
.LprintInt34:
	pushq	%rdi
	pushq	%rdx
	movl	$0, %edx
	movl	-4(%rbp), %edi
	cmpl	%edx, %edi
	setl	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.LprintInt36
	pushq	%rdx
	pushq	%rdi
	movq	$.strprintInt37, %rax
	movq	%rax, %rdi
	call	print
	popq	%rdi
	popq	%rdx
	pushq	%rdi
	pushq	%rdx
	movl	$-1, %edx
	movl	-4(%rbp), %edi
	imul	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -4(%rbp)
.LprintInt36:
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movl	-4(%rbp), %edi
	imul	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -8(%rbp)
	jmp	.LprintInt39
.LprintInt38:
	pushq	%rdi
	pushq	%rdx
	movl	-8(%rbp), %eax
	movl	-12(%rbp), %ecx
	cltd
	idivl	%ecx
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -8(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movl	-20(%rbp), %edi
	add	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -20(%rbp)
.LprintInt39:
	pushq	%rdi
	pushq	%rdx
	movl	$0, %edx
	movl	-8(%rbp), %edi
	cmpl	%edx, %edi
	setne	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$1, %al
	je	.LprintInt38
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movl	-20(%rbp), %edi
	sub	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -20(%rbp)
	jmp	.LprintInt41
.LprintInt40:
	pushq	%rdx
	pushq	%rdi
	movl	$10, %eax
	movl	%eax, %edi
	pushq	%rsi
	movl	-20(%rbp), %eax
	movl	%eax, %esi
	call	exp
	popq	%rsi
	popq	%rdi
	popq	%rdx
	movl	%eax, %ebx
	movl	%ebx, -16(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	-4(%rbp), %eax
	movl	-16(%rbp), %ecx
	cltd
	idivl	%ecx
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -8(%rbp)
	pushq	%rdx
	pushq	%rdi
	movl	-8(%rbp), %eax
	movl	%eax, %edi
	call	toChar
	popq	%rdi
	popq	%rdx
	movb	%al, %bl
	movb	%bl, -21(%rbp)
	lea	-21(%rbp), %rax
	movq	$1, %rdi
	movq	%rax, %rsi
	movq	$1, %rdx
	movq	$1, %rax
	syscall	
	pushq	%rdi
	pushq	%rdx
	movl	-16(%rbp), %edx
	movl	-8(%rbp), %edi
	imul	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -8(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	-8(%rbp), %edx
	movl	-4(%rbp), %edi
	sub	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -4(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movl	-20(%rbp), %edi
	sub	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -20(%rbp)
.LprintInt41:
	pushq	%rdi
	pushq	%rdx
	movl	$-1, %edx
	movl	-20(%rbp), %edi
	cmpl	%edx, %edi
	setg	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$1, %al
	je	.LprintInt40
	movl	$0, %eax
	leave
	ret
io.printHex:
printHex:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$64, %rsp
	movq	%rdi, -8(%rbp)
	movq	$0, %rbx
	movq	%rbx, -16(%rbp)
	movl	$16, %ebx
	movl	%ebx, -20(%rbp)
	movl	$0, %ebx
	movl	%ebx, -24(%rbp)
	movl	$0, %ebx
	movl	%ebx, -28(%rbp)
	pushq	%rdi
	pushq	%rdx
	movq	$0, %rdx
	movq	-8(%rbp), %rdi
	cmpq	%rdx, %rdi
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.LprintHex42
	pushq	%rdx
	pushq	%rdi
	movq	$.strprintHex43, %rax
	movq	%rax, %rdi
	call	print
	popq	%rdi
	popq	%rdx
	movl	$0, %eax
	leave
	ret
.LprintHex42:
	pushq	%rdi
	pushq	%rdx
	movq	$0, %rdx
	movq	-8(%rbp), %rdi
	cmpq	%rdx, %rdi
	setl	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.LprintHex44
	pushq	%rdx
	pushq	%rdi
	movq	$.strprintHex45, %rax
	movq	%rax, %rdi
	call	print
	popq	%rdi
	popq	%rdx
	pushq	%rdi
	pushq	%rdx
	movl	$-1, %edx
	movq	-8(%rbp), %rdi
	imul	%rdx, %rdi
	movq	%rdi, %rax
	popq	%rdx
	popq	%rdi
	movq	%rax, %rbx
	movq	%rbx, -8(%rbp)
.LprintHex44:
	movq	-8(%rbp), %rbx
	movq	%rbx, -16(%rbp)
	jmp	.LprintHex47
.LprintHex46:
	pushq	%rdi
	pushq	%rdx
	movq	-16(%rbp), %rax
	movq	-20(%rbp), %rcx
	cltd
	idivl	%rcx
	popq	%rdx
	popq	%rdi
	movq	%rax, %rbx
	movq	%rbx, -16(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movl	-28(%rbp), %edi
	add	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -28(%rbp)
.LprintHex47:
	pushq	%rdi
	pushq	%rdx
	movq	$0, %rdx
	movq	-16(%rbp), %rdi
	cmpq	%rdx, %rdi
	setne	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$1, %al
	je	.LprintHex46
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movl	-28(%rbp), %edi
	sub	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -28(%rbp)
	jmp	.LprintHex49
.LprintHex48:
	pushq	%rdx
	pushq	%rdi
	movl	$16, %eax
	movl	%eax, %edi
	pushq	%rsi
	movl	-28(%rbp), %eax
	movl	%eax, %esi
	call	exp
	popq	%rsi
	popq	%rdi
	popq	%rdx
	movl	%eax, %ebx
	movl	%ebx, -24(%rbp)
	pushq	%rdi
	pushq	%rdx
	movq	-8(%rbp), %rax
	movq	-24(%rbp), %rcx
	cltd
	idivl	%rcx
	popq	%rdx
	popq	%rdi
	movq	%rax, %rbx
	movq	%rbx, -16(%rbp)
	lea	-36(%rbp), %rax
	movq	%rax, %rbx
	movq	%rbx, -44(%rbp)
	movq	-44(%rbp), %rax
	movq	-16(%rbp), %rbx
	movq	%rbx, (%rax)
	movq	-44(%rbp), %rax
	movl	(%rax), %eax
	movl	%eax, %ebx
	movl	%ebx, -48(%rbp)
	pushq	%rdx
	pushq	%rdi
	movl	-48(%rbp), %eax
	movl	%eax, %edi
	call	toChar
	popq	%rdi
	popq	%rdx
	movb	%al, %bl
	movb	%bl, -49(%rbp)
	lea	-49(%rbp), %rax
	movq	$1, %rdi
	movq	%rax, %rsi
	movq	$1, %rdx
	movq	$1, %rax
	syscall	
	pushq	%rdi
	pushq	%rdx
	movl	-24(%rbp), %edx
	movq	-16(%rbp), %rdi
	imul	%rdx, %rdi
	movq	%rdi, %rax
	popq	%rdx
	popq	%rdi
	movq	%rax, %rbx
	movq	%rbx, -16(%rbp)
	pushq	%rdi
	pushq	%rdx
	movq	-16(%rbp), %rdx
	movq	-8(%rbp), %rdi
	sub	%rdx, %rdi
	movq	%rdi, %rax
	popq	%rdx
	popq	%rdi
	movq	%rax, %rbx
	movq	%rbx, -8(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movl	-28(%rbp), %edi
	sub	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -28(%rbp)
.LprintHex49:
	pushq	%rdi
	pushq	%rdx
	movl	$-1, %edx
	movl	-28(%rbp), %edi
	cmpl	%edx, %edi
	setg	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$1, %al
	je	.LprintHex48
	movl	$0, %eax
	leave
	ret
io.printLong:
printLong:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$64, %rsp
	movq	%rdi, -8(%rbp)
	movq	$0, %rbx
	movq	%rbx, -16(%rbp)
	movq	$10, %rbx
	movq	%rbx, -24(%rbp)
	movl	$0, %ebx
	movl	%ebx, -28(%rbp)
	movl	$0, %ebx
	movl	%ebx, -32(%rbp)
	pushq	%rdi
	pushq	%rdx
	movq	$0, %rdx
	movq	-8(%rbp), %rdi
	cmpq	%rdx, %rdi
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.LprintLong50
	pushq	%rdx
	pushq	%rdi
	movq	$.strprintLong51, %rax
	movq	%rax, %rdi
	call	print
	popq	%rdi
	popq	%rdx
	movl	$0, %eax
	leave
	ret
.LprintLong50:
	pushq	%rdi
	pushq	%rdx
	movq	$0, %rdx
	movq	-8(%rbp), %rdi
	cmpq	%rdx, %rdi
	setl	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.LprintLong52
	pushq	%rdx
	pushq	%rdi
	movq	$.strprintLong53, %rax
	movq	%rax, %rdi
	call	print
	popq	%rdi
	popq	%rdx
	pushq	%rdi
	pushq	%rdx
	movl	$-1, %edx
	movq	-8(%rbp), %rdi
	imul	%rdx, %rdi
	movq	%rdi, %rax
	popq	%rdx
	popq	%rdi
	movq	%rax, %rbx
	movq	%rbx, -8(%rbp)
.LprintLong52:
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movq	-8(%rbp), %rdi
	imul	%rdx, %rdi
	movq	%rdi, %rax
	popq	%rdx
	popq	%rdi
	movq	%rax, %rbx
	movq	%rbx, -16(%rbp)
	jmp	.LprintLong55
.LprintLong54:
	pushq	%rdi
	pushq	%rdx
	movq	-16(%rbp), %rax
	movq	-24(%rbp), %rcx
	cltd
	idivl	%rcx
	popq	%rdx
	popq	%rdi
	movq	%rax, %rbx
	movq	%rbx, -16(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movl	-32(%rbp), %edi
	add	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -32(%rbp)
.LprintLong55:
	pushq	%rdi
	pushq	%rdx
	movq	$0, %rdx
	movq	-16(%rbp), %rdi
	cmpq	%rdx, %rdi
	setne	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$1, %al
	je	.LprintLong54
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movl	-32(%rbp), %edi
	sub	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -32(%rbp)
	jmp	.LprintLong57
.LprintLong56:
	pushq	%rdx
	pushq	%rdi
	movl	$10, %eax
	movl	%eax, %edi
	pushq	%rsi
	movl	-32(%rbp), %eax
	movl	%eax, %esi
	call	exp
	popq	%rsi
	popq	%rdi
	popq	%rdx
	movl	%eax, %ebx
	movl	%ebx, -28(%rbp)
	pushq	%rdi
	pushq	%rdx
	movq	-8(%rbp), %rax
	movq	-28(%rbp), %rcx
	cltd
	idivl	%rcx
	popq	%rdx
	popq	%rdi
	movq	%rax, %rbx
	movq	%rbx, -16(%rbp)
	lea	-41(%rbp), %rax
	movq	%rax, %rbx
	movq	%rbx, -49(%rbp)
	movq	-49(%rbp), %rax
	movq	-16(%rbp), %rbx
	movq	%rbx, (%rax)
	movq	-49(%rbp), %rax
	movl	(%rax), %eax
	movl	%eax, %ebx
	movl	%ebx, -53(%rbp)
	pushq	%rdx
	pushq	%rdi
	movl	-53(%rbp), %eax
	movl	%eax, %edi
	call	toChar
	popq	%rdi
	popq	%rdx
	movb	%al, %bl
	movb	%bl, -33(%rbp)
	lea	-33(%rbp), %rax
	movq	$1, %rdi
	movq	%rax, %rsi
	movq	$1, %rdx
	movq	$1, %rax
	syscall	
	pushq	%rdi
	pushq	%rdx
	movl	-28(%rbp), %edx
	movq	-16(%rbp), %rdi
	imul	%rdx, %rdi
	movq	%rdi, %rax
	popq	%rdx
	popq	%rdi
	movq	%rax, %rbx
	movq	%rbx, -16(%rbp)
	pushq	%rdi
	pushq	%rdx
	movq	-16(%rbp), %rdx
	movq	-8(%rbp), %rdi
	sub	%rdx, %rdi
	movq	%rdi, %rax
	popq	%rdx
	popq	%rdi
	movq	%rax, %rbx
	movq	%rbx, -8(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movl	-32(%rbp), %edi
	sub	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -32(%rbp)
.LprintLong57:
	pushq	%rdi
	pushq	%rdx
	movl	$-1, %edx
	movl	-32(%rbp), %edi
	cmpl	%edx, %edi
	setg	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$1, %al
	je	.LprintLong56
	movl	$0, %eax
	leave
	ret
io.printChar:
printChar:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$16, %rsp
	movb	%dil, -1(%rbp)
	pushq	%rdx
	pushq	%rdi
	movl	$1, %eax
	movl	%eax, %edi
	lea	-1(%rbp), %rax
	pushq	%rsi
	movq	%rax, %rax
	movq	%rax, %rsi
	pushq	%rdx
	movl	$1, %eax
	movl	%eax, %edx
	call	sys_write
	popq	%rdx
	popq	%rsi
	popq	%rdi
	popq	%rdx
	movl	$0, %eax
	leave
	ret
io.printFloat:
printFloat:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$96, %rsp
	movq	%rdi, -8(%rbp)
	movl	%esi, -12(%rbp)
	pushq	%rdi
	pushq	%rdx
	movq	$0, %rdx
	movq	-8(%rbp), %rdi
	cmpq	%rdx, %rdi
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.LprintFloat58
	movl	$-1, %eax
	leave
	ret
.LprintFloat58:
	movl	$6, %ebx
	movl	%ebx, -16(%rbp)
	pushq	%rdx
	pushq	%rdi
	movl	$0, %eax
	movl	%eax, %edi
	call	strings.ascii
	popq	%rdi
	popq	%rdx
	movb	%al, %bl
	movb	%bl, -17(%rbp)
	movq	-8(%rbp), %rax
	movl	(%rax), %eax
	movl	%eax, %ebx
	movl	%ebx, -21(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	$0, %edx
	movl	-21(%rbp), %edi
	cmpl	%edx, %edi
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.LprintFloat59
	pushq	%rdx
	pushq	%rdi
	movl	$0, %eax
	movl	%eax, %edi
	call	printInt
	popq	%rdi
	popq	%rdx
	movl	$0, %eax
	leave
	ret
.LprintFloat59:
	pushq	%rdi
	pushq	%rdx
	movl	$31, %edx
	movl	-21(%rbp), %edi
	movl	%edi, %ecx
	sarl	%cl, %edi
	popq	%rdx
	popq	%rdi
	movl	%edi, %ebx
	movl	%ebx, -25(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	$23, %edx
	movl	-21(%rbp), %edi
	movl	%edi, %ecx
	sarl	%cl, %edi
	popq	%rdx
	popq	%rdi
	movl	%edi, %ebx
	movl	%ebx, -29(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	$255, %edx
	movl	-29(%rbp), %edi
	andl	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -29(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	$127, %edx
	movl	-29(%rbp), %edi
	sub	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -29(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	$8388607, %edx
	movl	-21(%rbp), %edi
	andl	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -33(%rbp)
	pushq	%rdx
	pushq	%rdi
	movl	$0, %eax
	movl	%eax, %edi
	call	newBit
	popq	%rdi
	popq	%rdx
	movq	%rax, %rbx
	movq	%rbx, -41(%rbp)
	movq	-41(%rbp), %rbx
	movq	%rbx, -49(%rbp)
	movl	$0, %ebx
	movl	%ebx, -53(%rbp)
	jmp	.LprintFloat61
.LprintFloat60:
	pushq	%rdx
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movl	-33(%rbp), %edi
	andl	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	pushq	%rdi
	movl	%eax, %eax
	movl	%eax, %edi
	call	newBit
	popq	%rdi
	popq	%rdx
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
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movl	-33(%rbp), %edi
	movl	%edi, %ecx
	sarl	%cl, %edi
	popq	%rdx
	popq	%rdi
	movl	%edi, %ebx
	movl	%ebx, -33(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movl	-53(%rbp), %edi
	add	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -53(%rbp)
.LprintFloat61:
	pushq	%rdi
	pushq	%rdx
	movl	$23, %edx
	movl	-53(%rbp), %edi
	cmpl	%edx, %edi
	setl	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$1, %al
	je	.LprintFloat60
	pushq	%rdx
	pushq	%rdi
	movl	$1, %eax
	movl	%eax, %edi
	call	newBit
	popq	%rdi
	popq	%rdx
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
	jmp	.LprintFloat63
.LprintFloat62:
	movq	-49(%rbp), %r14
	movq	12(%r14), %rbx
	movq	%rbx, -49(%rbp)
.LprintFloat63:
	pushq	%rdi
	pushq	%rdx
	movq	$0, %rdx
	movq	-49(%rbp), %r14
	movq	12(%r14), %rdi
	cmpq	%rdx, %rdi
	setne	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$1, %al
	je	.LprintFloat62
	pushq	%rdi
	pushq	%rdx
	movl	$0, %edx
	movl	-25(%rbp), %edi
	cmpl	%edx, %edi
	setne	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.LprintFloat64
	movb	$45, %bl
	movb	%bl, -17(%rbp)
.LprintFloat64:
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movl	-12(%rbp), %edi
	cmpl	%edx, %edi
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.LprintFloat65
	pushq	%rdi
	pushq	%rdx
	movl	$0, %edx
	movl	-25(%rbp), %edi
	cmpl	%edx, %edi
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.LprintFloat66
	movb	$43, %bl
	movb	%bl, -17(%rbp)
.LprintFloat66:
.LprintFloat65:
	movl	-29(%rbp), %ebx
	movl	%ebx, -65(%rbp)
	movl	$0, %ebx
	movl	%ebx, -73(%rbp)
	jmp	.LprintFloat68
.LprintFloat67:
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movq	-49(%rbp), %r14
	movl	0(%r14), %edi
	cmpl	%edx, %edi
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.LprintFloat69
	pushq	%rdx
	pushq	%rdi
	movl	$2, %eax
	movl	%eax, %edi
	pushq	%rsi
	movl	-65(%rbp), %eax
	movl	%eax, %esi
	call	exp
	popq	%rsi
	popq	%rdi
	popq	%rdx
	movl	%eax, %ebx
	movl	%ebx, -77(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	-77(%rbp), %edx
	movl	-69(%rbp), %edi
	add	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -69(%rbp)
.LprintFloat69:
	movq	-49(%rbp), %r14
	movq	4(%r14), %rbx
	movq	%rbx, -49(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movl	-65(%rbp), %edi
	sub	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -65(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movl	-73(%rbp), %edi
	add	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -73(%rbp)
.LprintFloat68:
	pushq	%rdi
	pushq	%rdx
	movl	$-1, %edx
	movl	-65(%rbp), %edi
	cmpl	%edx, %edi
	setg	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$1, %al
	je	.LprintFloat67
	pushq	%rdx
	pushq	%rdi
	movl	$10, %eax
	movl	%eax, %edi
	pushq	%rsi
	movl	-16(%rbp), %eax
	movl	%eax, %esi
	call	exp
	popq	%rsi
	popq	%rdi
	popq	%rdx
	movl	%eax, %ebx
	movl	%ebx, -65(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	$5, %edx
	movl	-65(%rbp), %edi
	imul	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -65(%rbp)
	movl	$2, %ebx
	movl	%ebx, -77(%rbp)
	movl	$0, %ebx
	movl	%ebx, -81(%rbp)
	jmp	.LprintFloat71
.LprintFloat70:
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movq	-49(%rbp), %r14
	movl	0(%r14), %edi
	cmpl	%edx, %edi
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.LprintFloat72
	pushq	%rdi
	pushq	%rdx
	movl	-65(%rbp), %edx
	movl	-81(%rbp), %edi
	add	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -81(%rbp)
.LprintFloat72:
	movq	-49(%rbp), %r14
	movq	4(%r14), %rbx
	movq	%rbx, -49(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	-65(%rbp), %eax
	movl	-77(%rbp), %ecx
	cltd
	idivl	%ecx
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -65(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movl	-73(%rbp), %edi
	add	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -73(%rbp)
.LprintFloat71:
	pushq	%rdi
	pushq	%rdx
	movl	$23, %edx
	movl	-73(%rbp), %edi
	cmpl	%edx, %edi
	setl	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$1, %al
	je	.LprintFloat70
	lea	-17(%rbp), %rax
	movq	$1, %rdi
	movq	%rax, %rsi
	movq	$1, %rdx
	movq	$1, %rax
	syscall	
	pushq	%rdx
	pushq	%rdi
	movl	-69(%rbp), %eax
	movl	%eax, %edi
	call	printInt
	popq	%rdi
	popq	%rdx
	pushq	%rdx
	pushq	%rdi
	movq	$.strprintFloat73, %rax
	movq	%rax, %rdi
	call	print
	popq	%rdi
	popq	%rdx
	pushq	%rdx
	pushq	%rdi
	movl	-81(%rbp), %eax
	movl	%eax, %edi
	call	printInt
	popq	%rdi
	popq	%rdx
	movl	$0, %eax
	leave
	ret
io.clear:
clear:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$16, %rsp
	pushq	%rdx
	pushq	%rdi
	movq	$.strclear74, %rax
	movq	%rax, %rdi
	call	print
	popq	%rdi
	popq	%rdx
	movl	$0, %eax
	leave
	ret


.data

.strclear74:
	.asciz	 "[1;1H[2J"
.strprintFloat73:
	.asciz	 "."
.strprintLong53:
	.asciz	 "-"
.strprintLong51:
	.asciz	 "0"
.strprintHex45:
	.asciz	 "-"
.strprintHex43:
	.asciz	 "0"
.strprintInt37:
	.asciz	 "-"
.strprintInt35:
	.asciz	 "0"


.bss

