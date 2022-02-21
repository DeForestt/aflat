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
pub_File_close:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$16, %rsp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rdx
	movl	8(%rdx), %eax
	movl	%eax, %edi
	call	sys_close
	movl	%eax, %eax
	leave
	ret
pub_File_write:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$32, %rsp
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	movq	-16(%rbp), %rax
	movq	%rax, %rdi
	call	len
	movl	%eax, -20(%rbp)
	movq	-8(%rbp), %rdx
	movl	8(%rdx), %eax
	movl	%eax, %edi
	movq	-16(%rbp), %rax
	movq	%rax, %rsi
	movl	-20(%rbp), %eax
	movl	%eax, %edx
	call	sys_write
	movl	%eax, -24(%rbp)
	movl	-24(%rbp), %eax
	leave
	ret
pub_File_read:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$32, %rsp
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	movb	%dl, -17(%rbp)
	movb	%cl, -18(%rbp)
	movb	$0, %bl
	movb	%bl, -19(%rbp)
	movb	$0, -20(%rbp)
	jmp	.Lread7
.Lread6:
	movq	-8(%rbp), %rdx
	movl	8(%rdx), %eax
	movl	%eax, %edi
	lea	-19(%rbp), %rax
	movq	%rax, %rax
	movq	%rax, %rsi
	movl	$1, %eax
	movl	%eax, %edx
	call	sys_read
	movb	-19(%rbp), %bl
	movb	%bl, -21(%rbp)
	movb	-21(%rbp), %al
	movb	-18(%rbp), %cl
	cmpb	%cl, %al
	jne	.Lread8
	movl	$1, %eax
	leave
	ret
.Lread8:
	movq	-16(%rbp), %rax
	movb	-21(%rbp), %bl
	movb	%bl, (%rax)
	movb	-21(%rbp), %al
	movb	-17(%rbp), %cl
	cmpb	%cl, %al
	jne	.Lread11
	movq	-16(%rbp), %rax
	movb	-20(%rbp), %bl
	movb	%bl, (%rax)
.Lread11:
	mov	$1, %rdx
	mov	-16(%rbp), %rax
	add	%rdx, %rax
	movl	%eax, %ebx
	movl	%ebx, -16(%rbp)
.Lread7:
	movb	-19(%rbp), %al
	movb	-17(%rbp), %cl
	cmpb	%cl, %al
	jne	.Lread6
	movq	-16(%rbp), %rax
	movb	-20(%rbp), %bl
	movb	%bl, (%rax)
	movl	$0, %eax
	leave
	ret
pub_File_toArray:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$32, %rsp
	movq	%rdi, -8(%rbp)
	movb	%sil, -9(%rbp)
	movb	%dl, -10(%rbp)
	movl	$8, %eax
	movl	%eax, %edi
	movl	$0, %eax
	movl	%eax, %esi
	call	newArray
	movq	%rax, -18(%rbp)
	movl	$256, %eax
	movl	%eax, %edi
	call	malloc
	movq	%rax, -26(%rbp)
	lea	-8(%rbp), %rax
	movq	(%rax), %rax
	movq	%rax, %rdi
	movq	-26(%rbp), %rax
	movq	%rax, %rsi
	movb	-9(%rbp), %al
	movb	%al, %dl
	movb	-10(%rbp), %al
	movb	%al, %cl
	call	pub_File_read
	movl	%eax, -30(%rbp)
	jmp	.LtoArray15
.LtoArray14:
	movq	-18(%rbp), %rdx
	movl	12(%rdx), %ebx
	movl	%ebx, -34(%rbp)
	lea	-18(%rbp), %rax
	movq	(%rax), %rax
	movq	%rax, %rdi
	movl	$1, %eax
	movl	%eax, %esi
	call	pub_Array_extend
	lea	-18(%rbp), %rax
	movq	(%rax), %rax
	movq	%rax, %rdi
	movl	-34(%rbp), %eax
	movl	%eax, %esi
	call	pub_Array_at
	movq	%rax, -42(%rbp)
	movl	-30(%rbp), %eax
	movl	$1, %ecx
	cmpl	%ecx, %eax
	je	.LtoArray16
	movq	-42(%rbp), %rax
	movq	-26(%rbp), %rbx
	movq	%rbx, (%rax)
	movl	$256, %eax
	movl	%eax, %edi
	call	malloc
	movq	%rax, %rbx
	movq	%rbx, -26(%rbp)
.LtoArray16:
	lea	-8(%rbp), %rax
	movq	(%rax), %rax
	movq	%rax, %rdi
	movq	-26(%rbp), %rax
	movq	%rax, %rsi
	movb	-9(%rbp), %al
	movb	%al, %dl
	movb	-10(%rbp), %al
	movb	%al, %cl
	call	pub_File_read
	movl	%eax, %ebx
	movl	%ebx, -30(%rbp)
.LtoArray15:
	movl	-30(%rbp), %eax
	movl	$1, %ecx
	cmpl	%ecx, %eax
	jne	.LtoArray14
	movq	-18(%rbp), %rax
	leave
	ret
pub_File_toList:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$32, %rsp
	movq	%rdi, -8(%rbp)
	movb	%sil, -9(%rbp)
	movb	%dl, -10(%rbp)
	movl	$8, %eax
	movl	%eax, %edi
	call	newList
	movq	%rax, -18(%rbp)
	movl	$256, %eax
	movl	%eax, %edi
	call	malloc
	movq	%rax, -26(%rbp)
	lea	-8(%rbp), %rax
	movq	(%rax), %rax
	movq	%rax, %rdi
	movq	-26(%rbp), %rax
	movq	%rax, %rsi
	movb	-9(%rbp), %al
	movb	%al, %dl
	movb	-10(%rbp), %al
	movb	%al, %cl
	call	pub_File_read
	movl	%eax, -30(%rbp)
	jmp	.LtoList20
.LtoList19:
	lea	-18(%rbp), %rax
	movq	(%rax), %rax
	movq	%rax, %rdi
	movq	-26(%rbp), %rax
	movq	%rax, %rsi
	call	pub_LinkedList_append
	movl	$256, %eax
	movl	%eax, %edi
	call	malloc
	movq	%rax, %rbx
	movq	%rbx, -26(%rbp)
	lea	-8(%rbp), %rax
	movq	(%rax), %rax
	movq	%rax, %rdi
	movq	-26(%rbp), %rax
	movq	%rax, %rsi
	movb	-9(%rbp), %al
	movb	%al, %dl
	movb	-10(%rbp), %al
	movb	%al, %cl
	call	pub_File_read
	movl	%eax, %ebx
	movl	%ebx, -30(%rbp)
.LtoList20:
	movl	-30(%rbp), %eax
	movl	$1, %ecx
	cmpl	%ecx, %eax
	jne	.LtoList19
	movq	-18(%rbp), %rax
	leave
	ret


.data



.bss

