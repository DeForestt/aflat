.global	pub_File_toList
.global	pub_File_toArray
.global	pub_File_read
.global	pub_File_write
.global	pub_File_close
.global	files.openFile
.global	files.createFile


.text

files.createFile:
createFile:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$32, %rsp
	movq	%rdi, -8(%rbp)
	pushq	%rdx
	pushq	%rdi
	movl	$12, %eax
	movl	%eax, %edi
	call	malloc
	popq	%rdi
	popq	%rdx
	movq	%rax, %rbx
	movq	%rbx, -16(%rbp)
	pushq	%rdx
	pushq	%rdi
	movq	-8(%rbp), %rax
	movq	%rax, %rdi
	pushq	%rsi
	movq	$511, %rax
	movq	%rax, %rsi
	call	sys_create
	popq	%rsi
	popq	%rdi
	popq	%rdx
	movq	-16(%rbp), %rdx
	movl	%eax, %ebx
	movl	%ebx, 8(%rdx)
	movq	-16(%rbp), %r14
	movl	8(%r14), %ebx
	movl	%ebx, -20(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movl	-20(%rbp), %edi
	cmpl	%edx, %edi
	setl	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.LcreateFile0
	movq	$0, %rax
	leave
	ret
.LcreateFile0:
	movq	-16(%rbp), %rax
	leave
	ret
files.openFile:
openFile:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$32, %rsp
	movq	%rdi, -8(%rbp)
	pushq	%rdx
	pushq	%rdi
	movl	$12, %eax
	movl	%eax, %edi
	call	malloc
	popq	%rdi
	popq	%rdx
	movq	%rax, %rbx
	movq	%rbx, -16(%rbp)
	movq	$2, %rbx
	movq	%rbx, -24(%rbp)
	pushq	%rdx
	pushq	%rdi
	movq	-8(%rbp), %rax
	movq	%rax, %rdi
	pushq	%rsi
	movq	-24(%rbp), %rax
	movq	%rax, %rsi
	pushq	%rdx
	movq	$777, %rax
	movq	%rax, %rdx
	call	sys_open
	popq	%rdx
	popq	%rsi
	popq	%rdi
	popq	%rdx
	movq	-16(%rbp), %rdx
	movl	%eax, %ebx
	movl	%ebx, 8(%rdx)
	movq	-16(%rbp), %r14
	movl	8(%r14), %ebx
	movl	%ebx, -28(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	$0, %edx
	movl	-28(%rbp), %edi
	cmpl	%edx, %edi
	setl	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.LopenFile1
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
	pushq	%rbx
	subq	$16, %rsp
	movq	%rdi, -8(%rbp)
	pushq	%rdx
	movq	-8(%rbp), %r14
	pushq	%rdi
	movl	8(%r14), %eax
	movl	%eax, %edi
	call	sys_close
	popq	%rdi
	popq	%rdx
	movl	%eax, %eax
	leave
	ret
pub_File_write:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$32, %rsp
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	pushq	%rdx
	pushq	%rdi
	movq	-16(%rbp), %rax
	movq	%rax, %rdi
	call	strings.len
	popq	%rdi
	popq	%rdx
	movl	%eax, %ebx
	movl	%ebx, -20(%rbp)
	pushq	%rdx
	movq	-8(%rbp), %r14
	pushq	%rdi
	movl	8(%r14), %eax
	movl	%eax, %edi
	pushq	%rsi
	movq	-16(%rbp), %rax
	movq	%rax, %rsi
	pushq	%rdx
	movl	-20(%rbp), %eax
	movl	%eax, %edx
	call	sys_write
	popq	%rdx
	popq	%rsi
	popq	%rdi
	popq	%rdx
	movl	%eax, %ebx
	movl	%ebx, -24(%rbp)
	movl	-24(%rbp), %eax
	leave
	ret
pub_File_read:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$32, %rsp
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	movb	%dl, -17(%rbp)
	movb	$0, %bl
	movb	%bl, -18(%rbp)
	movb	$0, %bl
	movb	%bl, -19(%rbp)
	jmp	.Lread3
.Lread2:
	pushq	%rdx
	movq	-8(%rbp), %r14
	pushq	%rdi
	movl	8(%r14), %eax
	movl	%eax, %edi
	lea	-18(%rbp), %rax
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
	movl	%eax, %ebx
	movl	%ebx, -23(%rbp)
	movb	-18(%rbp), %bl
	movb	%bl, -24(%rbp)
	movq	-16(%rbp), %rax
	movb	-24(%rbp), %bl
	movb	%bl, (%rax)
	pushq	%rdi
	pushq	%rdx
	movb	-17(%rbp), %dl
	movb	-24(%rbp), %dil
	cmpb	%dl, %dil
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.Lread4
	movq	-16(%rbp), %rax
	movb	-19(%rbp), %bl
	movb	%bl, (%rax)
.Lread4:
	pushq	%rdi
	pushq	%rdx
	movl	$0, %edx
	movl	-23(%rbp), %edi
	cmpl	%edx, %edi
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.Lread5
	movq	-16(%rbp), %rax
	movb	-19(%rbp), %bl
	movb	%bl, (%rax)
	movl	$1, %eax
	leave
	ret
.Lread5:
	pushq	%rdi
	pushq	%rdx
	movq	$1, %rdx
	movq	-16(%rbp), %rdi
	add	%rdx, %rdi
	movq	%rdi, %rax
	popq	%rdx
	popq	%rdi
	movq	%rax, %rbx
	movq	%rbx, -16(%rbp)
.Lread3:
	pushq	%rdi
	pushq	%rdx
	movb	-17(%rbp), %dl
	movb	-18(%rbp), %dil
	cmpb	%dl, %dil
	setne	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$1, %al
	je	.Lread2
	movq	-16(%rbp), %rax
	movb	-19(%rbp), %bl
	movb	%bl, (%rax)
	movl	$0, %eax
	leave
	ret
pub_File_toArray:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$48, %rsp
	movq	%rdi, -8(%rbp)
	movb	%sil, -9(%rbp)
	pushq	%rdx
	pushq	%rdi
	movl	$32, %eax
	movl	%eax, %edi
	call	malloc
	popq	%rdi
	popq	%rdx
	pushq	%rdi
	movq	%rax, %rdi
	pushq	%rdx
	pushq	%rsi
	movl	$8, %eax
	movl	%eax, %esi
	pushq	%rdx
	movl	$0, %eax
	movl	%eax, %edx
	call	pub_Array_init
	popq	%rdx
	popq	%rsi
	popq	%rdi
	popq	%rdx
	movq	%rax, %rbx
	movq	%rbx, -17(%rbp)
	pushq	%rdx
	pushq	%rdi
	movl	$256, %eax
	movl	%eax, %edi
	call	malloc
	popq	%rdi
	popq	%rdx
	movq	%rax, %rbx
	movq	%rbx, -25(%rbp)
	pushq	%rdx
	lea	-8(%rbp), %rax
	pushq	%rdi
	movq	(%rax), %rax
	movq	%rax, %rdi
	pushq	%rsi
	movq	-25(%rbp), %rax
	movq	%rax, %rsi
	pushq	%rdx
	movb	-9(%rbp), %al
	movb	%al, %dl
	call	pub_File_read
	popq	%rdx
	popq	%rsi
	popq	%rdi
	popq	%rdx
	movl	%eax, %ebx
	movl	%ebx, -29(%rbp)
	jmp	.LtoArray7
.LtoArray6:
	movq	-17(%rbp), %r14
	movl	28(%r14), %ebx
	movl	%ebx, -33(%rbp)
	pushq	%rdx
	lea	-17(%rbp), %rax
	pushq	%rdi
	movq	(%rax), %rax
	movq	%rax, %rdi
	pushq	%rsi
	movl	$1, %eax
	movl	%eax, %esi
	call	pub_Array_extend
	popq	%rsi
	popq	%rdi
	popq	%rdx
	pushq	%rdx
	lea	-17(%rbp), %rax
	pushq	%rdi
	movq	(%rax), %rax
	movq	%rax, %rdi
	pushq	%rsi
	movl	-33(%rbp), %eax
	movl	%eax, %esi
	call	pub_Array_at
	popq	%rsi
	popq	%rdi
	popq	%rdx
	movq	%rax, %rbx
	movq	%rbx, -41(%rbp)
	movq	-41(%rbp), %rax
	movq	-25(%rbp), %rbx
	movq	%rbx, (%rax)
	pushq	%rdx
	pushq	%rdi
	movl	$256, %eax
	movl	%eax, %edi
	call	malloc
	popq	%rdi
	popq	%rdx
	movq	%rax, %rbx
	movq	%rbx, -25(%rbp)
	pushq	%rdx
	lea	-8(%rbp), %rax
	pushq	%rdi
	movq	(%rax), %rax
	movq	%rax, %rdi
	pushq	%rsi
	movq	-25(%rbp), %rax
	movq	%rax, %rsi
	pushq	%rdx
	movb	-9(%rbp), %al
	movb	%al, %dl
	call	pub_File_read
	popq	%rdx
	popq	%rsi
	popq	%rdi
	popq	%rdx
	movl	%eax, %ebx
	movl	%ebx, -29(%rbp)
.LtoArray7:
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movl	-29(%rbp), %edi
	cmpl	%edx, %edi
	setne	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$1, %al
	je	.LtoArray6
	movq	-17(%rbp), %r14
	movl	28(%r14), %ebx
	movl	%ebx, -33(%rbp)
	pushq	%rdx
	lea	-17(%rbp), %rax
	pushq	%rdi
	movq	(%rax), %rax
	movq	%rax, %rdi
	pushq	%rsi
	movl	$1, %eax
	movl	%eax, %esi
	call	pub_Array_extend
	popq	%rsi
	popq	%rdi
	popq	%rdx
	pushq	%rdx
	lea	-17(%rbp), %rax
	pushq	%rdi
	movq	(%rax), %rax
	movq	%rax, %rdi
	pushq	%rsi
	movl	-33(%rbp), %eax
	movl	%eax, %esi
	call	pub_Array_at
	popq	%rsi
	popq	%rdi
	popq	%rdx
	movq	%rax, %rbx
	movq	%rbx, -41(%rbp)
	movq	-41(%rbp), %rax
	movq	-25(%rbp), %rbx
	movq	%rbx, (%rax)
	pushq	%rdx
	pushq	%rdi
	movl	$256, %eax
	movl	%eax, %edi
	call	malloc
	popq	%rdi
	popq	%rdx
	movq	%rax, %rbx
	movq	%rbx, -25(%rbp)
	movq	-17(%rbp), %rax
	leave
	ret
pub_File_toList:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$32, %rsp
	movq	%rdi, -8(%rbp)
	movb	%sil, -9(%rbp)
	pushq	%rdx
	pushq	%rdi
	movl	$24, %eax
	movl	%eax, %edi
	call	malloc
	popq	%rdi
	popq	%rdx
	pushq	%rdi
	movq	%rax, %rdi
	pushq	%rdx
	call	pub_LinkedList_init
	popq	%rdi
	popq	%rdx
	movq	%rax, %rbx
	movq	%rbx, -17(%rbp)
	pushq	%rdx
	pushq	%rdi
	movl	$256, %eax
	movl	%eax, %edi
	call	malloc
	popq	%rdi
	popq	%rdx
	movq	%rax, %rbx
	movq	%rbx, -25(%rbp)
	pushq	%rdx
	lea	-8(%rbp), %rax
	pushq	%rdi
	movq	(%rax), %rax
	movq	%rax, %rdi
	pushq	%rsi
	movq	-25(%rbp), %rax
	movq	%rax, %rsi
	pushq	%rdx
	movb	-9(%rbp), %al
	movb	%al, %dl
	call	pub_File_read
	popq	%rdx
	popq	%rsi
	popq	%rdi
	popq	%rdx
	movl	%eax, %ebx
	movl	%ebx, -29(%rbp)
	jmp	.LtoList9
.LtoList8:
	pushq	%rdx
	lea	-17(%rbp), %rax
	pushq	%rdi
	movq	(%rax), %rax
	movq	%rax, %rdi
	pushq	%rsi
	movq	-25(%rbp), %rax
	movq	%rax, %rsi
	call	pub_LinkedList_append
	popq	%rsi
	popq	%rdi
	popq	%rdx
	pushq	%rdx
	pushq	%rdi
	movl	$256, %eax
	movl	%eax, %edi
	call	malloc
	popq	%rdi
	popq	%rdx
	movq	%rax, %rbx
	movq	%rbx, -25(%rbp)
	pushq	%rdx
	lea	-8(%rbp), %rax
	pushq	%rdi
	movq	(%rax), %rax
	movq	%rax, %rdi
	pushq	%rsi
	movq	-25(%rbp), %rax
	movq	%rax, %rsi
	pushq	%rdx
	movb	-9(%rbp), %al
	movb	%al, %dl
	call	pub_File_read
	popq	%rdx
	popq	%rsi
	popq	%rdi
	popq	%rdx
	movl	%eax, %ebx
	movl	%ebx, -29(%rbp)
.LtoList9:
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movl	-29(%rbp), %edi
	cmpl	%edx, %edi
	setne	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$1, %al
	je	.LtoList8
	pushq	%rdx
	lea	-17(%rbp), %rax
	pushq	%rdi
	movq	(%rax), %rax
	movq	%rax, %rdi
	pushq	%rsi
	movq	-25(%rbp), %rax
	movq	%rax, %rsi
	call	pub_LinkedList_append
	popq	%rsi
	popq	%rdi
	popq	%rdx
	movq	-17(%rbp), %rax
	leave
	ret


.data



.bss

