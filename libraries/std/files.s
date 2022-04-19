.global	pub_File_toList
.global	pub_File_toArray
.global	pub_File_readToEnd
.global	pub_File_read
.global	pub_File_write
.global	pub_File_close
.global	files.openFile
.global	files.createFile


.text

pub_ICollection_init:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$16, %rsp
	movq	%rdi, -8(%rbp)
	pushq	%r14
	movq	-8(%rbp), %r14
	movq	$lambda_1650397015394_0, %rbx
	movq	%rbx, 0(%r14)
	popq	%r14
	pushq	%r14
	movq	-8(%rbp), %r14
	movq	$lambda_1650397015394_2, %rbx
	movq	%rbx, 8(%r14)
	popq	%r14
	movq	-8(%rbp), %r15
	movq	%r15, %rax
	leave
	ret
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
	pushq	%r14
	movq	-16(%rbp), %r14
	pushq	%rdx
	movq	-8(%rbp), %r15
	pushq	%rdi
	movq	%r15, %rax
	movq	%rax, %rdi
	pushq	%rsi
	movq	$511, %rax
	movq	%rax, %rsi
	call	sys_create
	popq	%rsi
	popq	%rdi
	popq	%rdx
	movl	%eax, %ebx
	movl	%ebx, 8(%r14)
	popq	%r14
	pushq	%r14
	movq	-16(%rbp), %r14
	movl	8(%r14), %r15d
	popq	%r14
	movl	%r15d, %ebx
	movl	%ebx, -20(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movl	-20(%rbp), %r15d
	movl	%r15d, %edi
	cmpl	%edx, %edi
	setl	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.LcreateFile4
	movq	$0, %rax
	leave
	ret
.LcreateFile4:
	movq	-16(%rbp), %r15
	movq	%r15, %rax
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
	pushq	%r14
	movq	-16(%rbp), %r14
	pushq	%rdx
	movq	-8(%rbp), %r15
	pushq	%rdi
	movq	%r15, %rax
	movq	%rax, %rdi
	movq	-24(%rbp), %r15
	pushq	%rsi
	movq	%r15, %rax
	movq	%rax, %rsi
	pushq	%rdx
	movq	$777, %rax
	movq	%rax, %rdx
	call	sys_open
	popq	%rdx
	popq	%rsi
	popq	%rdi
	popq	%rdx
	movl	%eax, %ebx
	movl	%ebx, 8(%r14)
	popq	%r14
	pushq	%r14
	movq	-16(%rbp), %r14
	movl	8(%r14), %r15d
	popq	%r14
	movl	%r15d, %ebx
	movl	%ebx, -28(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	$0, %edx
	movl	-28(%rbp), %r15d
	movl	%r15d, %edi
	cmpl	%edx, %edi
	setl	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.LopenFile5
	movq	$0, %rax
	leave
	ret
.LopenFile5:
	pushq	%r14
	movq	-16(%rbp), %r14
	movq	-8(%rbp), %r15
	movq	%r15, %rbx
	movq	%rbx, 0(%r14)
	popq	%r14
	movq	-16(%rbp), %r15
	movq	%r15, %rax
	leave
	ret
pub_File_close:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$16, %rsp
	movq	%rdi, -8(%rbp)
	pushq	%rdx
	pushq	%r14
	movq	-8(%rbp), %r14
	movl	8(%r14), %r15d
	popq	%r14
	pushq	%rdi
	movl	%r15d, %eax
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
	movq	-16(%rbp), %r15
	pushq	%rdi
	movq	%r15, %rax
	movq	%rax, %rdi
	call	strings.len
	popq	%rdi
	popq	%rdx
	movl	%eax, %ebx
	movl	%ebx, -20(%rbp)
	pushq	%rdx
	pushq	%r14
	movq	-8(%rbp), %r14
	movl	8(%r14), %r15d
	popq	%r14
	pushq	%rdi
	movl	%r15d, %eax
	movl	%eax, %edi
	movq	-16(%rbp), %r15
	pushq	%rsi
	movq	%r15, %rax
	movq	%rax, %rsi
	movl	-20(%rbp), %r15d
	pushq	%rdx
	movl	%r15d, %eax
	movl	%eax, %edx
	call	sys_write
	popq	%rdx
	popq	%rsi
	popq	%rdi
	popq	%rdx
	movl	%eax, %ebx
	movl	%ebx, -24(%rbp)
	movl	-24(%rbp), %r15d
	movl	%r15d, %eax
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
	jmp	.Lread7
.Lread6:
	pushq	%rdx
	pushq	%r14
	movq	-8(%rbp), %r14
	movl	8(%r14), %r15d
	popq	%r14
	pushq	%rdi
	movl	%r15d, %eax
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
	movb	-18(%rbp), %r15b
	movb	%r15b, %bl
	movb	%bl, -24(%rbp)
	movb	-24(%rbp), %r15b
	movq	-16(%rbp), %rax
	movb	%r15b, %bl
	movb	%bl, (%rax)
	pushq	%rdi
	pushq	%rdx
	movb	-17(%rbp), %r15b
	movb	%r15b, %dl
	movb	-24(%rbp), %r15b
	movb	%r15b, %dil
	cmpb	%dl, %dil
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.Lread8
	movb	-19(%rbp), %r15b
	movq	-16(%rbp), %rax
	movb	%r15b, %bl
	movb	%bl, (%rax)
.Lread8:
	pushq	%rdi
	pushq	%rdx
	movl	$0, %edx
	movl	-23(%rbp), %r15d
	movl	%r15d, %edi
	cmpl	%edx, %edi
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.Lread9
	movb	-19(%rbp), %r15b
	movq	-16(%rbp), %rax
	movb	%r15b, %bl
	movb	%bl, (%rax)
	movl	$1, %eax
	leave
	ret
.Lread9:
	pushq	%rdi
	pushq	%rdx
	movq	$1, %rdx
	movq	-16(%rbp), %r15
	movq	%r15, %rdi
	add	%rdx, %rdi
	movq	%rdi, %rax
	popq	%rdx
	popq	%rdi
	movq	%rax, %rbx
	movq	%rbx, -16(%rbp)
.Lread7:
	pushq	%rdi
	pushq	%rdx
	movb	-17(%rbp), %r15b
	movb	%r15b, %dl
	movb	-18(%rbp), %r15b
	movb	%r15b, %dil
	cmpb	%dl, %dil
	setne	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$1, %al
	je	.Lread6
	movb	-19(%rbp), %r15b
	movq	-16(%rbp), %rax
	movb	%r15b, %bl
	movb	%bl, (%rax)
	movl	$0, %eax
	leave
	ret
pub_File_readToEnd:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$48, %rsp
	movq	%rdi, -8(%rbp)
	pushq	%rdx
	pushq	%rdi
	movl	$96, %eax
	movl	%eax, %edi
	call	malloc
	popq	%rdi
	popq	%rdx
	movq	%rax, %rbx
	movq	%rbx, -16(%rbp)
	pushq	%rdx
	pushq	%r14
	movq	-8(%rbp), %r14
	movl	8(%r14), %r15d
	popq	%r14
	pushq	%rdi
	movl	%r15d, %eax
	movl	%eax, %edi
	movq	-16(%rbp), %r15
	pushq	%rsi
	movq	%r15, %rax
	movq	%rax, %rsi
	call	sys_fstat
	popq	%rsi
	popq	%rdi
	popq	%rdx
	pushq	%rdx
	pushq	%rdi
	pushq	%rdx
	pushq	%r14
	movq	-16(%rbp), %r14
	movq	48(%r14), %r15
	popq	%r14
	movq	%r15, %rdx
	movl	$1, %edi
	add	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	pushq	%rdi
	movl	%eax, %eax
	movl	%eax, %edi
	call	malloc
	popq	%rdi
	popq	%rdx
	movq	%rax, %rbx
	movq	%rbx, -24(%rbp)
	pushq	%rdx
	pushq	%r14
	movq	-8(%rbp), %r14
	movl	8(%r14), %r15d
	popq	%r14
	pushq	%rdi
	movl	%r15d, %eax
	movl	%eax, %edi
	movq	-24(%rbp), %r15
	pushq	%rsi
	movq	%r15, %rax
	movq	%rax, %rsi
	pushq	%rdi
	pushq	%rdx
	pushq	%r14
	movq	-16(%rbp), %r14
	movq	48(%r14), %r15
	popq	%r14
	movq	%r15, %rdx
	movl	$0, %edi
	add	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	pushq	%rdx
	movl	%eax, %eax
	movl	%eax, %edx
	call	sys_read
	popq	%rdx
	popq	%rsi
	popq	%rdi
	popq	%rdx
	pushq	%rdi
	pushq	%rdx
	pushq	%rdi
	pushq	%rdx
	pushq	%r14
	movq	-16(%rbp), %r14
	movq	48(%r14), %r15
	popq	%r14
	movq	%r15, %rdx
	movl	$0, %edi
	add	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %edx
	movq	-24(%rbp), %r15
	movq	%r15, %rdi
	add	%rdx, %rdi
	movq	%rdi, %rax
	popq	%rdx
	popq	%rdi
	movq	%rax, %rbx
	movq	%rbx, -32(%rbp)
	movq	-32(%rbp), %rax
	movb	$0, %bl
	movb	%bl, (%rax)
	pushq	%rdx
	pushq	%rdi
	movl	$8, %eax
	movl	%eax, %edi
	call	malloc
	popq	%rdi
	popq	%rdx
	pushq	%rdi
	movq	%rax, %rdi
	pushq	%rdx
	movq	-24(%rbp), %r15
	pushq	%rsi
	movq	%r15, %rax
	movq	%rax, %rsi
	call	pub_String_init
	popq	%rsi
	popq	%rdi
	popq	%rdx
	movq	%rax, %rbx
	movq	%rbx, -40(%rbp)
	pushq	%rdx
	movq	-24(%rbp), %r15
	pushq	%rdi
	movq	%r15, %rax
	movq	%rax, %rdi
	call	free
	popq	%rdi
	popq	%rdx
	pushq	%rdx
	movq	-16(%rbp), %r15
	pushq	%rdi
	movq	%r15, %rax
	movq	%rax, %rdi
	call	free
	popq	%rdi
	popq	%rdx
	movq	-40(%rbp), %r15
	movq	%r15, %rax
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
	movq	-25(%rbp), %r15
	pushq	%rsi
	movq	%r15, %rax
	movq	%rax, %rsi
	movb	-9(%rbp), %r15b
	pushq	%rdx
	movb	%r15b, %al
	movb	%al, %dl
	call	pub_File_read
	popq	%rdx
	popq	%rsi
	popq	%rdi
	popq	%rdx
	movl	%eax, %ebx
	movl	%ebx, -29(%rbp)
	jmp	.LtoArray11
.LtoArray10:
	pushq	%r14
	movq	-17(%rbp), %r14
	movl	28(%r14), %r15d
	popq	%r14
	movl	%r15d, %ebx
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
	movl	-33(%rbp), %r15d
	pushq	%rsi
	movl	%r15d, %eax
	movl	%eax, %esi
	call	pub_Array_at
	popq	%rsi
	popq	%rdi
	popq	%rdx
	movq	%rax, %rbx
	movq	%rbx, -41(%rbp)
	movq	-25(%rbp), %r15
	movq	-41(%rbp), %rax
	movq	%r15, %rbx
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
	movq	-25(%rbp), %r15
	pushq	%rsi
	movq	%r15, %rax
	movq	%rax, %rsi
	movb	-9(%rbp), %r15b
	pushq	%rdx
	movb	%r15b, %al
	movb	%al, %dl
	call	pub_File_read
	popq	%rdx
	popq	%rsi
	popq	%rdi
	popq	%rdx
	movl	%eax, %ebx
	movl	%ebx, -29(%rbp)
.LtoArray11:
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movl	-29(%rbp), %r15d
	movl	%r15d, %edi
	cmpl	%edx, %edi
	setne	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$1, %al
	je	.LtoArray10
	pushq	%r14
	movq	-17(%rbp), %r14
	movl	28(%r14), %r15d
	popq	%r14
	movl	%r15d, %ebx
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
	movl	-33(%rbp), %r15d
	pushq	%rsi
	movl	%r15d, %eax
	movl	%eax, %esi
	call	pub_Array_at
	popq	%rsi
	popq	%rdi
	popq	%rdx
	movq	%rax, %rbx
	movq	%rbx, -41(%rbp)
	movq	-25(%rbp), %r15
	movq	-41(%rbp), %rax
	movq	%r15, %rbx
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
	movq	-17(%rbp), %r15
	movq	%r15, %rax
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
	movl	$16, %eax
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
	movq	-25(%rbp), %r15
	pushq	%rsi
	movq	%r15, %rax
	movq	%rax, %rsi
	movb	-9(%rbp), %r15b
	pushq	%rdx
	movb	%r15b, %al
	movb	%al, %dl
	call	pub_File_read
	popq	%rdx
	popq	%rsi
	popq	%rdi
	popq	%rdx
	movl	%eax, %ebx
	movl	%ebx, -29(%rbp)
	jmp	.LtoList13
.LtoList12:
	pushq	%rdx
	lea	-17(%rbp), %rax
	pushq	%rdi
	movq	(%rax), %rax
	movq	%rax, %rdi
	movq	-25(%rbp), %r15
	pushq	%rsi
	movq	%r15, %rax
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
	movq	-25(%rbp), %r15
	pushq	%rsi
	movq	%r15, %rax
	movq	%rax, %rsi
	movb	-9(%rbp), %r15b
	pushq	%rdx
	movb	%r15b, %al
	movb	%al, %dl
	call	pub_File_read
	popq	%rdx
	popq	%rsi
	popq	%rdi
	popq	%rdx
	movl	%eax, %ebx
	movl	%ebx, -29(%rbp)
.LtoList13:
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movl	-29(%rbp), %r15d
	movl	%r15d, %edi
	cmpl	%edx, %edi
	setne	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$1, %al
	je	.LtoList12
	pushq	%rdx
	lea	-17(%rbp), %rax
	pushq	%rdi
	movq	(%rax), %rax
	movq	%rax, %rdi
	movq	-25(%rbp), %r15
	pushq	%rsi
	movq	%r15, %rax
	movq	%rax, %rsi
	call	pub_LinkedList_append
	popq	%rsi
	popq	%rdi
	popq	%rdx
	movq	-17(%rbp), %r15
	movq	%r15, %rax
	leave
	ret
lambda_1650397015394_0:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$16, %rsp
	pushq	%rdx
	pushq	%rdi
	movq	$.strICollection.lambda_1650397015394_01, %rax
	movq	%rax, %rdi
	call	panic
	popq	%rdi
	popq	%rdx
	leave
	ret
lambda_1650397015394_2:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$16, %rsp
	pushq	%rdx
	pushq	%rdi
	movq	$.strICollection.lambda_1650397015394_23, %rax
	movq	%rax, %rdi
	call	panic
	popq	%rdi
	popq	%rdx
	leave
	ret


.data

.strICollection.lambda_1650397015394_23:
	.asciz	 "Function (where) not implemented on this instance"
.strICollection.lambda_1650397015394_01:
	.asciz	 "Function (forEach) not implemented on this instance"


.bss

