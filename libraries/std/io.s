.global	printFloat
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
	movb	$32, -9(%rbp)
	movl	$10, %eax
	movl	%eax, %edi
	call	ascii
	movb	%al, -10(%rbp)
	movl	$0, %eax
	movl	%eax, %edi
	call	ascii
	movb	%al, -11(%rbp)
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
clearInput:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$96, %rsp
	movl	$10, %eax
	movl	%eax, %edi
	call	ascii
	movb	%al, -1(%rbp)
	movb	$32, -2(%rbp)
	jmp	.LclearInput4
.LclearInput3:
	lea	-2(%rbp), %rax
	movq	$1, %rdi
	movq	%rax, %rsi
	movq	$1, %rdx
	movq	$0, %rax
	syscall	
.LclearInput4:
	movb	-2(%rbp), %al
	movb	-1(%rbp), %cl
	cmpb	%cl, %al
	jne	.LclearInput3
	movl	$0, %eax
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
	movl	%eax, -12(%rbp)
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
toChar:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$16, %rsp
	movl	%edi, -4(%rbp)
	movb	$48, -5(%rbp)
	movl	-4(%rbp), %eax
	movl	$9, %ecx
	cmpl	%ecx, %eax
	jle	.LtoChar5
	movb	$65, %bl
	movb	%bl, -5(%rbp)
.LtoChar5:
	movl	-4(%rbp), %eax
	movl	$0, %ecx
	cmpl	%ecx, %eax
	jne	.LtoChar6
	movb	$48, %bl
	movb	%bl, -5(%rbp)
.LtoChar6:
	movl	-4(%rbp), %eax
	movl	$1, %ecx
	cmpl	%ecx, %eax
	jne	.LtoChar7
	movb	$49, %bl
	movb	%bl, -5(%rbp)
.LtoChar7:
	movl	-4(%rbp), %eax
	movl	$2, %ecx
	cmpl	%ecx, %eax
	jne	.LtoChar8
	movb	$50, %bl
	movb	%bl, -5(%rbp)
.LtoChar8:
	movl	-4(%rbp), %eax
	movl	$3, %ecx
	cmpl	%ecx, %eax
	jne	.LtoChar9
	movb	$51, %bl
	movb	%bl, -5(%rbp)
.LtoChar9:
	movl	-4(%rbp), %eax
	movl	$4, %ecx
	cmpl	%ecx, %eax
	jne	.LtoChar10
	movb	$52, %bl
	movb	%bl, -5(%rbp)
.LtoChar10:
	movl	-4(%rbp), %eax
	movl	$5, %ecx
	cmpl	%ecx, %eax
	jne	.LtoChar11
	movb	$53, %bl
	movb	%bl, -5(%rbp)
.LtoChar11:
	movl	-4(%rbp), %eax
	movl	$6, %ecx
	cmpl	%ecx, %eax
	jne	.LtoChar12
	movb	$54, %bl
	movb	%bl, -5(%rbp)
.LtoChar12:
	movl	-4(%rbp), %eax
	movl	$7, %ecx
	cmpl	%ecx, %eax
	jne	.LtoChar13
	movb	$55, %bl
	movb	%bl, -5(%rbp)
.LtoChar13:
	movl	-4(%rbp), %eax
	movl	$8, %ecx
	cmpl	%ecx, %eax
	jne	.LtoChar14
	movb	$56, %bl
	movb	%bl, -5(%rbp)
.LtoChar14:
	movl	-4(%rbp), %eax
	movl	$9, %ecx
	cmpl	%ecx, %eax
	jne	.LtoChar15
	movb	$57, %bl
	movb	%bl, -5(%rbp)
.LtoChar15:
	movb	-5(%rbp), %al
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
	jne	.LtoInt16
	movl	$0, %eax
	leave
	ret
.LtoInt16:
	movb	-1(%rbp), %al
	movb	$49, %cl
	cmpb	%cl, %al
	jne	.LtoInt17
	movl	$1, %eax
	leave
	ret
.LtoInt17:
	movb	-1(%rbp), %al
	movb	$50, %cl
	cmpb	%cl, %al
	jne	.LtoInt18
	movl	$2, %eax
	leave
	ret
.LtoInt18:
	movb	-1(%rbp), %al
	movb	$51, %cl
	cmpb	%cl, %al
	jne	.LtoInt19
	movl	$3, %eax
	leave
	ret
.LtoInt19:
	movb	-1(%rbp), %al
	movb	$52, %cl
	cmpb	%cl, %al
	jne	.LtoInt20
	movl	$4, %eax
	leave
	ret
.LtoInt20:
	movb	-1(%rbp), %al
	movb	$53, %cl
	cmpb	%cl, %al
	jne	.LtoInt21
	movl	$5, %eax
	leave
	ret
.LtoInt21:
	movb	-1(%rbp), %al
	movb	$54, %cl
	cmpb	%cl, %al
	jne	.LtoInt22
	movl	$6, %eax
	leave
	ret
.LtoInt22:
	movb	-1(%rbp), %al
	movb	$55, %cl
	cmpb	%cl, %al
	jne	.LtoInt23
	movl	$7, %eax
	leave
	ret
.LtoInt23:
	movb	-1(%rbp), %al
	movb	$56, %cl
	cmpb	%cl, %al
	jne	.LtoInt24
	movl	$8, %eax
	leave
	ret
.LtoInt24:
	movb	-1(%rbp), %al
	movb	$57, %cl
	cmpb	%cl, %al
	jne	.LtoInt25
	movl	$9, %eax
	leave
	ret
.LtoInt25:
	movl	$0, %eax
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
	movl	%eax, -8(%rbp)
	movl	$0, -12(%rbp)
	movl	$0, -16(%rbp)
	movl	$0, -20(%rbp)
	jmp	.LpullInt27
.LpullInt26:
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
.LpullInt27:
	movl	-8(%rbp), %eax
	movl	$-1, %ecx
	cmpl	%ecx, %eax
	jg	.LpullInt26
	call	clearInput
	movl	-12(%rbp), %eax
	leave
	ret
printInt:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$32, %rsp
	movl	%edi, -4(%rbp)
	movl	$0, -8(%rbp)
	movl	$10, -12(%rbp)
	movl	$0, -16(%rbp)
	movl	$0, -20(%rbp)
	movl	-4(%rbp), %eax
	movl	$0, %ecx
	cmpl	%ecx, %eax
	jne	.LprintInt28
	movq	$.strprintInt29, %rax
	movq	%rax, %rdi
	call	print
	movl	$0, %eax
	leave
	ret
.LprintInt28:
	movl	-4(%rbp), %eax
	movl	$0, %ecx
	cmpl	%ecx, %eax
	jge	.LprintInt30
	movq	$.strprintInt31, %rax
	movq	%rax, %rdi
	call	print
	mov	$-1, %edx
	mov	-4(%rbp), %eax
	imul	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -4(%rbp)
.LprintInt30:
	mov	$1, %edx
	mov	-4(%rbp), %eax
	imul	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -8(%rbp)
	jmp	.LprintInt33
.LprintInt32:
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
.LprintInt33:
	movl	-8(%rbp), %eax
	movl	$0, %ecx
	cmpl	%ecx, %eax
	jne	.LprintInt32
	mov	$1, %edx
	mov	-20(%rbp), %eax
	sub	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -20(%rbp)
	jmp	.LprintInt35
.LprintInt34:
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
.LprintInt35:
	movl	-20(%rbp), %eax
	movl	$-1, %ecx
	cmpl	%ecx, %eax
	jg	.LprintInt34
	movl	$0, %eax
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
	jne	.LprintFloat36
	movl	$-1, %eax
	leave
	ret
.LprintFloat36:
	movl	$6, -16(%rbp)
	movl	$0, %eax
	movl	%eax, %edi
	call	ascii
	movb	%al, -17(%rbp)
	movq	-8(%rbp), %rax
	movl	(%rax), %eax
	movl	%eax, -21(%rbp)
	movl	-21(%rbp), %eax
	movl	$0, %ecx
	cmpl	%ecx, %eax
	jne	.LprintFloat37
	movl	$0, %eax
	movl	%eax, %edi
	call	printInt
	movl	$0, %eax
	leave
	ret
.LprintFloat37:
	mov	$31, %edx
	mov	-21(%rbp), %eax
	movl	%edx, %ecx
	sarl	%cl, %eax
	movl	%eax, -25(%rbp)
	mov	$23, %edx
	mov	-21(%rbp), %eax
	movl	%edx, %ecx
	sarl	%cl, %eax
	movl	%eax, -29(%rbp)
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
	movl	%eax, -33(%rbp)
	movl	$0, %eax
	movl	%eax, %edi
	call	newBit
	movq	%rax, -41(%rbp)
	movq	-41(%rbp), %rbx
	movq	%rbx, -49(%rbp)
	movl	$0, -53(%rbp)
	jmp	.LprintFloat39
.LprintFloat38:
	mov	$1, %edx
	mov	-33(%rbp), %eax
	andl	%edx, %eax
	movl	%eax, %eax
	movl	%eax, %edi
	call	newBit
	movq	%rax, -61(%rbp)
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
.LprintFloat39:
	movl	-53(%rbp), %eax
	movl	$23, %ecx
	cmpl	%ecx, %eax
	jl	.LprintFloat38
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
	jmp	.LprintFloat41
.LprintFloat40:
	movq	-49(%rbp), %rdx
	movq	12(%rdx), %rbx
	movq	%rbx, -49(%rbp)
.LprintFloat41:
	movq	-49(%rbp), %rdx
	movq	12(%rdx), %rax
	movq	$0, %rcx
	cmpq	%rcx, %rax
	jne	.LprintFloat40
	movl	-25(%rbp), %eax
	movl	$0, %ecx
	cmpl	%ecx, %eax
	je	.LprintFloat42
	movb	$45, %bl
	movb	%bl, -17(%rbp)
.LprintFloat42:
	movl	-12(%rbp), %eax
	movl	$1, %ecx
	cmpl	%ecx, %eax
	jne	.LprintFloat43
	movl	-25(%rbp), %eax
	movl	$0, %ecx
	cmpl	%ecx, %eax
	jne	.LprintFloat44
	movb	$43, %bl
	movb	%bl, -17(%rbp)
.LprintFloat44:
.LprintFloat43:
	movl	-29(%rbp), %ebx
	movl	%ebx, -65(%rbp)
	movl	$0, -73(%rbp)
	jmp	.LprintFloat46
.LprintFloat45:
	movq	-49(%rbp), %rdx
	movl	0(%rdx), %eax
	movl	$1, %ecx
	cmpl	%ecx, %eax
	jne	.LprintFloat47
	movl	$2, %eax
	movl	%eax, %edi
	movl	-65(%rbp), %eax
	movl	%eax, %esi
	call	exp
	movl	%eax, -77(%rbp)
	mov	-77(%rbp), %edx
	mov	-69(%rbp), %eax
	add	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -69(%rbp)
.LprintFloat47:
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
.LprintFloat46:
	movl	-65(%rbp), %eax
	movl	$-1, %ecx
	cmpl	%ecx, %eax
	jg	.LprintFloat45
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
	movl	$2, -81(%rbp)
	movl	$0, -85(%rbp)
	jmp	.LprintFloat49
.LprintFloat48:
	movq	-49(%rbp), %rdx
	movl	0(%rdx), %eax
	movl	$1, %ecx
	cmpl	%ecx, %eax
	jne	.LprintFloat50
	mov	-65(%rbp), %edx
	mov	-85(%rbp), %eax
	add	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -85(%rbp)
.LprintFloat50:
	movq	-49(%rbp), %rdx
	movq	4(%rdx), %rbx
	movq	%rbx, -49(%rbp)
	mov	-65(%rbp), %eax
	cltd
	idivl	-81(%rbp)
	movl	%eax, %ebx
	movl	%ebx, -65(%rbp)
	mov	$1, %edx
	mov	-73(%rbp), %eax
	add	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -73(%rbp)
.LprintFloat49:
	movl	-73(%rbp), %eax
	movl	$23, %ecx
	cmpl	%ecx, %eax
	jl	.LprintFloat48
	lea	-17(%rbp), %rax
	movq	$1, %rdi
	movq	%rax, %rsi
	movq	$1, %rdx
	movq	$1, %rax
	syscall	
	movl	-69(%rbp), %eax
	movl	%eax, %edi
	call	printInt
	movq	$.strprintFloat51, %rax
	movq	%rax, %rdi
	call	print
	movl	-85(%rbp), %eax
	movl	%eax, %edi
	call	printInt
	movl	$0, %eax
	leave
	ret


.data

.strprintFloat51:
	.asciz	 "."
.strprintInt31:
	.asciz	 "-"
.strprintInt29:
	.asciz	 "0"


.bss

