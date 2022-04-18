.global	io.clear
.global	io.printFloat
.global	io.printLong
.global	io.printHex
.global	io.printUnsignedInt
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
	movb	-1(%rbp), %r15b
	movb	%r15b, %dil
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
	movb	-1(%rbp), %r15b
	movb	%r15b, %dil
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
	movb	-1(%rbp), %r15b
	movb	%r15b, %dil
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
	movb	-1(%rbp), %r15b
	movb	%r15b, %dil
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
	movb	-1(%rbp), %r15b
	movb	%r15b, %dil
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
	movb	-1(%rbp), %r15b
	movb	%r15b, %dil
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
	movb	-1(%rbp), %r15b
	movb	%r15b, %dil
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
	movb	-1(%rbp), %r15b
	movb	%r15b, %dil
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
	pushq	%rdx
	pushq	%rdi
	movl	$0, %eax
	movl	%eax, %edi
	lea	-9(%rbp), %rax
	pushq	%rsi
	movq	%rax, %rax
	movq	%rax, %rsi
	pushq	%rdx
	movl	$1, %eax
	movl	%eax, %edx
	call	sys_read
	popq	%rdx
	popq	%rsi
	popq	%rdi
	popq	%rdx
	movb	-9(%rbp), %r15b
	movq	-8(%rbp), %rax
	movb	%r15b, %bl
	movb	%bl, (%rax)
	pushq	%rdi
	pushq	%rdx
	movb	-10(%rbp), %r15b
	movb	%r15b, %dl
	movb	-9(%rbp), %r15b
	movb	%r15b, %dil
	cmpb	%dl, %dil
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.LreadString10
	movb	-11(%rbp), %r15b
	movq	-8(%rbp), %rax
	movb	%r15b, %bl
	movb	%bl, (%rax)
.LreadString10:
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movq	-8(%rbp), %r15
	movq	%r15, %rdi
	add	%rdx, %rdi
	movq	%rdi, %rax
	popq	%rdx
	popq	%rdi
	movq	%rax, %rbx
	movq	%rbx, -8(%rbp)
.LreadString9:
	pushq	%rdi
	pushq	%rdx
	movb	-10(%rbp), %r15b
	movb	%r15b, %dl
	movb	-9(%rbp), %r15b
	movb	%r15b, %dil
	cmpb	%dl, %dil
	setne	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$1, %al
	je	.LreadString8
	movb	-11(%rbp), %r15b
	movq	-8(%rbp), %rax
	movb	%r15b, %bl
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
	pushq	%rdx
	pushq	%rdi
	movl	$0, %eax
	movl	%eax, %edi
	lea	-2(%rbp), %rax
	pushq	%rsi
	movq	%rax, %rax
	movq	%rax, %rsi
	pushq	%rdx
	movl	$1, %eax
	movl	%eax, %edx
	call	sys_read
	popq	%rdx
	popq	%rsi
	popq	%rdi
	popq	%rdx
.LclearInput12:
	pushq	%rdi
	pushq	%rdx
	movb	-1(%rbp), %r15b
	movb	%r15b, %dl
	movb	-2(%rbp), %r15b
	movb	%r15b, %dil
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
	movq	-8(%rbp), %r15
	pushq	%rdi
	movq	%r15, %rax
	movq	%rax, %rdi
	call	strings.len
	popq	%rdi
	popq	%rdx
	movl	%eax, %ebx
	movl	%ebx, -13(%rbp)
	pushq	%rdi
	pushq	%rdx
	movb	$0, %dl
	movb	-9(%rbp), %r15b
	movb	%r15b, %dil
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
	movb	-9(%rbp), %r15b
	pushq	%rdi
	movb	%r15b, %al
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
	movq	-8(%rbp), %r15
	pushq	%rsi
	movq	%r15, %rax
	movq	%rax, %rsi
	movl	-13(%rbp), %r15d
	pushq	%rdx
	movl	%r15d, %eax
	movl	%eax, %edx
	call	sys_write
	popq	%rdx
	popq	%rsi
	popq	%rdi
	popq	%rdx
	pushq	%rdi
	pushq	%rdx
	movb	$0, %dl
	movb	-9(%rbp), %r15b
	movb	%r15b, %dil
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
	movl	-4(%rbp), %r15d
	movl	%r15d, %edi
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
	movl	-4(%rbp), %r15d
	movl	%r15d, %edi
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
	movl	-4(%rbp), %r15d
	movl	%r15d, %edi
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
	movl	-4(%rbp), %r15d
	movl	%r15d, %edi
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
	movl	-4(%rbp), %r15d
	movl	%r15d, %edi
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
	movl	-4(%rbp), %r15d
	movl	%r15d, %edi
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
	movl	-4(%rbp), %r15d
	movl	%r15d, %edi
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
	movl	-4(%rbp), %r15d
	movl	%r15d, %edi
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
	movl	-4(%rbp), %r15d
	movl	%r15d, %edi
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
	movl	-4(%rbp), %r15d
	movl	%r15d, %edi
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
	movl	-4(%rbp), %r15d
	movl	%r15d, %edi
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
	movl	-4(%rbp), %r15d
	movl	%r15d, %edi
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
	movl	-4(%rbp), %r15d
	movl	%r15d, %edi
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
	movl	-4(%rbp), %r15d
	movl	%r15d, %edi
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
	movl	-4(%rbp), %r15d
	movl	%r15d, %edi
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
	movl	-4(%rbp), %r15d
	movl	%r15d, %edi
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
	movl	-4(%rbp), %r15d
	movl	%r15d, %edi
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
	movb	-5(%rbp), %r15b
	movb	%r15b, %al
	leave
	ret
longtoChar:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$16, %rsp
	movq	%rdi, -8(%rbp)
	pushq	%rdi
	pushq	%rdx
	movq	$0, %rdx
	movq	-8(%rbp), %r15
	movq	%r15, %rdi
	cmpq	%rdx, %rdi
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.LlongtoChar34
	movb	$48, %al
	leave
	ret
.LlongtoChar34:
	pushq	%rdi
	pushq	%rdx
	movq	$1, %rdx
	movq	-8(%rbp), %r15
	movq	%r15, %rdi
	cmpq	%rdx, %rdi
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.LlongtoChar35
	movb	$49, %al
	leave
	ret
.LlongtoChar35:
	pushq	%rdi
	pushq	%rdx
	movq	$2, %rdx
	movq	-8(%rbp), %r15
	movq	%r15, %rdi
	cmpq	%rdx, %rdi
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.LlongtoChar36
	movb	$50, %al
	leave
	ret
.LlongtoChar36:
	pushq	%rdi
	pushq	%rdx
	movq	$3, %rdx
	movq	-8(%rbp), %r15
	movq	%r15, %rdi
	cmpq	%rdx, %rdi
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.LlongtoChar37
	movb	$51, %al
	leave
	ret
.LlongtoChar37:
	pushq	%rdi
	pushq	%rdx
	movq	$4, %rdx
	movq	-8(%rbp), %r15
	movq	%r15, %rdi
	cmpq	%rdx, %rdi
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.LlongtoChar38
	movb	$52, %al
	leave
	ret
.LlongtoChar38:
	pushq	%rdi
	pushq	%rdx
	movq	$5, %rdx
	movq	-8(%rbp), %r15
	movq	%r15, %rdi
	cmpq	%rdx, %rdi
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.LlongtoChar39
	movb	$53, %al
	leave
	ret
.LlongtoChar39:
	pushq	%rdi
	pushq	%rdx
	movq	$6, %rdx
	movq	-8(%rbp), %r15
	movq	%r15, %rdi
	cmpq	%rdx, %rdi
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.LlongtoChar40
	movb	$54, %al
	leave
	ret
.LlongtoChar40:
	pushq	%rdi
	pushq	%rdx
	movq	$7, %rdx
	movq	-8(%rbp), %r15
	movq	%r15, %rdi
	cmpq	%rdx, %rdi
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.LlongtoChar41
	movb	$55, %al
	leave
	ret
.LlongtoChar41:
	pushq	%rdi
	pushq	%rdx
	movq	$8, %rdx
	movq	-8(%rbp), %r15
	movq	%r15, %rdi
	cmpq	%rdx, %rdi
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.LlongtoChar42
	movb	$56, %al
	leave
	ret
.LlongtoChar42:
	pushq	%rdi
	pushq	%rdx
	movq	$9, %rdx
	movq	-8(%rbp), %r15
	movq	%r15, %rdi
	cmpq	%rdx, %rdi
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.LlongtoChar43
	movb	$57, %al
	leave
	ret
.LlongtoChar43:
	pushq	%rdi
	pushq	%rdx
	movq	$10, %rdx
	movq	-8(%rbp), %r15
	movq	%r15, %rdi
	cmpq	%rdx, %rdi
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.LlongtoChar44
	movb	$65, %al
	leave
	ret
.LlongtoChar44:
	pushq	%rdi
	pushq	%rdx
	movq	$11, %rdx
	movq	-8(%rbp), %r15
	movq	%r15, %rdi
	cmpq	%rdx, %rdi
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.LlongtoChar45
	movb	$66, %al
	leave
	ret
.LlongtoChar45:
	pushq	%rdi
	pushq	%rdx
	movq	$12, %rdx
	movq	-8(%rbp), %r15
	movq	%r15, %rdi
	cmpq	%rdx, %rdi
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.LlongtoChar46
	movb	$67, %al
	leave
	ret
.LlongtoChar46:
	pushq	%rdi
	pushq	%rdx
	movq	$13, %rdx
	movq	-8(%rbp), %r15
	movq	%r15, %rdi
	cmpq	%rdx, %rdi
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.LlongtoChar47
	movb	$68, %al
	leave
	ret
.LlongtoChar47:
	pushq	%rdi
	pushq	%rdx
	movq	$14, %rdx
	movq	-8(%rbp), %r15
	movq	%r15, %rdi
	cmpq	%rdx, %rdi
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.LlongtoChar48
	movb	$69, %al
	leave
	ret
.LlongtoChar48:
	pushq	%rdi
	pushq	%rdx
	movq	$15, %rdx
	movq	-8(%rbp), %r15
	movq	%r15, %rdi
	cmpq	%rdx, %rdi
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.LlongtoChar49
	movb	$70, %al
	leave
	ret
.LlongtoChar49:
	movb	$88, %al
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
	movb	-1(%rbp), %r15b
	movb	%r15b, %dil
	cmpb	%dl, %dil
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.LtoInt50
	movl	$0, %eax
	leave
	ret
.LtoInt50:
	pushq	%rdi
	pushq	%rdx
	movb	$49, %dl
	movb	-1(%rbp), %r15b
	movb	%r15b, %dil
	cmpb	%dl, %dil
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.LtoInt51
	movl	$1, %eax
	leave
	ret
.LtoInt51:
	pushq	%rdi
	pushq	%rdx
	movb	$50, %dl
	movb	-1(%rbp), %r15b
	movb	%r15b, %dil
	cmpb	%dl, %dil
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.LtoInt52
	movl	$2, %eax
	leave
	ret
.LtoInt52:
	pushq	%rdi
	pushq	%rdx
	movb	$51, %dl
	movb	-1(%rbp), %r15b
	movb	%r15b, %dil
	cmpb	%dl, %dil
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.LtoInt53
	movl	$3, %eax
	leave
	ret
.LtoInt53:
	pushq	%rdi
	pushq	%rdx
	movb	$52, %dl
	movb	-1(%rbp), %r15b
	movb	%r15b, %dil
	cmpb	%dl, %dil
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.LtoInt54
	movl	$4, %eax
	leave
	ret
.LtoInt54:
	pushq	%rdi
	pushq	%rdx
	movb	$53, %dl
	movb	-1(%rbp), %r15b
	movb	%r15b, %dil
	cmpb	%dl, %dil
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.LtoInt55
	movl	$5, %eax
	leave
	ret
.LtoInt55:
	pushq	%rdi
	pushq	%rdx
	movb	$54, %dl
	movb	-1(%rbp), %r15b
	movb	%r15b, %dil
	cmpb	%dl, %dil
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.LtoInt56
	movl	$6, %eax
	leave
	ret
.LtoInt56:
	pushq	%rdi
	pushq	%rdx
	movb	$55, %dl
	movb	-1(%rbp), %r15b
	movb	%r15b, %dil
	cmpb	%dl, %dil
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.LtoInt57
	movl	$7, %eax
	leave
	ret
.LtoInt57:
	pushq	%rdi
	pushq	%rdx
	movb	$56, %dl
	movb	-1(%rbp), %r15b
	movb	%r15b, %dil
	cmpb	%dl, %dil
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.LtoInt58
	movl	$8, %eax
	leave
	ret
.LtoInt58:
	pushq	%rdi
	pushq	%rdx
	movb	$57, %dl
	movb	-1(%rbp), %r15b
	movb	%r15b, %dil
	cmpb	%dl, %dil
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.LtoInt59
	movl	$9, %eax
	leave
	ret
.LtoInt59:
	pushq	%rdi
	pushq	%rdx
	movb	$65, %dl
	movb	-1(%rbp), %r15b
	movb	%r15b, %dil
	cmpb	%dl, %dil
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.LtoInt60
	movl	$10, %eax
	leave
	ret
.LtoInt60:
	pushq	%rdi
	pushq	%rdx
	movb	$66, %dl
	movb	-1(%rbp), %r15b
	movb	%r15b, %dil
	cmpb	%dl, %dil
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.LtoInt61
	movl	$11, %eax
	leave
	ret
.LtoInt61:
	pushq	%rdi
	pushq	%rdx
	movb	$67, %dl
	movb	-1(%rbp), %r15b
	movb	%r15b, %dil
	cmpb	%dl, %dil
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.LtoInt62
	movl	$12, %eax
	leave
	ret
.LtoInt62:
	pushq	%rdi
	pushq	%rdx
	movb	$68, %dl
	movb	-1(%rbp), %r15b
	movb	%r15b, %dil
	cmpb	%dl, %dil
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.LtoInt63
	movl	$13, %eax
	leave
	ret
.LtoInt63:
	pushq	%rdi
	pushq	%rdx
	movb	$69, %dl
	movb	-1(%rbp), %r15b
	movb	%r15b, %dil
	cmpb	%dl, %dil
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.LtoInt64
	movl	$14, %eax
	leave
	ret
.LtoInt64:
	pushq	%rdi
	pushq	%rdx
	movb	$70, %dl
	movb	-1(%rbp), %r15b
	movb	%r15b, %dil
	cmpb	%dl, %dil
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.LtoInt65
	movl	$15, %eax
	leave
	ret
.LtoInt65:
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
	movl	-4(%rbp), %r15d
	movl	%r15d, %edi
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
	jmp	.LpullInt67
.LpullInt66:
	pushq	%rdx
	pushq	%rdi
	movl	$0, %eax
	movl	%eax, %edi
	lea	-21(%rbp), %rax
	pushq	%rsi
	movq	%rax, %rax
	movq	%rax, %rsi
	pushq	%rdx
	movl	$1, %eax
	movl	%eax, %edx
	call	sys_read
	popq	%rdx
	popq	%rsi
	popq	%rdi
	popq	%rdx
	pushq	%rdx
	movb	-21(%rbp), %r15b
	pushq	%rdi
	movb	%r15b, %al
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
	movl	-8(%rbp), %r15d
	pushq	%rsi
	movl	%r15d, %eax
	movl	%eax, %esi
	call	math.exp
	popq	%rsi
	popq	%rdi
	popq	%rdx
	movl	%eax, %ebx
	movl	%ebx, -20(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	-16(%rbp), %r15d
	movl	%r15d, %edx
	movl	-20(%rbp), %r15d
	movl	%r15d, %edi
	imul	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -16(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	-16(%rbp), %r15d
	movl	%r15d, %edx
	movl	-12(%rbp), %r15d
	movl	%r15d, %edi
	add	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -12(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movl	-8(%rbp), %r15d
	movl	%r15d, %edi
	sub	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -8(%rbp)
.LpullInt67:
	pushq	%rdi
	pushq	%rdx
	movl	$-1, %edx
	movl	-8(%rbp), %r15d
	movl	%r15d, %edi
	cmpl	%edx, %edi
	setg	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$1, %al
	je	.LpullInt66
	pushq	%rdx
	call	clearInput
	popq	%rdx
	movl	-12(%rbp), %r15d
	movl	%r15d, %eax
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
	movl	-4(%rbp), %r15d
	movl	%r15d, %edi
	cmpl	%edx, %edi
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.LprintInt68
	pushq	%rdx
	pushq	%rdi
	movq	$.strprintInt69, %rax
	movq	%rax, %rdi
	movq	$0, %rsi
	call	print
	popq	%rdi
	popq	%rdx
	movl	$0, %eax
	leave
	ret
.LprintInt68:
	pushq	%rdi
	pushq	%rdx
	movl	$0, %edx
	movl	-4(%rbp), %r15d
	movl	%r15d, %edi
	cmpl	%edx, %edi
	setl	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.LprintInt70
	pushq	%rdx
	pushq	%rdi
	movq	$.strprintInt71, %rax
	movq	%rax, %rdi
	movq	$0, %rsi
	call	print
	popq	%rdi
	popq	%rdx
	pushq	%rdi
	pushq	%rdx
	movl	$-1, %edx
	movl	-4(%rbp), %r15d
	movl	%r15d, %edi
	imul	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -4(%rbp)
.LprintInt70:
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movl	-4(%rbp), %r15d
	movl	%r15d, %edi
	imul	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -8(%rbp)
	jmp	.LprintInt73
.LprintInt72:
	pushq	%rdi
	pushq	%rdx
	movl	-8(%rbp), %r15d
	movl	%r15d, %eax
	movl	-12(%rbp), %r15d
	movl	%r15d, %ecx
	cltd
	idiv	%ecx
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -8(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movl	-20(%rbp), %r15d
	movl	%r15d, %edi
	add	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -20(%rbp)
.LprintInt73:
	pushq	%rdi
	pushq	%rdx
	movl	$0, %edx
	movl	-8(%rbp), %r15d
	movl	%r15d, %edi
	cmpl	%edx, %edi
	setne	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$1, %al
	je	.LprintInt72
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movl	-20(%rbp), %r15d
	movl	%r15d, %edi
	sub	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -20(%rbp)
	jmp	.LprintInt75
.LprintInt74:
	pushq	%rdx
	pushq	%rdi
	movl	$10, %eax
	movl	%eax, %edi
	movl	-20(%rbp), %r15d
	pushq	%rsi
	movl	%r15d, %eax
	movl	%eax, %esi
	call	math.exp
	popq	%rsi
	popq	%rdi
	popq	%rdx
	movl	%eax, %ebx
	movl	%ebx, -16(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	-4(%rbp), %r15d
	movl	%r15d, %eax
	movl	-16(%rbp), %r15d
	movl	%r15d, %ecx
	cltd
	idiv	%ecx
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -8(%rbp)
	pushq	%rdx
	movl	-8(%rbp), %r15d
	pushq	%rdi
	movl	%r15d, %eax
	movl	%eax, %edi
	call	toChar
	popq	%rdi
	popq	%rdx
	movb	%al, %bl
	movb	%bl, -21(%rbp)
	pushq	%rdx
	movb	-21(%rbp), %r15b
	pushq	%rdi
	movb	%r15b, %al
	movb	%al, %dil
	call	printChar
	popq	%rdi
	popq	%rdx
	pushq	%rdi
	pushq	%rdx
	movl	-16(%rbp), %r15d
	movl	%r15d, %edx
	movl	-8(%rbp), %r15d
	movl	%r15d, %edi
	imul	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -8(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	-8(%rbp), %r15d
	movl	%r15d, %edx
	movl	-4(%rbp), %r15d
	movl	%r15d, %edi
	sub	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -4(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movl	-20(%rbp), %r15d
	movl	%r15d, %edi
	sub	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -20(%rbp)
.LprintInt75:
	pushq	%rdi
	pushq	%rdx
	movl	$-1, %edx
	movl	-20(%rbp), %r15d
	movl	%r15d, %edi
	cmpl	%edx, %edi
	setg	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$1, %al
	je	.LprintInt74
	movl	$0, %eax
	leave
	ret
io.printUnsignedInt:
printUnsignedInt:
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
	movl	-4(%rbp), %r15d
	movl	%r15d, %edi
	cmpl	%edx, %edi
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.LprintUnsignedInt76
	pushq	%rdx
	pushq	%rdi
	movq	$.strprintUnsignedInt77, %rax
	movq	%rax, %rdi
	movq	$0, %rsi
	call	print
	popq	%rdi
	popq	%rdx
	movl	$0, %eax
	leave
	ret
.LprintUnsignedInt76:
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movl	-4(%rbp), %r15d
	movl	%r15d, %edi
	imul	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -8(%rbp)
	jmp	.LprintUnsignedInt79
.LprintUnsignedInt78:
	pushq	%rdi
	pushq	%rdx
	movl	-8(%rbp), %r15d
	movl	%r15d, %eax
	movl	-12(%rbp), %r15d
	movl	%r15d, %ecx
	cltd
	idiv	%ecx
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -8(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movl	-20(%rbp), %r15d
	movl	%r15d, %edi
	add	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -20(%rbp)
.LprintUnsignedInt79:
	pushq	%rdi
	pushq	%rdx
	movl	$0, %edx
	movl	-8(%rbp), %r15d
	movl	%r15d, %edi
	cmpl	%edx, %edi
	setne	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$1, %al
	je	.LprintUnsignedInt78
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movl	-20(%rbp), %r15d
	movl	%r15d, %edi
	sub	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -20(%rbp)
	jmp	.LprintUnsignedInt81
.LprintUnsignedInt80:
	pushq	%rdx
	pushq	%rdi
	movl	$10, %eax
	movl	%eax, %edi
	movl	-20(%rbp), %r15d
	pushq	%rsi
	movl	%r15d, %eax
	movl	%eax, %esi
	call	math.exp
	popq	%rsi
	popq	%rdi
	popq	%rdx
	movl	%eax, %ebx
	movl	%ebx, -16(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	-4(%rbp), %r15d
	movl	%r15d, %eax
	movl	-16(%rbp), %r15d
	movl	%r15d, %ecx
	cltd
	idiv	%ecx
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -8(%rbp)
	pushq	%rdx
	movl	-8(%rbp), %r15d
	pushq	%rdi
	movl	%r15d, %eax
	movl	%eax, %edi
	call	toChar
	popq	%rdi
	popq	%rdx
	movb	%al, %bl
	movb	%bl, -21(%rbp)
	pushq	%rdx
	movb	-21(%rbp), %r15b
	pushq	%rdi
	movb	%r15b, %al
	movb	%al, %dil
	call	printChar
	popq	%rdi
	popq	%rdx
	pushq	%rdi
	pushq	%rdx
	movl	-16(%rbp), %r15d
	movl	%r15d, %edx
	movl	-8(%rbp), %r15d
	movl	%r15d, %edi
	imul	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -8(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	-8(%rbp), %r15d
	movl	%r15d, %edx
	movl	-4(%rbp), %r15d
	movl	%r15d, %edi
	sub	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -4(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movl	-20(%rbp), %r15d
	movl	%r15d, %edi
	sub	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -20(%rbp)
.LprintUnsignedInt81:
	pushq	%rdi
	pushq	%rdx
	movl	$-1, %edx
	movl	-20(%rbp), %r15d
	movl	%r15d, %edi
	cmpl	%edx, %edi
	setg	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$1, %al
	je	.LprintUnsignedInt80
	movl	$0, %eax
	leave
	ret
io.printHex:
printHex:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$32, %rsp
	movq	%rdi, -8(%rbp)
	movq	$0, %rbx
	movq	%rbx, -16(%rbp)
	movq	$0, %rbx
	movq	%rbx, -24(%rbp)
	movl	$0, %ebx
	movl	%ebx, -28(%rbp)
	pushq	%rdi
	pushq	%rdx
	movq	$0, %rdx
	movq	-8(%rbp), %r15
	movq	%r15, %rdi
	cmpq	%rdx, %rdi
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.LprintHex82
	pushq	%rdx
	pushq	%rdi
	movq	$.strprintHex83, %rax
	movq	%rax, %rdi
	movq	$0, %rsi
	call	print
	popq	%rdi
	popq	%rdx
	movl	$0, %eax
	leave
	ret
.LprintHex82:
	pushq	%rdi
	pushq	%rdx
	movq	$0, %rdx
	movq	-8(%rbp), %r15
	movq	%r15, %rdi
	cmpq	%rdx, %rdi
	setl	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.LprintHex84
	pushq	%rdx
	pushq	%rdi
	movq	$.strprintHex85, %rax
	movq	%rax, %rdi
	movq	$0, %rsi
	call	print
	popq	%rdi
	popq	%rdx
	pushq	%rdi
	pushq	%rdx
	movl	$-1, %edx
	movq	-8(%rbp), %r15
	movq	%r15, %rdi
	imul	%rdx, %rdi
	movq	%rdi, %rax
	popq	%rdx
	popq	%rdi
	movq	%rax, %rbx
	movq	%rbx, -8(%rbp)
.LprintHex84:
	movq	-8(%rbp), %r15
	movq	%r15, %rbx
	movq	%rbx, -16(%rbp)
	jmp	.LprintHex87
.LprintHex86:
	pushq	%rdi
	pushq	%rdx
	movq	-16(%rbp), %r15
	movq	%r15, %rax
	movq	$16, %rcx
	cltd
	idiv	%rcx
	popq	%rdx
	popq	%rdi
	movq	%rax, %rbx
	movq	%rbx, -16(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movl	-28(%rbp), %r15d
	movl	%r15d, %edi
	add	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -28(%rbp)
.LprintHex87:
	pushq	%rdi
	pushq	%rdx
	movq	$0, %rdx
	movq	-16(%rbp), %r15
	movq	%r15, %rdi
	cmpq	%rdx, %rdi
	setne	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$1, %al
	je	.LprintHex86
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movl	-28(%rbp), %r15d
	movl	%r15d, %edi
	sub	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -28(%rbp)
	jmp	.LprintHex89
.LprintHex88:
	pushq	%rdx
	pushq	%rdi
	movl	$16, %eax
	movl	%eax, %edi
	movl	-28(%rbp), %r15d
	pushq	%rsi
	movl	%r15d, %eax
	movl	%eax, %esi
	call	math.longExp
	popq	%rsi
	popq	%rdi
	popq	%rdx
	movq	%rax, %rbx
	movq	%rbx, -24(%rbp)
	pushq	%rdi
	pushq	%rdx
	movq	-8(%rbp), %r15
	movq	%r15, %rax
	movq	-24(%rbp), %r15
	movq	%r15, %rcx
	cltd
	idiv	%rcx
	popq	%rdx
	popq	%rdi
	movq	%rax, %rbx
	movq	%rbx, -16(%rbp)
	pushq	%rdx
	movq	-16(%rbp), %r15
	pushq	%rdi
	movq	%r15, %rax
	movq	%rax, %rdi
	call	longtoChar
	popq	%rdi
	popq	%rdx
	movb	%al, %bl
	movb	%bl, -29(%rbp)
	pushq	%rdx
	movb	-29(%rbp), %r15b
	pushq	%rdi
	movb	%r15b, %al
	movb	%al, %dil
	call	printChar
	popq	%rdi
	popq	%rdx
	pushq	%rdi
	pushq	%rdx
	movq	-24(%rbp), %r15
	movq	%r15, %rdx
	movq	-16(%rbp), %r15
	movq	%r15, %rdi
	imul	%rdx, %rdi
	movq	%rdi, %rax
	popq	%rdx
	popq	%rdi
	movq	%rax, %rbx
	movq	%rbx, -16(%rbp)
	pushq	%rdi
	pushq	%rdx
	movq	-16(%rbp), %r15
	movq	%r15, %rdx
	movq	-8(%rbp), %r15
	movq	%r15, %rdi
	sub	%rdx, %rdi
	movq	%rdi, %rax
	popq	%rdx
	popq	%rdi
	movq	%rax, %rbx
	movq	%rbx, -8(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movl	-28(%rbp), %r15d
	movl	%r15d, %edi
	sub	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -28(%rbp)
.LprintHex89:
	pushq	%rdi
	pushq	%rdx
	movl	$-1, %edx
	movl	-28(%rbp), %r15d
	movl	%r15d, %edi
	cmpl	%edx, %edi
	setg	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$1, %al
	je	.LprintHex88
	movl	$0, %eax
	leave
	ret
io.printLong:
printLong:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$48, %rsp
	movq	%rdi, -8(%rbp)
	movq	$0, %rbx
	movq	%rbx, -16(%rbp)
	movq	$10, %rbx
	movq	%rbx, -24(%rbp)
	movq	$0, %rbx
	movq	%rbx, -32(%rbp)
	movl	$0, %ebx
	movl	%ebx, -36(%rbp)
	pushq	%rdi
	pushq	%rdx
	movq	$0, %rdx
	movq	-8(%rbp), %r15
	movq	%r15, %rdi
	cmpq	%rdx, %rdi
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.LprintLong90
	pushq	%rdx
	pushq	%rdi
	movq	$.strprintLong91, %rax
	movq	%rax, %rdi
	movq	$0, %rsi
	call	print
	popq	%rdi
	popq	%rdx
	movl	$0, %eax
	leave
	ret
.LprintLong90:
	pushq	%rdi
	pushq	%rdx
	movq	$0, %rdx
	movq	-8(%rbp), %r15
	movq	%r15, %rdi
	cmpq	%rdx, %rdi
	setl	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.LprintLong92
	pushq	%rdx
	pushq	%rdi
	movq	$.strprintLong93, %rax
	movq	%rax, %rdi
	movq	$0, %rsi
	call	print
	popq	%rdi
	popq	%rdx
	pushq	%rdi
	pushq	%rdx
	movl	$-1, %edx
	movq	-8(%rbp), %r15
	movq	%r15, %rdi
	imul	%rdx, %rdi
	movq	%rdi, %rax
	popq	%rdx
	popq	%rdi
	movq	%rax, %rbx
	movq	%rbx, -8(%rbp)
.LprintLong92:
	movq	-8(%rbp), %r15
	movq	%r15, %rbx
	movq	%rbx, -16(%rbp)
	jmp	.LprintLong95
.LprintLong94:
	pushq	%rdi
	pushq	%rdx
	movq	-16(%rbp), %r15
	movq	%r15, %rax
	movq	-24(%rbp), %r15
	movq	%r15, %rcx
	cltd
	idiv	%rcx
	popq	%rdx
	popq	%rdi
	movq	%rax, %rbx
	movq	%rbx, -16(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movl	-36(%rbp), %r15d
	movl	%r15d, %edi
	add	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -36(%rbp)
.LprintLong95:
	pushq	%rdi
	pushq	%rdx
	movq	$0, %rdx
	movq	-16(%rbp), %r15
	movq	%r15, %rdi
	cmpq	%rdx, %rdi
	setne	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$1, %al
	je	.LprintLong94
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movl	-36(%rbp), %r15d
	movl	%r15d, %edi
	sub	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -36(%rbp)
	jmp	.LprintLong97
.LprintLong96:
	pushq	%rdx
	pushq	%rdi
	movl	$10, %eax
	movl	%eax, %edi
	movl	-36(%rbp), %r15d
	pushq	%rsi
	movl	%r15d, %eax
	movl	%eax, %esi
	call	math.longExp
	popq	%rsi
	popq	%rdi
	popq	%rdx
	movq	%rax, %rbx
	movq	%rbx, -32(%rbp)
	pushq	%rdi
	pushq	%rdx
	movq	-8(%rbp), %r15
	movq	%r15, %rax
	movq	-32(%rbp), %r15
	movq	%r15, %rcx
	cltd
	idiv	%rcx
	popq	%rdx
	popq	%rdi
	movq	%rax, %rbx
	movq	%rbx, -16(%rbp)
	pushq	%rdx
	movq	-16(%rbp), %r15
	pushq	%rdi
	movq	%r15, %rax
	movq	%rax, %rdi
	call	longtoChar
	popq	%rdi
	popq	%rdx
	movb	%al, %bl
	movb	%bl, -37(%rbp)
	pushq	%rdx
	movb	-37(%rbp), %r15b
	pushq	%rdi
	movb	%r15b, %al
	movb	%al, %dil
	call	printChar
	popq	%rdi
	popq	%rdx
	pushq	%rdi
	pushq	%rdx
	movq	-32(%rbp), %r15
	movq	%r15, %rdx
	movq	-16(%rbp), %r15
	movq	%r15, %rdi
	imul	%rdx, %rdi
	movq	%rdi, %rax
	popq	%rdx
	popq	%rdi
	movq	%rax, %rbx
	movq	%rbx, -16(%rbp)
	pushq	%rdi
	pushq	%rdx
	movq	-16(%rbp), %r15
	movq	%r15, %rdx
	movq	-8(%rbp), %r15
	movq	%r15, %rdi
	sub	%rdx, %rdi
	movq	%rdi, %rax
	popq	%rdx
	popq	%rdi
	movq	%rax, %rbx
	movq	%rbx, -8(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movl	-36(%rbp), %r15d
	movl	%r15d, %edi
	sub	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -36(%rbp)
.LprintLong97:
	pushq	%rdi
	pushq	%rdx
	movl	$-1, %edx
	movl	-36(%rbp), %r15d
	movl	%r15d, %edi
	cmpl	%edx, %edi
	setg	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$1, %al
	je	.LprintLong96
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
	movq	-8(%rbp), %r15
	movq	%r15, %rdi
	cmpq	%rdx, %rdi
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.LprintFloat98
	movl	$-1, %eax
	leave
	ret
.LprintFloat98:
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
	movl	-21(%rbp), %r15d
	movl	%r15d, %edi
	cmpl	%edx, %edi
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.LprintFloat99
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
.LprintFloat99:
	pushq	%rdi
	pushq	%rdx
	movl	$31, %edx
	movl	-21(%rbp), %r15d
	movl	%r15d, %edi
	movl	%edx, %ecx
	sarl	%cl, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -25(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	$23, %edx
	movl	-21(%rbp), %r15d
	movl	%r15d, %edi
	movl	%edx, %ecx
	sarl	%cl, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -29(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	$255, %edx
	movl	-29(%rbp), %r15d
	movl	%r15d, %edi
	andl	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -29(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	$127, %edx
	movl	-29(%rbp), %r15d
	movl	%r15d, %edi
	sub	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -29(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	$8388607, %edx
	movl	-21(%rbp), %r15d
	movl	%r15d, %edi
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
	movq	-41(%rbp), %r15
	movq	%r15, %rbx
	movq	%rbx, -49(%rbp)
	movl	$0, %ebx
	movl	%ebx, -53(%rbp)
	jmp	.LprintFloat101
.LprintFloat100:
	pushq	%rdx
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movl	-33(%rbp), %r15d
	movl	%r15d, %edi
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
	pushq	%r14
	movq	-49(%rbp), %r14
	movq	-61(%rbp), %r15
	movq	%r15, %rbx
	movq	%rbx, 12(%r14)
	popq	%r14
	pushq	%r14
	movq	-61(%rbp), %r14
	movq	-49(%rbp), %r15
	movq	%r15, %rbx
	movq	%rbx, 4(%r14)
	popq	%r14
	movq	-61(%rbp), %r15
	movq	%r15, %rbx
	movq	%rbx, -49(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movl	-33(%rbp), %r15d
	movl	%r15d, %edi
	movl	%edx, %ecx
	sarl	%cl, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -33(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movl	-53(%rbp), %r15d
	movl	%r15d, %edi
	add	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -53(%rbp)
.LprintFloat101:
	pushq	%rdi
	pushq	%rdx
	movl	$23, %edx
	movl	-53(%rbp), %r15d
	movl	%r15d, %edi
	cmpl	%edx, %edi
	setl	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$1, %al
	je	.LprintFloat100
	pushq	%rdx
	pushq	%rdi
	movl	$1, %eax
	movl	%eax, %edi
	call	newBit
	popq	%rdi
	popq	%rdx
	movq	%rax, %rbx
	movq	%rbx, -61(%rbp)
	pushq	%r14
	movq	-49(%rbp), %r14
	movq	-61(%rbp), %r15
	movq	%r15, %rbx
	movq	%rbx, 12(%r14)
	popq	%r14
	pushq	%r14
	movq	-61(%rbp), %r14
	movq	-49(%rbp), %r15
	movq	%r15, %rbx
	movq	%rbx, 4(%r14)
	popq	%r14
	movq	-61(%rbp), %r15
	movq	%r15, %rbx
	movq	%rbx, -49(%rbp)
	movq	-41(%rbp), %r15
	movq	%r15, %rbx
	movq	%rbx, -49(%rbp)
	jmp	.LprintFloat103
.LprintFloat102:
	pushq	%r14
	movq	-49(%rbp), %r14
	movq	12(%r14), %r15
	popq	%r14
	movq	%r15, %rbx
	movq	%rbx, -49(%rbp)
.LprintFloat103:
	pushq	%rdi
	pushq	%rdx
	movq	$0, %rdx
	pushq	%r14
	movq	-49(%rbp), %r14
	movq	12(%r14), %r15
	popq	%r14
	movq	%r15, %rdi
	cmpq	%rdx, %rdi
	setne	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$1, %al
	je	.LprintFloat102
	pushq	%rdi
	pushq	%rdx
	movl	$0, %edx
	movl	-25(%rbp), %r15d
	movl	%r15d, %edi
	cmpl	%edx, %edi
	setne	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.LprintFloat104
	movb	$45, %bl
	movb	%bl, -17(%rbp)
.LprintFloat104:
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movl	-12(%rbp), %r15d
	movl	%r15d, %edi
	cmpl	%edx, %edi
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.LprintFloat105
	pushq	%rdi
	pushq	%rdx
	movl	$0, %edx
	movl	-25(%rbp), %r15d
	movl	%r15d, %edi
	cmpl	%edx, %edi
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.LprintFloat106
	movb	$43, %bl
	movb	%bl, -17(%rbp)
.LprintFloat106:
.LprintFloat105:
	movl	-29(%rbp), %r15d
	movl	%r15d, %ebx
	movl	%ebx, -65(%rbp)
	movl	$0, %ebx
	movl	%ebx, -73(%rbp)
	jmp	.LprintFloat108
.LprintFloat107:
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	pushq	%r14
	movq	-49(%rbp), %r14
	movl	0(%r14), %r15d
	popq	%r14
	movl	%r15d, %edi
	cmpl	%edx, %edi
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.LprintFloat109
	pushq	%rdx
	pushq	%rdi
	movl	$2, %eax
	movl	%eax, %edi
	movl	-65(%rbp), %r15d
	pushq	%rsi
	movl	%r15d, %eax
	movl	%eax, %esi
	call	math.exp
	popq	%rsi
	popq	%rdi
	popq	%rdx
	movl	%eax, %ebx
	movl	%ebx, -77(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	-77(%rbp), %r15d
	movl	%r15d, %edx
	movl	-69(%rbp), %r15d
	movl	%r15d, %edi
	add	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -69(%rbp)
.LprintFloat109:
	pushq	%r14
	movq	-49(%rbp), %r14
	movq	4(%r14), %r15
	popq	%r14
	movq	%r15, %rbx
	movq	%rbx, -49(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movl	-65(%rbp), %r15d
	movl	%r15d, %edi
	sub	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -65(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movl	-73(%rbp), %r15d
	movl	%r15d, %edi
	add	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -73(%rbp)
.LprintFloat108:
	pushq	%rdi
	pushq	%rdx
	movl	$-1, %edx
	movl	-65(%rbp), %r15d
	movl	%r15d, %edi
	cmpl	%edx, %edi
	setg	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$1, %al
	je	.LprintFloat107
	pushq	%rdx
	pushq	%rdi
	movl	$10, %eax
	movl	%eax, %edi
	movl	-16(%rbp), %r15d
	pushq	%rsi
	movl	%r15d, %eax
	movl	%eax, %esi
	call	math.exp
	popq	%rsi
	popq	%rdi
	popq	%rdx
	movl	%eax, %ebx
	movl	%ebx, -65(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	$5, %edx
	movl	-65(%rbp), %r15d
	movl	%r15d, %edi
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
	jmp	.LprintFloat111
.LprintFloat110:
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	pushq	%r14
	movq	-49(%rbp), %r14
	movl	0(%r14), %r15d
	popq	%r14
	movl	%r15d, %edi
	cmpl	%edx, %edi
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.LprintFloat112
	pushq	%rdi
	pushq	%rdx
	movl	-65(%rbp), %r15d
	movl	%r15d, %edx
	movl	-81(%rbp), %r15d
	movl	%r15d, %edi
	add	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -81(%rbp)
.LprintFloat112:
	pushq	%r14
	movq	-49(%rbp), %r14
	movq	4(%r14), %r15
	popq	%r14
	movq	%r15, %rbx
	movq	%rbx, -49(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	-65(%rbp), %r15d
	movl	%r15d, %eax
	movl	-77(%rbp), %r15d
	movl	%r15d, %ecx
	cltd
	idiv	%ecx
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -65(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movl	-73(%rbp), %r15d
	movl	%r15d, %edi
	add	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -73(%rbp)
.LprintFloat111:
	pushq	%rdi
	pushq	%rdx
	movl	$23, %edx
	movl	-73(%rbp), %r15d
	movl	%r15d, %edi
	cmpl	%edx, %edi
	setl	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$1, %al
	je	.LprintFloat110
	pushq	%rdx
	movb	-17(%rbp), %r15b
	pushq	%rdi
	movb	%r15b, %al
	movb	%al, %dil
	call	printChar
	popq	%rdi
	popq	%rdx
	pushq	%rdx
	movl	-69(%rbp), %r15d
	pushq	%rdi
	movl	%r15d, %eax
	movl	%eax, %edi
	call	printInt
	popq	%rdi
	popq	%rdx
	pushq	%rdx
	pushq	%rdi
	movq	$.strprintFloat113, %rax
	movq	%rax, %rdi
	movq	$0, %rsi
	call	print
	popq	%rdi
	popq	%rdx
	pushq	%rdx
	movl	-81(%rbp), %r15d
	pushq	%rdi
	movl	%r15d, %eax
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
	movq	$.strclear114, %rax
	movq	%rax, %rdi
	movq	$0, %rsi
	call	print
	popq	%rdi
	popq	%rdx
	movl	$0, %eax
	leave
	ret


.data

.strclear114:
	.asciz	 "[1;1H[2J"
.strprintFloat113:
	.asciz	 "."
.strprintLong93:
	.asciz	 "-"
.strprintLong91:
	.asciz	 "0"
.strprintHex85:
	.asciz	 "-"
.strprintHex83:
	.asciz	 "0"
.strprintUnsignedInt77:
	.asciz	 "0"
.strprintInt71:
	.asciz	 "-"
.strprintInt69:
	.asciz	 "0"
.strprint16:
	.asciz	 "[0m"
.strprint14:
	.asciz	 "["


.bss

