.global	pub_File_toList
.global	pub_File_toArray
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
	pushq	%rdi
	movl	$12, %eax
	movl	%eax, %edi
	call	malloc
	popq	%rdi
	movq	%rax, %rbx
	movq	%rbx, -16(%rbp)
	pushq	%rdi
	movq	-8(%rbp), %rax
	movq	%rax, %rdi
	pushq	%rsi
	movq	$511, %rax
	movq	%rax, %rsi
	call	sys_create
	popq	%rsi
	popq	%rdi
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
	leave
	ret
openFile:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$32, %rsp
	movq	%rdi, -8(%rbp)
	pushq	%rdi
	movl	$12, %eax
	movl	%eax, %edi
	call	malloc
	popq	%rdi
	movq	%rax, %rbx
	movq	%rbx, -16(%rbp)
	movq	$2, %rbx
	movq	%rbx, -24(%rbp)
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
	movq	-16(%rbp), %rdx
	movl	%eax, %ebx
	movl	%ebx, 8(%rdx)
	movq	-16(%rbp), %rdx
	movl	8(%rdx), %ebx
	movl	%ebx, -28(%rbp)
	movl	-28(%rbp), %eax
	movl	$0, %ecx
	cmpl	%ecx, %eax
	jge	.LopenFile3
	movq	$0, %rax
	leave
	ret
.LopenFile3:
	movq	-16(%rbp), %rdx
	movq	-8(%rbp), %rbx
	movq	%rbx, 0(%rdx)
	movq	-16(%rbp), %rax
	leave
	ret
	leave
	ret
pub_File_close:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$16, %rsp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rdx
	pushq	%rdi
	movl	8(%rdx), %eax
	movl	%eax, %edi
	call	sys_close
	popq	%rdi
	movl	%eax, %eax
	leave
	ret
	leave
	ret
pub_File_write:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$32, %rsp
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	pushq	%rdi
	movq	-16(%rbp), %rax
	movq	%rax, %rdi
	call	len
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -20(%rbp)
	movq	-8(%rbp), %rdx
	pushq	%rdi
	movl	8(%rdx), %eax
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
	movl	%eax, %ebx
	movl	%ebx, -24(%rbp)
	movl	-24(%rbp), %eax
	leave
	ret
	leave
	ret
pub_File_read:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$32, %rsp
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	movb	%dl, -17(%rbp)
	movb	$0, %bl
	movb	%bl, -18(%rbp)
	movb	$0, %bl
	movb	%bl, -19(%rbp)
	jmp	.Lread7
.Lread6:
	movq	-8(%rbp), %rdx
	pushq	%rdi
	movl	8(%rdx), %eax
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
	movl	%eax, %ebx
	movl	%ebx, -23(%rbp)
	movb	-18(%rbp), %bl
	movb	%bl, -24(%rbp)
	movq	-16(%rbp), %rax
	movb	-24(%rbp), %bl
	movb	%bl, (%rax)
	movb	-24(%rbp), %al
	movb	-17(%rbp), %cl
	cmpb	%cl, %al
	jne	.Lread8
	movq	-16(%rbp), %rax
	movb	-19(%rbp), %bl
	movb	%bl, (%rax)
.Lread8:
	movl	-23(%rbp), %eax
	movl	$0, %ecx
	cmpl	%ecx, %eax
	jne	.Lread11
	movq	-16(%rbp), %rax
	movb	-19(%rbp), %bl
	movb	%bl, (%rax)
	movl	$1, %eax
	leave
	ret
.Lread11:
	mov	$1, %rdx
	mov	-16(%rbp), %rax
	add	%rdx, %rax
	movl	%eax, %ebx
	movl	%ebx, -16(%rbp)
.Lread7:
	movb	-18(%rbp), %al
	movb	-17(%rbp), %cl
	cmpb	%cl, %al
	jne	.Lread6
	movq	-16(%rbp), %rax
	movb	-19(%rbp), %bl
	movb	%bl, (%rax)
	movl	$0, %eax
	leave
	ret
	leave
	ret
pub_File_toArray:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$48, %rsp
	movq	%rdi, -8(%rbp)
	movb	%sil, -9(%rbp)
	pushq	%rdi
	movl	$32, %eax
	movl	%eax, %edi
	call	malloc
	popq	%rdi
	pushq	%rdi
	movq	%rax, %rdi
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
	movq	%rax, %rbx
	movq	%rbx, -17(%rbp)
	pushq	%rdi
	movl	$256, %eax
	movl	%eax, %edi
	call	malloc
	popq	%rdi
	movq	%rax, %rbx
	movq	%rbx, -25(%rbp)
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
	movl	%eax, %ebx
	movl	%ebx, -29(%rbp)
	jmp	.LtoArray15
.LtoArray14:
	movq	-17(%rbp), %rdx
	movl	28(%rdx), %ebx
	movl	%ebx, -33(%rbp)
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
	movq	%rax, %rbx
	movq	%rbx, -41(%rbp)
	movq	-41(%rbp), %rax
	movq	-25(%rbp), %rbx
	movq	%rbx, (%rax)
	pushq	%rdi
	movl	$256, %eax
	movl	%eax, %edi
	call	malloc
	popq	%rdi
	movq	%rax, %rbx
	movq	%rbx, -25(%rbp)
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
	movl	%eax, %ebx
	movl	%ebx, -29(%rbp)
.LtoArray15:
	movl	-29(%rbp), %eax
	movl	$1, %ecx
	cmpl	%ecx, %eax
	jne	.LtoArray14
	movq	-17(%rbp), %rdx
	movl	28(%rdx), %ebx
	movl	%ebx, -33(%rbp)
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
	movq	%rax, %rbx
	movq	%rbx, -41(%rbp)
	movq	-41(%rbp), %rax
	movq	-25(%rbp), %rbx
	movq	%rbx, (%rax)
	pushq	%rdi
	movl	$256, %eax
	movl	%eax, %edi
	call	malloc
	popq	%rdi
	movq	%rax, %rbx
	movq	%rbx, -25(%rbp)
	movq	-17(%rbp), %rax
	leave
	ret
	leave
	ret
pub_File_toList:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$32, %rsp
	movq	%rdi, -8(%rbp)
	movb	%sil, -9(%rbp)
	pushq	%rdi
	movl	$24, %eax
	movl	%eax, %edi
	call	malloc
	popq	%rdi
	pushq	%rdi
	movq	%rax, %rdi
	call	pub_LinkedList_init
	popq	%rdi
	movq	%rax, %rbx
	movq	%rbx, -17(%rbp)
	pushq	%rdi
	movl	$256, %eax
	movl	%eax, %edi
	call	malloc
	popq	%rdi
	movq	%rax, %rbx
	movq	%rbx, -25(%rbp)
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
	movl	%eax, %ebx
	movl	%ebx, -29(%rbp)
	jmp	.LtoList17
.LtoList16:
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
	pushq	%rdi
	movl	$256, %eax
	movl	%eax, %edi
	call	malloc
	popq	%rdi
	movq	%rax, %rbx
	movq	%rbx, -25(%rbp)
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
	movl	%eax, %ebx
	movl	%ebx, -29(%rbp)
.LtoList17:
	movl	-29(%rbp), %eax
	movl	$1, %ecx
	cmpl	%ecx, %eax
	jne	.LtoList16
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
	movq	-17(%rbp), %rax
	leave
	ret
	leave
	ret


.data



.bss

