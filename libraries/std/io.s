.global	io.clear
.global	io.printFloat
.global	io.printLong
.global	io.printHex
.global	io.printInt
.global	io.pullInt
.global	io.toInt
.global	io.toChar
.global	io.print
.global	io.printChar
.global	io.readString


.text

charToColor:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$16, %rsp
	movb	%dil, -1(%rbp)
	pushq	%rdi
	pushq	%rdx
	movb	$66, %dl
	movb	-1(%rbp), %dil
	cmpb	%dl, %dil
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.LcharToColor0
	movl	$30, %eax
	leave
	ret
.LcharToColor0:
	pushq	%rdi
	pushq	%rdx
	movb	$114, %dl
	movb	-1(%rbp), %dil
	cmpb	%dl, %dil
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.LcharToColor1
	movl	$31, %eax
	leave
	ret
.LcharToColor1:
	pushq	%rdi
	pushq	%rdx
	movb	$103, %dl
	movb	-1(%rbp), %dil
	cmpb	%dl, %dil
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.LcharToColor2
	movl	$32, %eax
	leave
	ret
.LcharToColor2:
	pushq	%rdi
	pushq	%rdx
	movb	$98, %dl
	movb	-1(%rbp), %dil
	cmpb	%dl, %dil
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.LcharToColor3
	movl	$33, %eax
	leave
	ret
.LcharToColor3:
	pushq	%rdi
	pushq	%rdx
	movb	$99, %dl
	movb	-1(%rbp), %dil
	cmpb	%dl, %dil
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.LcharToColor4
	movl	$34, %eax
	leave
	ret
.LcharToColor4:
	pushq	%rdi
	pushq	%rdx
	movb	$109, %dl
	movb	-1(%rbp), %dil
	cmpb	%dl, %dil
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.LcharToColor5
	movl	$35, %eax
	leave
	ret
.LcharToColor5:
	pushq	%rdi
	pushq	%rdx
	movb	$112, %dl
	movb	-1(%rbp), %dil
	cmpb	%dl, %dil
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.LcharToColor6
	movl	$36, %eax
	leave
	ret
.LcharToColor6:
	pushq	%rdi
	pushq	%rdx
	movb	$119, %dl
	movb	-1(%rbp), %dil
	cmpb	%dl, %dil
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.LcharToColor7
	movl	$37, %eax
	leave
	ret
.LcharToColor7:
	leave
	ret
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
	jmp	.LreadString9
.LreadString8:
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
	je	.LreadString10
	movq	-8(%rbp), %rax
	movb	-11(%rbp), %bl
	movb	%bl, (%rax)
.LreadString10:
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
.LreadString9:
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
	je	.LreadString8
	movq	-8(%rbp), %rax
	movb	-11(%rbp), %bl
	movb	%bl, (%rax)
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
	jmp	.LclearInput12
.LclearInput11:
	lea	-2(%rbp), %rax
	movq	$1, %rdi
	movq	%rax, %rsi
	movq	$1, %rdx
	movq	$0, %rax
	syscall	
.LclearInput12:
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
	je	.LclearInput11
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
	movb	%sil, -9(%rbp)
	pushq	%rdx
	pushq	%rdi
	movq	-8(%rbp), %rax
	movq	%rax, %rdi
	call	strings.len
	popq	%rdi
	popq	%rdx
	movl	%eax, %ebx
	movl	%ebx, -13(%rbp)
	pushq	%rdi
	pushq	%rdx
	movb	$0, %dl
	movb	-9(%rbp), %dil
	cmpb	%dl, %dil
	setne	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.Lprint13
	pushq	%rdx
	pushq	%rdi
	movq	$.strprint14, %rax
	movq	%rax, %rdi
	movq	$0, %rsi
	call	print
	popq	%rdi
	popq	%rdx
	pushq	%rdx
	pushq	%rdx
	pushq	%rdi
	movb	-9(%rbp), %al
	movb	%al, %dil
	call	charToColor
	popq	%rdi
	popq	%rdx
	pushq	%rdi
	movl	%eax, %eax
	movl	%eax, %edi
	call	printInt
	popq	%rdi
	popq	%rdx
	pushq	%rdx
	pushq	%rdi
	movb	$109, %al
	movb	%al, %dil
	call	printChar
	popq	%rdi
	popq	%rdx
.Lprint13:
	pushq	%rdx
	pushq	%rdi
	movl	$1, %eax
	movl	%eax, %edi
	pushq	%rsi
	movq	-8(%rbp), %rax
	movq	%rax, %rsi
	pushq	%rdx
	movl	-13(%rbp), %eax
	movl	%eax, %edx
	call	sys_write
	popq	%rdx
	popq	%rsi
	popq	%rdi
	popq	%rdx
	pushq	%rdi
	pushq	%rdx
	movb	$0, %dl
	movb	-9(%rbp), %dil
	cmpb	%dl, %dil
	setne	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.Lprint15
	pushq	%rdx
	pushq	%rdi
	movq	$.strprint16, %rax
	movq	%rax, %rdi
	movq	$0, %rsi
	call	print
	popq	%rdi
	popq	%rdx
.Lprint15:
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
	je	.LtoChar17
	movb	$88, %bl
	movb	%bl, -5(%rbp)
.LtoChar17:
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
	je	.LtoChar18
	movb	$48, %bl
	movb	%bl, -5(%rbp)
.LtoChar18:
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
	je	.LtoChar19
	movb	$49, %bl
	movb	%bl, -5(%rbp)
.LtoChar19:
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
	je	.LtoChar20
	movb	$50, %bl
	movb	%bl, -5(%rbp)
.LtoChar20:
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
	je	.LtoChar21
	movb	$51, %bl
	movb	%bl, -5(%rbp)
.LtoChar21:
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
	je	.LtoChar22
	movb	$52, %bl
	movb	%bl, -5(%rbp)
.LtoChar22:
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
	je	.LtoChar23
	movb	$53, %bl
	movb	%bl, -5(%rbp)
.LtoChar23:
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
	je	.LtoChar24
	movb	$54, %bl
	movb	%bl, -5(%rbp)
.LtoChar24:
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
	je	.LtoChar25
	movb	$55, %bl
	movb	%bl, -5(%rbp)
.LtoChar25:
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
	je	.LtoChar26
	movb	$56, %bl
	movb	%bl, -5(%rbp)
.LtoChar26:
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
	je	.LtoChar27
	movb	$57, %bl
	movb	%bl, -5(%rbp)
.LtoChar27:
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
	je	.LtoChar28
	movb	$65, %bl
	movb	%bl, -5(%rbp)
.LtoChar28:
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
	je	.LtoChar29
	movb	$66, %bl
	movb	%bl, -5(%rbp)
.LtoChar29:
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
	je	.LtoChar30
	movb	$67, %bl
	movb	%bl, -5(%rbp)
.LtoChar30:
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
	je	.LtoChar31
	movb	$68, %bl
	movb	%bl, -5(%rbp)
.LtoChar31:
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
	je	.LtoChar32
	movb	$69, %bl
	movb	%bl, -5(%rbp)
.LtoChar32:
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
	je	.LtoChar33
	movb	$70, %bl
	movb	%bl, -5(%rbp)
.LtoChar33:
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
	je	.LtoInt34
	movl	$0, %eax
	leave
	ret
.LtoInt34:
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
	je	.LtoInt35
	movl	$1, %eax
	leave
	ret
.LtoInt35:
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
	je	.LtoInt36
	movl	$2, %eax
	leave
	ret
.LtoInt36:
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
	je	.LtoInt37
	movl	$3, %eax
	leave
	ret
.LtoInt37:
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
	je	.LtoInt38
	movl	$4, %eax
	leave
	ret
.LtoInt38:
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
	je	.LtoInt39
	movl	$5, %eax
	leave
	ret
.LtoInt39:
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
	je	.LtoInt40
	movl	$6, %eax
	leave
	ret
.LtoInt40:
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
	je	.LtoInt41
	movl	$7, %eax
	leave
	ret
.LtoInt41:
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
	je	.LtoInt42
	movl	$8, %eax
	leave
	ret
.LtoInt42:
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
	je	.LtoInt43
	movl	$9, %eax
	leave
	ret
.LtoInt43:
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
	jmp	.LpullInt45
.LpullInt44:
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
.LpullInt45:
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
	je	.LpullInt44
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
	je	.LprintInt46
	pushq	%rdx
	pushq	%rdi
	movq	$.strprintInt47, %rax
	movq	%rax, %rdi
	movq	$0, %rsi
	call	print
	popq	%rdi
	popq	%rdx
	movl	$0, %eax
	leave
	ret
.LprintInt46:
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
	je	.LprintInt48
	pushq	%rdx
	pushq	%rdi
	movq	$.strprintInt49, %rax
	movq	%rax, %rdi
	movq	$0, %rsi
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
.LprintInt48:
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
	jmp	.LprintInt51
.LprintInt50:
	pushq	%rdi
	pushq	%rdx
	movl	-8(%rbp), %eax
	movl	-12(%rbp), %ecx
	cltd
	idiv	%ecx
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
.LprintInt51:
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
	je	.LprintInt50
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
	jmp	.LprintInt53
.LprintInt52:
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
	idiv	%ecx
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
.LprintInt53:
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
	je	.LprintInt52
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
	je	.LprintHex54
	pushq	%rdx
	pushq	%rdi
	movq	$.strprintHex55, %rax
	movq	%rax, %rdi
	movq	$0, %rsi
	call	print
	popq	%rdi
	popq	%rdx
	movl	$0, %eax
	leave
	ret
.LprintHex54:
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
	je	.LprintHex56
	pushq	%rdx
	pushq	%rdi
	movq	$.strprintHex57, %rax
	movq	%rax, %rdi
	movq	$0, %rsi
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
.LprintHex56:
	movq	-8(%rbp), %rbx
	movq	%rbx, -16(%rbp)
	jmp	.LprintHex59
.LprintHex58:
	pushq	%rdi
	pushq	%rdx
	movq	-16(%rbp), %rax
	movq	-20(%rbp), %rcx
	cltd
	idiv	%rcx
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
.LprintHex59:
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
	je	.LprintHex58
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
	jmp	.LprintHex61
.LprintHex60:
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
	idiv	%rcx
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
.LprintHex61:
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
	je	.LprintHex60
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
	je	.LprintLong62
	pushq	%rdx
	pushq	%rdi
	movq	$.strprintLong63, %rax
	movq	%rax, %rdi
	movq	$0, %rsi
	call	print
	popq	%rdi
	popq	%rdx
	movl	$0, %eax
	leave
	ret
.LprintLong62:
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
	je	.LprintLong64
	pushq	%rdx
	pushq	%rdi
	movq	$.strprintLong65, %rax
	movq	%rax, %rdi
	movq	$0, %rsi
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
.LprintLong64:
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
	jmp	.LprintLong67
.LprintLong66:
	pushq	%rdi
	pushq	%rdx
	movq	-16(%rbp), %rax
	movq	-24(%rbp), %rcx
	cltd
	idiv	%rcx
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
.LprintLong67:
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
	je	.LprintLong66
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
	jmp	.LprintLong69
.LprintLong68:
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
	idiv	%rcx
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
.LprintLong69:
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
	je	.LprintLong68
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
	je	.LprintFloat70
	movl	$-1, %eax
	leave
	ret
.LprintFloat70:
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
	je	.LprintFloat71
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
.LprintFloat71:
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
	jmp	.LprintFloat73
.LprintFloat72:
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
.LprintFloat73:
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
	je	.LprintFloat72
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
	jmp	.LprintFloat75
.LprintFloat74:
	movq	-49(%rbp), %r14
	movq	12(%r14), %rbx
	movq	%rbx, -49(%rbp)
.LprintFloat75:
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
	je	.LprintFloat74
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
	je	.LprintFloat76
	movb	$45, %bl
	movb	%bl, -17(%rbp)
.LprintFloat76:
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
	je	.LprintFloat77
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
	je	.LprintFloat78
	movb	$43, %bl
	movb	%bl, -17(%rbp)
.LprintFloat78:
.LprintFloat77:
	movl	-29(%rbp), %ebx
	movl	%ebx, -65(%rbp)
	movl	$0, %ebx
	movl	%ebx, -73(%rbp)
	jmp	.LprintFloat80
.LprintFloat79:
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
	je	.LprintFloat81
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
.LprintFloat81:
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
.LprintFloat80:
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
	je	.LprintFloat79
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
	jmp	.LprintFloat83
.LprintFloat82:
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
	je	.LprintFloat84
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
.LprintFloat84:
	movq	-49(%rbp), %r14
	movq	4(%r14), %rbx
	movq	%rbx, -49(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	-65(%rbp), %eax
	movl	-77(%rbp), %ecx
	cltd
	idiv	%ecx
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
.LprintFloat83:
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
	je	.LprintFloat82
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
	movq	$.strprintFloat85, %rax
	movq	%rax, %rdi
	movq	$0, %rsi
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
	movq	$.strclear86, %rax
	movq	%rax, %rdi
	movq	$0, %rsi
	call	print
	popq	%rdi
	popq	%rdx
	movl	$0, %eax
	leave
	ret


.data

.strclear86:
	.asciz	 "[1;1H[2J"
.strprintFloat85:
	.asciz	 "."
.strprintLong65:
	.asciz	 "-"
.strprintLong63:
	.asciz	 "0"
.strprintHex57:
	.asciz	 "-"
.strprintHex55:
	.asciz	 "0"
.strprintInt49:
	.asciz	 "-"
.strprintInt47:
	.asciz	 "0"
.strprint16:
	.asciz	 "[0m"
.strprint14:
	.asciz	 "["


.bss

