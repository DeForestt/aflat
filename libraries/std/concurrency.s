.global	concurrency.wait
.global	concurrency.exit
.global	pub_Pipe_write
.global	pub_Pipe_read
.global	pub_Pipe_init
.global	pub_Message_writePipe
.global	concurrency.readPipe
.global	pub_MProcess_output
.global	pub_MProcess_input
.global	pub_Process_isRunning
.global	pub_Thread_getPid
.global	pub_MProcess_start
.global	pub_Process_start
.global	pub_MProcess_init
.global	pub_Process_init
.global	pub_Message_init


.text

pub_Message_init:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$16, %rsp
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	pushq	%r14
	movq	-8(%rbp), %r14
	pushq	%rdx
	movq	-16(%rbp), %r15
	pushq	%rdi
	movq	%r15, %rax
	movq	%rax, %rdi
	call	strings.len
	popq	%rdi
	popq	%rdx
	movl	%eax, %ebx
	movl	%ebx, 0(%r14)
	popq	%r14
	pushq	%r14
	movq	-8(%rbp), %r14
	movq	-16(%rbp), %r15
	movq	%r15, %rbx
	movq	%rbx, 4(%r14)
	popq	%r14
	popq	%rbx
	movq	-8(%rbp), %rax
	leave
	ret
pub_Process_init:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$16, %rsp
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	pushq	%r14
	movq	-8(%rbp), %r14
	movq	-16(%rbp), %r15
	movq	%r15, %rbx
	movq	%rbx, 4(%r14)
	popq	%r14
	pushq	%r14
	movq	-8(%rbp), %r14
	movl	$0, %ebx
	movl	%ebx, 0(%r14)
	popq	%r14
	popq	%rbx
	movq	-8(%rbp), %rax
	leave
	ret
pub_MProcess_init:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$16, %rsp
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	pushq	%r14
	movq	-8(%rbp), %r14
	movq	-16(%rbp), %r15
	movq	%r15, %rbx
	movq	%rbx, 4(%r14)
	popq	%r14
	pushq	%r14
	movq	-8(%rbp), %r14
	movl	$0, %ebx
	movl	%ebx, 0(%r14)
	popq	%r14
	pushq	%r14
	movq	-8(%rbp), %r14
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
	call	pub_Pipe_init
	popq	%rdi
	popq	%rdx
	movq	%rax, %rbx
	movq	%rbx, 12(%r14)
	popq	%r14
	pushq	%r14
	movq	-8(%rbp), %r14
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
	call	pub_Pipe_init
	popq	%rdi
	popq	%rdx
	movq	%rax, %rbx
	movq	%rbx, 20(%r14)
	popq	%r14
	popq	%rbx
	movq	-8(%rbp), %rax
	leave
	ret
pub_Process_start:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$32, %rsp
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	pushq	%r14
	movq	-8(%rbp), %r14
	pushq	%rdx
	call	sys_fork
	popq	%rdx
	movl	%eax, %ebx
	movl	%ebx, 0(%r14)
	popq	%r14
	pushq	%r14
	movq	-8(%rbp), %r14
	movq	4(%r14), %r15
	popq	%r14
	movq	%r15, %rbx
	movq	%rbx, -24(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	$0, %edx
	pushq	%r14
	movq	-8(%rbp), %r14
	movl	0(%r14), %r15d
	popq	%r14
	movl	%r15d, %edi
	cmpl	%edx, %edi
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.Lstart0
	pushq	%rdx
	movq	-24(%rbp), %r15
	movq	%r15, %r11
	movq	-16(%rbp), %r15
	pushq	%rdi
	movq	%r15, %rax
	movq	%rax, %rdi
	call	*%r11
	popq	%rdi
	popq	%rdx
	pushq	%rdx
	pushq	%rdi
	movl	$0, %eax
	movl	%eax, %edi
	call	sys_exit
	popq	%rdi
	popq	%rdx
.Lstart0:
	movl	$0, %eax
	leave
	ret
pub_MProcess_start:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$32, %rsp
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	pushq	%r14
	movq	-8(%rbp), %r14
	pushq	%rdx
	call	sys_fork
	popq	%rdx
	movl	%eax, %ebx
	movl	%ebx, 0(%r14)
	popq	%r14
	pushq	%r14
	movq	-8(%rbp), %r14
	movq	4(%r14), %r15
	popq	%r14
	movq	%r15, %rbx
	movq	%rbx, -24(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	$0, %edx
	pushq	%r14
	movq	-8(%rbp), %r14
	movl	0(%r14), %r15d
	popq	%r14
	movl	%r15d, %edi
	cmpl	%edx, %edi
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.Lstart1
	pushq	%rdx
	movq	-24(%rbp), %r15
	movq	%r15, %r11
	movq	-16(%rbp), %r15
	pushq	%rdi
	movq	%r15, %rax
	movq	%rax, %rdi
	pushq	%r14
	movq	-8(%rbp), %r14
	movq	12(%r14), %r15
	popq	%r14
	pushq	%rsi
	movq	%r15, %rax
	movq	%rax, %rsi
	pushq	%r14
	movq	-8(%rbp), %r14
	movq	20(%r14), %r15
	popq	%r14
	pushq	%rdx
	movq	%r15, %rax
	movq	%rax, %rdx
	call	*%r11
	popq	%rdx
	popq	%rsi
	popq	%rdi
	popq	%rdx
	pushq	%rdx
	pushq	%rdi
	movl	$0, %eax
	movl	%eax, %edi
	call	sys_exit
	popq	%rdi
	popq	%rdx
.Lstart1:
	movl	$0, %eax
	leave
	ret
pub_Thread_getPid:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$16, %rsp
	movq	%rdi, -8(%rbp)
	pushq	%r14
	movq	-8(%rbp), %r14
	movl	0(%r14), %r15d
	popq	%r14
	movl	%r15d, %eax
	leave
	ret
pub_Process_isRunning:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$16, %rsp
	movq	%rdi, -8(%rbp)
	pushq	%rdx
	pushq	%r14
	movq	-8(%rbp), %r14
	movl	0(%r14), %r15d
	popq	%r14
	pushq	%rdi
	movl	%r15d, %eax
	movl	%eax, %edi
	pushq	%rsi
	movl	$0, %eax
	movl	%eax, %esi
	call	sys_kill
	popq	%rsi
	popq	%rdi
	popq	%rdx
	movl	%eax, %ebx
	movl	%ebx, -12(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	$0, %edx
	movl	-12(%rbp), %r15d
	movl	%r15d, %edi
	cmpl	%edx, %edi
	setge	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	leave
	ret
pub_MProcess_input:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$64, %rsp
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	lea	-36(%rbp), %rax
	pushq	%rdi
	movq	%rax, %rdi
	pushq	%rdx
	movq	-16(%rbp), %r15
	pushq	%rsi
	movq	%r15, %rax
	movq	%rax, %rsi
	call	pub_Message_init
	popq	%rsi
	popq	%rdi
	popq	%rdx
	movq	%rax, %rbx
	movq	%rbx, -24(%rbp)
	pushq	%r14
	movq	-8(%rbp), %r14
	movq	12(%r14), %r15
	popq	%r14
	movq	%r15, %rbx
	movq	%rbx, -44(%rbp)
	pushq	%rdx
	pushq	%rdi
	movl	$5, %eax
	movl	%eax, %edi
	call	malloc
	popq	%rdi
	popq	%rdx
	movq	%rax, %rbx
	movq	%rbx, -52(%rbp)
	pushq	%rdx
	pushq	%r14
	movq	-24(%rbp), %r14
	movl	0(%r14), %r15d
	popq	%r14
	pushq	%rdi
	movl	%r15d, %eax
	movl	%eax, %edi
	movq	-52(%rbp), %r15
	pushq	%rsi
	movq	%r15, %rax
	movq	%rax, %rsi
	pushq	%rdx
	movl	$5, %eax
	movl	%eax, %edx
	call	strings.int_toStringL0
	popq	%rdx
	popq	%rsi
	popq	%rdi
	popq	%rdx
	pushq	%rdx
	lea	-44(%rbp), %rax
	pushq	%rdi
	movq	(%rax), %rax
	movq	%rax, %rdi
	movq	-52(%rbp), %r15
	pushq	%rsi
	movq	%r15, %rax
	movq	%rax, %rsi
	pushq	%rdx
	movl	$5, %eax
	movl	%eax, %edx
	call	pub_Pipe_write
	popq	%rdx
	popq	%rsi
	popq	%rdi
	popq	%rdx
	movl	%eax, %ebx
	movl	%ebx, -56(%rbp)
	pushq	%rdx
	lea	-44(%rbp), %rax
	pushq	%rdi
	movq	(%rax), %rax
	movq	%rax, %rdi
	pushq	%r14
	movq	-24(%rbp), %r14
	movq	4(%r14), %r15
	popq	%r14
	pushq	%rsi
	movq	%r15, %rax
	movq	%rax, %rsi
	pushq	%r14
	movq	-24(%rbp), %r14
	movl	0(%r14), %r15d
	popq	%r14
	pushq	%rdx
	movl	%r15d, %eax
	movl	%eax, %edx
	call	pub_Pipe_write
	popq	%rdx
	popq	%rsi
	popq	%rdi
	popq	%rdx
	movl	%eax, %ebx
	movl	%ebx, -56(%rbp)
	pushq	%rdx
	movq	-52(%rbp), %r15
	pushq	%rdi
	movq	%r15, %rax
	movq	%rax, %rdi
	call	free
	popq	%rdi
	popq	%rdx
	movb	$1, %al
	leave
	ret
pub_MProcess_output:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$48, %rsp
	movq	%rdi, -8(%rbp)
	pushq	%r14
	movq	-8(%rbp), %r14
	movq	20(%r14), %r15
	popq	%r14
	movq	%r15, %rbx
	movq	%rbx, -16(%rbp)
	pushq	%rdx
	pushq	%rdi
	movl	$5, %eax
	movl	%eax, %edi
	call	malloc
	popq	%rdi
	popq	%rdx
	movq	%rax, %rbx
	movq	%rbx, -24(%rbp)
	pushq	%rdx
	lea	-16(%rbp), %rax
	pushq	%rdi
	movq	(%rax), %rax
	movq	%rax, %rdi
	movq	-24(%rbp), %r15
	pushq	%rsi
	movq	%r15, %rax
	movq	%rax, %rsi
	pushq	%rdx
	movl	$5, %eax
	movl	%eax, %edx
	call	pub_Pipe_read
	popq	%rdx
	popq	%rsi
	popq	%rdi
	popq	%rdx
	pushq	%rdx
	movq	-24(%rbp), %r15
	pushq	%rdi
	movq	%r15, %rax
	movq	%rax, %rdi
	call	strings.str_toInt
	popq	%rdi
	popq	%rdx
	movl	%eax, %ebx
	movl	%ebx, -28(%rbp)
	pushq	%rdx
	movq	-24(%rbp), %r15
	pushq	%rdi
	movq	%r15, %rax
	movq	%rax, %rdi
	call	free
	popq	%rdi
	popq	%rdx
	pushq	%rdx
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movl	-28(%rbp), %r15d
	movl	%r15d, %edi
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
	movq	%rbx, -36(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	-28(%rbp), %r15d
	movl	%r15d, %edx
	movq	-36(%rbp), %r15
	movq	%r15, %rdi
	add	%rdx, %rdi
	movq	%rdi, %rax
	popq	%rdx
	popq	%rdi
	movq	%rax, %rbx
	movq	%rbx, -44(%rbp)
	movq	-44(%rbp), %rax
	movb	$0, %bl
	movb	%bl, (%rax)
	pushq	%rdx
	lea	-16(%rbp), %rax
	pushq	%rdi
	movq	(%rax), %rax
	movq	%rax, %rdi
	movq	-36(%rbp), %r15
	pushq	%rsi
	movq	%r15, %rax
	movq	%rax, %rsi
	movl	-28(%rbp), %r15d
	pushq	%rdx
	movl	%r15d, %eax
	movl	%eax, %edx
	call	pub_Pipe_read
	popq	%rdx
	popq	%rsi
	popq	%rdi
	popq	%rdx
	movq	-36(%rbp), %r15
	movq	%r15, %rax
	leave
	ret
concurrency.readPipe:
readPipe:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$48, %rsp
	movq	%rdi, -8(%rbp)
	pushq	%rdx
	pushq	%rdi
	movl	$5, %eax
	movl	%eax, %edi
	call	malloc
	popq	%rdi
	popq	%rdx
	movq	%rax, %rbx
	movq	%rbx, -16(%rbp)
	pushq	%rdx
	lea	-8(%rbp), %rax
	pushq	%rdi
	movq	(%rax), %rax
	movq	%rax, %rdi
	movq	-16(%rbp), %r15
	pushq	%rsi
	movq	%r15, %rax
	movq	%rax, %rsi
	pushq	%rdx
	movl	$5, %eax
	movl	%eax, %edx
	call	pub_Pipe_read
	popq	%rdx
	popq	%rsi
	popq	%rdi
	popq	%rdx
	pushq	%rdx
	movq	-16(%rbp), %r15
	pushq	%rdi
	movq	%r15, %rax
	movq	%rax, %rdi
	call	strings.str_toInt
	popq	%rdi
	popq	%rdx
	movl	%eax, %ebx
	movl	%ebx, -20(%rbp)
	pushq	%rdx
	movq	-16(%rbp), %r15
	pushq	%rdi
	movq	%r15, %rax
	movq	%rax, %rdi
	call	free
	popq	%rdi
	popq	%rdx
	pushq	%rdx
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movl	-20(%rbp), %r15d
	movl	%r15d, %edi
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
	movq	%rbx, -28(%rbp)
	pushq	%rdx
	lea	-8(%rbp), %rax
	pushq	%rdi
	movq	(%rax), %rax
	movq	%rax, %rdi
	movq	-28(%rbp), %r15
	pushq	%rsi
	movq	%r15, %rax
	movq	%rax, %rsi
	movl	-20(%rbp), %r15d
	pushq	%rdx
	movl	%r15d, %eax
	movl	%eax, %edx
	call	pub_Pipe_read
	popq	%rdx
	popq	%rsi
	popq	%rdi
	popq	%rdx
	lea	-48(%rbp), %rax
	pushq	%rdi
	movq	%rax, %rdi
	pushq	%rdx
	movq	-28(%rbp), %r15
	pushq	%rsi
	movq	%r15, %rax
	movq	%rax, %rsi
	call	pub_Message_init
	popq	%rsi
	popq	%rdi
	popq	%rdx
	movq	%rax, %rbx
	movq	%rbx, -36(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	-20(%rbp), %r15d
	movl	%r15d, %edx
	movq	-28(%rbp), %r15
	movq	%r15, %rdi
	add	%rdx, %rdi
	movq	%rdi, %rax
	popq	%rdx
	popq	%rdi
	movq	%rax, %rbx
	movq	%rbx, -28(%rbp)
	movq	-28(%rbp), %rax
	movb	$0, %bl
	movb	%bl, (%rax)
	movq	-36(%rbp), %r15
	movq	%r15, %rax
	leave
	ret
pub_Message_writePipe:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$32, %rsp
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	pushq	%rdx
	pushq	%rdi
	movl	$5, %eax
	movl	%eax, %edi
	call	malloc
	popq	%rdi
	popq	%rdx
	movq	%rax, %rbx
	movq	%rbx, -24(%rbp)
	pushq	%rdx
	pushq	%r14
	movq	-8(%rbp), %r14
	movl	0(%r14), %r15d
	popq	%r14
	pushq	%rdi
	movl	%r15d, %eax
	movl	%eax, %edi
	movq	-24(%rbp), %r15
	pushq	%rsi
	movq	%r15, %rax
	movq	%rax, %rsi
	pushq	%rdx
	movl	$5, %eax
	movl	%eax, %edx
	call	strings.int_toStringL0
	popq	%rdx
	popq	%rsi
	popq	%rdi
	popq	%rdx
	pushq	%rdx
	lea	-16(%rbp), %rax
	pushq	%rdi
	movq	(%rax), %rax
	movq	%rax, %rdi
	movq	-24(%rbp), %r15
	pushq	%rsi
	movq	%r15, %rax
	movq	%rax, %rsi
	pushq	%rdx
	movl	$5, %eax
	movl	%eax, %edx
	call	pub_Pipe_write
	popq	%rdx
	popq	%rsi
	popq	%rdi
	popq	%rdx
	movl	%eax, %ebx
	movl	%ebx, -28(%rbp)
	pushq	%rdx
	lea	-16(%rbp), %rax
	pushq	%rdi
	movq	(%rax), %rax
	movq	%rax, %rdi
	pushq	%r14
	movq	-8(%rbp), %r14
	movq	4(%r14), %r15
	popq	%r14
	pushq	%rsi
	movq	%r15, %rax
	movq	%rax, %rsi
	pushq	%r14
	movq	-8(%rbp), %r14
	movl	0(%r14), %r15d
	popq	%r14
	pushq	%rdx
	movl	%r15d, %eax
	movl	%eax, %edx
	call	pub_Pipe_write
	popq	%rdx
	popq	%rsi
	popq	%rdi
	popq	%rdx
	movl	%eax, %ebx
	movl	%ebx, -28(%rbp)
	pushq	%rdx
	movq	-24(%rbp), %r15
	pushq	%rdi
	movq	%r15, %rax
	movq	%rax, %rdi
	call	free
	popq	%rdi
	popq	%rdx
	movl	$0, %eax
	leave
	ret
pub_Pipe_init:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$32, %rsp
	movq	%rdi, -8(%rbp)
	lea	-24(%rbp), %rax
	movq	%rax, %rbx
	movq	%rbx, -16(%rbp)
	pushq	%rdx
	movq	-16(%rbp), %r15
	pushq	%rdi
	movq	%r15, %rax
	movq	%rax, %rdi
	call	sys_pipe
	popq	%rdi
	popq	%rdx
	pushq	%r14
	movq	-8(%rbp), %r14
	pushq	%r14
	movq	-16(%rbp), %r14
	movl	0(%r14), %r15d
	popq	%r14
	movl	%r15d, %ebx
	movl	%ebx, 4(%r14)
	popq	%r14
	pushq	%r14
	movq	-8(%rbp), %r14
	pushq	%r14
	movq	-16(%rbp), %r14
	movl	4(%r14), %r15d
	popq	%r14
	movl	%r15d, %ebx
	movl	%ebx, 0(%r14)
	popq	%r14
	popq	%rbx
	movq	-8(%rbp), %rax
	leave
	ret
pub_Pipe_read:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$32, %rsp
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	movl	%edx, -20(%rbp)
	pushq	%r14
	movq	-8(%rbp), %r14
	movl	4(%r14), %r15d
	popq	%r14
	movl	%r15d, %ebx
	movl	%ebx, -24(%rbp)
	pushq	%rdx
	movl	-24(%rbp), %r15d
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
	call	sys_read
	popq	%rdx
	popq	%rsi
	popq	%rdi
	popq	%rdx
	movl	%eax, %ebx
	movl	%ebx, -28(%rbp)
	movl	-28(%rbp), %r15d
	movl	%r15d, %eax
	leave
	ret
pub_Pipe_write:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$32, %rsp
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	movl	%edx, -20(%rbp)
	pushq	%r14
	movq	-8(%rbp), %r14
	movl	0(%r14), %r15d
	popq	%r14
	movl	%r15d, %ebx
	movl	%ebx, -24(%rbp)
	pushq	%rdx
	movl	-24(%rbp), %r15d
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
	movl	%ebx, -28(%rbp)
	movl	-28(%rbp), %r15d
	movl	%r15d, %eax
	leave
	ret
concurrency.exit:
exit:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$16, %rsp
	movl	%edi, -4(%rbp)
	pushq	%rdx
	movl	-4(%rbp), %r15d
	pushq	%rdi
	movl	%r15d, %eax
	movl	%eax, %edi
	call	sys_exit
	popq	%rdi
	popq	%rdx
	movl	$0, %eax
	leave
	ret
concurrency.wait:
wait:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$16, %rsp
	movq	%rdi, -8(%rbp)
	pushq	%rdx
	pushq	%rdx
	lea	-8(%rbp), %rax
	pushq	%rdi
	movq	(%rax), %rax
	movq	%rax, %rdi
	call	pub_Thread_getPid
	popq	%rdi
	popq	%rdx
	pushq	%rdi
	movl	%eax, %eax
	movl	%eax, %edi
	pushq	%rsi
	movq	$0, %rax
	movq	%rax, %rsi
	pushq	%rdx
	movl	$0, %eax
	movl	%eax, %edx
	pushq	%rcx
	movq	$0, %rax
	movq	%rax, %rcx
	call	sys_wait
	popq	%rcx
	popq	%rdx
	popq	%rsi
	popq	%rdi
	popq	%rdx
	movl	$0, %eax
	leave
	ret


.data



.bss

