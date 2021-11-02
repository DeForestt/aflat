.global	pub_File_read
.global	pub_File_write
.global	pub_File_close
.global	openFile
.global	createFile


.text

createFile:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$32, %rsp
	movq	%rdi, -8(%rbp)
	movl	$12, %eax
	movl	%eax, %edi
	call	malloc
	movq	%rax, -16(%rbp)
	movq	-8(%rbp), %rax
	movq	%rax, %rdi
	movq	$511, %rax
	movq	%rax, %rsi
	call	sys_create
	movq	-16(%rbp), %rdx
	movl	%eax, %ebx
	movl	%ebx, 8(%rdx)
	movq	-16(%rbp), %rdx
	movl	8(%rdx), %ebx
	movl	%ebx, -20(%rbp)
	movl	-20(%rbp), %eax
	movl	$1, %ecx
	cmpl	%ecx, %eax
	jge	.LcreateFile0
	movq	$0, %rax
	leave
	ret
.LcreateFile0:
	movq	-16(%rbp), %rax
	leave
	ret
openFile:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$32, %rsp
	movq	%rdi, -8(%rbp)
	movl	$12, %eax
	movl	%eax, %edi
	call	malloc
	movq	%rax, -16(%rbp)
	movq	$2, -24(%rbp)
	movq	-8(%rbp), %rax
	movq	%rax, %rdi
	movq	-24(%rbp), %rax
	movq	%rax, %rsi
	movq	$777, %rax
	movq	%rax, %rdx
	call	sys_open
	movq	-16(%rbp), %rdx
	movl	%eax, %ebx
	movl	%ebx, 8(%rdx)
	movq	-16(%rbp), %rdx
	movl	8(%rdx), %ebx
	movl	%ebx, -28(%rbp)
	movl	-28(%rbp), %eax
	movl	$0, %ecx
	cmpl	%ecx, %eax
	jge	.LopenFile1
	movq	$0, %rax
	leave
	ret
.LopenFile1:
	movq	-16(%rbp), %rdx
	movq	-8(%rbp), %rbx
	movq	%rbx, 0(%rdx)
	movq	-16(%rbp), %rax
	leave
	ret
pub_File_close:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$32, %rsp
	movq	%rdi, -20(%rbp)
	movq	-20(%rbp), %rdx
	movl	8(%rdx), %eax
	movl	%eax, %edi
	call	sys_close
	movl	%eax, %eax
	leave
	ret
pub_File_write:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$48, %rsp
	movq	%rdi, -20(%rbp)
	movq	%rsi, -28(%rbp)
	movq	-28(%rbp), %rax
	movq	%rax, %rdi
	call	len
	movl	%eax, -32(%rbp)
	movq	-20(%rbp), %rdx
	movl	8(%rdx), %eax
	movl	%eax, %edi
	movq	-28(%rbp), %rax
	movq	%rax, %rsi
	movl	-32(%rbp), %eax
	movl	%eax, %edx
	call	sys_write
	movl	%eax, -36(%rbp)
	movl	-36(%rbp), %eax
	leave
	ret
pub_File_read:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$32, %rsp
	movq	%rdi, -20(%rbp)
	movq	%rsi, -28(%rbp)
	movb	%dl, -29(%rbp)
	movb	%cl, -30(%rbp)
	movb	$32, -31(%rbp)
	movl	$0, %eax
	movl	%eax, %edi
	call	ascii
	movb	%al, -32(%rbp)
	jmp	.Lread3
.Lread2:
	movq	-20(%rbp), %rdx
	movl	8(%rdx), %eax
	movl	%eax, %edi
	lea	-31(%rbp), %rax
	movq	%rax, %rax
	movq	%rax, %rsi
	movl	$1, %eax
	movl	%eax, %edx
	call	sys_read
	movb	-31(%rbp), %al
	movb	-30(%rbp), %cl
	cmpb	%cl, %al
	jne	.Lread4
	movl	$1, %eax
	leave
	ret
.Lread4:
	movq	-28(%rbp), %rax
	movb	-31(%rbp), %bl
	movb	%bl, (%rax)
	movb	-31(%rbp), %al
	movb	-29(%rbp), %cl
	cmpb	%cl, %al
	jne	.Lread5
	movq	-28(%rbp), %rax
	movb	-32(%rbp), %bl
	movb	%bl, (%rax)
.Lread5:
	mov	$1, %rdx
	mov	-28(%rbp), %rax
	add	%rdx, %rax
	movl	%eax, %ebx
	movl	%ebx, -28(%rbp)
.Lread3:
	movb	-31(%rbp), %al
	movb	-29(%rbp), %cl
	cmpb	%cl, %al
	jne	.Lread2
	movq	-28(%rbp), %rax
	movb	-32(%rbp), %bl
	movb	%bl, (%rax)
	movl	$0, %eax
	leave
	ret


.data



.bss

