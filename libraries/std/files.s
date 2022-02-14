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
	subq	$20, %rsp
	movq	%rdi, -0(%rbp)
	movl	$12, %eax
	movl	%eax, %edi
	call	malloc
	movq	%rax, -8(%rbp)
	movq	-0(%rbp), %rax
	movq	%rax, %rdi
	movq	$511, %rax
	movq	%rax, %rsi
	call	sys_create
	movq	-8(%rbp), %rdx
	movl	%eax, %ebx
	movl	%ebx, 8(%rdx)
	movq	-8(%rbp), %rdx
	movl	8(%rdx), %ebx
	movl	%ebx, -16(%rbp)
	movl	-16(%rbp), %eax
	movl	$1, %ecx
	cmpl	%ecx, %eax
	jge	.LcreateFile0
	movq	$0, %rax
	leave
	ret
.LcreateFile0:
	movq	-8(%rbp), %rax
	leave
	ret
openFile:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$48, %rsp
	movq	%rdi, -20(%rbp)
	movl	$12, %eax
	movl	%eax, %edi
	call	malloc
	movq	%rax, -28(%rbp)
	movq	$2, -36(%rbp)
	movq	-20(%rbp), %rax
	movq	%rax, %rdi
	movq	-36(%rbp), %rax
	movq	%rax, %rsi
	movq	$777, %rax
	movq	%rax, %rdx
	call	sys_open
	movq	-28(%rbp), %rdx
	movl	%eax, %ebx
	movl	%ebx, 8(%rdx)
	movq	-28(%rbp), %rdx
	movl	8(%rdx), %ebx
	movl	%ebx, -44(%rbp)
	movl	-44(%rbp), %eax
	movl	$0, %ecx
	cmpl	%ecx, %eax
	jge	.LopenFile3
	movq	$0, %rax
	leave
	ret
.LopenFile3:
	movq	-28(%rbp), %rdx
	movq	-20(%rbp), %rbx
	movq	%rbx, 0(%rdx)
	movq	-28(%rbp), %rax
	leave
	ret
pub_File_close:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$56, %rsp
	movq	%rdi, -48(%rbp)
	movq	-48(%rbp), %rdx
	movl	8(%rdx), %eax
	movl	%eax, %edi
	call	sys_close
	movl	%eax, %eax
	leave
	ret
pub_File_write:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$80, %rsp
	movq	%rdi, -56(%rbp)
	movq	%rsi, -64(%rbp)
	movq	-64(%rbp), %rax
	movq	%rax, %rdi
	call	len
	movl	%eax, -72(%rbp)
	movq	-56(%rbp), %rdx
	movl	8(%rdx), %eax
	movl	%eax, %edi
	movq	-64(%rbp), %rax
	movq	%rax, %rsi
	movl	-72(%rbp), %eax
	movl	%eax, %edx
	call	sys_write
	movl	%eax, -76(%rbp)
	movl	-76(%rbp), %eax
	leave
	ret
pub_File_read:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$108, %rsp
	movq	%rdi, -80(%rbp)
	movq	%rsi, -88(%rbp)
	movb	%dl, -96(%rbp)
	movb	%cl, -97(%rbp)
	movl	$1, %eax
	movl	%eax, %edi
	call	malloc
	movq	%rax, -98(%rbp)
	movq	-98(%rbp), %rax
	movb	$0, %bl
	movb	%bl, (%rax)
	movb	$0, -106(%rbp)
	jmp	.Lread7
.Lread6:
	movq	-80(%rbp), %rdx
	movl	8(%rdx), %eax
	movl	%eax, %edi
	movq	-98(%rbp), %rax
	movq	%rax, %rsi
	movl	$1, %eax
	movl	%eax, %edx
	call	sys_read
	movq	-98(%rbp), %rax
	movb	(%rax), %al
	movb	%al, -107(%rbp)
	movb	-107(%rbp), %al
	movb	-97(%rbp), %cl
	cmpb	%cl, %al
	jne	.Lread8
	movl	$1, %eax
	leave
	ret
.Lread8:
	movq	-88(%rbp), %rax
	movb	-107(%rbp), %bl
	movb	%bl, (%rax)
	movb	-107(%rbp), %al
	movb	-96(%rbp), %cl
	cmpb	%cl, %al
	jne	.Lread11
	movq	-88(%rbp), %rax
	movb	-106(%rbp), %bl
	movb	%bl, (%rax)
.Lread11:
	mov	$1, %rdx
	mov	-88(%rbp), %rax
	add	%rdx, %rax
	movl	%eax, %ebx
	movl	%ebx, -88(%rbp)
.Lread7:
	movq	-98(%rbp), %rax
	movb	(%rax), %al
	movb	%al, %al
	movb	-96(%rbp), %cl
	cmpb	%cl, %al
	jne	.Lread6
	movq	-88(%rbp), %rax
	movb	-106(%rbp), %bl
	movb	%bl, (%rax)
	movq	-98(%rbp), %rax
	movq	%rax, %rdi
	call	free
	movl	$0, %eax
	leave
	ret
pub_File_toArray:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$150, %rsp
	movq	%rdi, -108(%rbp)
	movb	%sil, -116(%rbp)
	movb	%dl, -117(%rbp)
	movl	$8, %eax
	movl	%eax, %edi
	movl	$0, %eax
	movl	%eax, %esi
	call	newArray
	movq	%rax, -118(%rbp)
	movl	$256, %eax
	movl	%eax, %edi
	call	malloc
	movq	%rax, -126(%rbp)
	lea	-108(%rbp), %rax
	movq	(%rax), %rax
	movq	%rax, %rdi
	movq	-126(%rbp), %rax
	movq	%rax, %rsi
	movb	-116(%rbp), %al
	movb	%al, %dl
	movb	-117(%rbp), %al
	movb	%al, %cl
	call	pub_File_read
	movl	%eax, -134(%rbp)
	jmp	.LtoArray15
.LtoArray14:
	movq	-118(%rbp), %rdx
	movl	12(%rdx), %ebx
	movl	%ebx, -138(%rbp)
	lea	-118(%rbp), %rax
	movq	(%rax), %rax
	movq	%rax, %rdi
	movl	$1, %eax
	movl	%eax, %esi
	call	pub_Array_extend
	lea	-118(%rbp), %rax
	movq	(%rax), %rax
	movq	%rax, %rdi
	movl	-138(%rbp), %eax
	movl	%eax, %esi
	call	pub_Array_at
	movq	%rax, -142(%rbp)
	movl	-134(%rbp), %eax
	movl	$1, %ecx
	cmpl	%ecx, %eax
	je	.LtoArray16
	movq	-142(%rbp), %rax
	movq	-126(%rbp), %rbx
	movq	%rbx, (%rax)
	movl	$256, %eax
	movl	%eax, %edi
	call	malloc
	movq	%rax, %rbx
	movq	%rbx, -126(%rbp)
.LtoArray16:
	lea	-108(%rbp), %rax
	movq	(%rax), %rax
	movq	%rax, %rdi
	movq	-126(%rbp), %rax
	movq	%rax, %rsi
	movb	-116(%rbp), %al
	movb	%al, %dl
	movb	-117(%rbp), %al
	movb	%al, %cl
	call	pub_File_read
	movl	%eax, %ebx
	movl	%ebx, -134(%rbp)
.LtoArray15:
	movl	-134(%rbp), %eax
	movl	$1, %ecx
	cmpl	%ecx, %eax
	jne	.LtoArray14
	movq	-118(%rbp), %rax
	leave
	ret


.data



.bss

