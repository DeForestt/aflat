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
	subq	$48, %rsp
	movq	%rdi, -28(%rbp)
	movl	$12, %eax
	movl	%eax, %edi
	call	malloc
	movq	%rax, -36(%rbp)
	movq	$2, -44(%rbp)
	movq	-28(%rbp), %rax
	movq	%rax, %rdi
	movq	-44(%rbp), %rax
	movq	%rax, %rsi
	movq	$777, %rax
	movq	%rax, %rdx
	call	sys_open
	movq	-36(%rbp), %rdx
	movl	%eax, %ebx
	movl	%ebx, 8(%rdx)
	movq	-36(%rbp), %rdx
	movl	8(%rdx), %ebx
	movl	%ebx, -48(%rbp)
	movl	-48(%rbp), %eax
	movl	$0, %ecx
	cmpl	%ecx, %eax
	jge	.LopenFile3
	movq	$0, %rax
	leave
	ret
.LopenFile3:
	movq	-36(%rbp), %rdx
	movq	-28(%rbp), %rbx
	movq	%rbx, 0(%rdx)
	movq	-36(%rbp), %rax
	leave
	ret
pub_File_close:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$56, %rsp
	movq	%rdi, -56(%rbp)
	movq	-56(%rbp), %rdx
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
	movq	%rdi, -64(%rbp)
	movq	%rsi, -72(%rbp)
	movq	-72(%rbp), %rax
	movq	%rax, %rdi
	call	len
	movl	%eax, -76(%rbp)
	movq	-64(%rbp), %rdx
	movl	8(%rdx), %eax
	movl	%eax, %edi
	movq	-72(%rbp), %rax
	movq	%rax, %rsi
	movl	-76(%rbp), %eax
	movl	%eax, %edx
	call	sys_write
	movl	%eax, -80(%rbp)
	movl	-80(%rbp), %eax
	leave
	ret
pub_File_read:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$101, %rsp
	movq	%rdi, -88(%rbp)
	movq	%rsi, -96(%rbp)
	movb	%dl, -97(%rbp)
	movb	%cl, -98(%rbp)
	movb	$0, %bl
	movb	%bl, -99(%rbp)
	movb	$0, -100(%rbp)
	jmp	.Lread7
.Lread6:
	movq	-88(%rbp), %rdx
	movl	8(%rdx), %eax
	movl	%eax, %edi
	lea	-99(%rbp), %rax
	movq	%rax, %rax
	movq	%rax, %rsi
	movl	$1, %eax
	movl	%eax, %edx
	call	sys_read
	movb	-99(%rbp), %bl
	movb	%bl, -101(%rbp)
	movb	-101(%rbp), %al
	movb	-98(%rbp), %cl
	cmpb	%cl, %al
	jne	.Lread8
	movl	$1, %eax
	leave
	ret
.Lread8:
	movq	-96(%rbp), %rax
	movb	-101(%rbp), %bl
	movb	%bl, (%rax)
	movb	-101(%rbp), %al
	movb	-97(%rbp), %cl
	cmpb	%cl, %al
	jne	.Lread11
	movq	-96(%rbp), %rax
	movb	-100(%rbp), %bl
	movb	%bl, (%rax)
.Lread11:
	mov	$1, %rdx
	mov	-96(%rbp), %rax
	add	%rdx, %rax
	movl	%eax, %ebx
	movl	%ebx, -96(%rbp)
.Lread7:
	movb	-99(%rbp), %al
	movb	-97(%rbp), %cl
	cmpb	%cl, %al
	jne	.Lread6
	movq	-96(%rbp), %rax
	movb	-100(%rbp), %bl
	movb	%bl, (%rax)
	movl	$0, %eax
	leave
	ret
pub_File_toArray:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$143, %rsp
	movq	%rdi, -109(%rbp)
	movb	%sil, -110(%rbp)
	movb	%dl, -111(%rbp)
	movl	$8, %eax
	movl	%eax, %edi
	movl	$0, %eax
	movl	%eax, %esi
	call	newArray
	movq	%rax, -119(%rbp)
	movl	$256, %eax
	movl	%eax, %edi
	call	malloc
	movq	%rax, -127(%rbp)
	lea	-109(%rbp), %rax
	movq	(%rax), %rax
	movq	%rax, %rdi
	movq	-127(%rbp), %rax
	movq	%rax, %rsi
	movb	-110(%rbp), %al
	movb	%al, %dl
	movb	-111(%rbp), %al
	movb	%al, %cl
	call	pub_File_read
	movl	%eax, -131(%rbp)
	jmp	.LtoArray15
.LtoArray14:
	movq	-119(%rbp), %rdx
	movl	12(%rdx), %ebx
	movl	%ebx, -135(%rbp)
	lea	-119(%rbp), %rax
	movq	(%rax), %rax
	movq	%rax, %rdi
	movl	$1, %eax
	movl	%eax, %esi
	call	pub_Array_extend
	lea	-119(%rbp), %rax
	movq	(%rax), %rax
	movq	%rax, %rdi
	movl	-135(%rbp), %eax
	movl	%eax, %esi
	call	pub_Array_at
	movq	%rax, -143(%rbp)
	movl	-131(%rbp), %eax
	movl	$1, %ecx
	cmpl	%ecx, %eax
	je	.LtoArray16
	movq	-143(%rbp), %rax
	movq	-127(%rbp), %rbx
	movq	%rbx, (%rax)
	movl	$256, %eax
	movl	%eax, %edi
	call	malloc
	movq	%rax, %rbx
	movq	%rbx, -127(%rbp)
.LtoArray16:
	lea	-109(%rbp), %rax
	movq	(%rax), %rax
	movq	%rax, %rdi
	movq	-127(%rbp), %rax
	movq	%rax, %rsi
	movb	-110(%rbp), %al
	movb	%al, %dl
	movb	-111(%rbp), %al
	movb	%al, %cl
	call	pub_File_read
	movl	%eax, %ebx
	movl	%ebx, -131(%rbp)
.LtoArray15:
	movl	-131(%rbp), %eax
	movl	$1, %ecx
	cmpl	%ecx, %eax
	jne	.LtoArray14
	movq	-119(%rbp), %rax
	leave
	ret


.data



.bss

