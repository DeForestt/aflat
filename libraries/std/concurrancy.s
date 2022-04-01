.global	concurrancy.wait
.global	concurrancy.exit
.global	pub_Pipe_write
.global	pub_Pipe_read
.global	pub_Pipe_init
.global	pub_Message_writePipe
.global	concurrancy.readPipe
.global	pub_MProcess_output
.global	pub_MProcess_input
.global	pub_Process_isRunning
.global	pub_MProcess_getPid
.global	pub_Process_getPid
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
	pushq	%rdx
	pushq	%rdi
	movq	-16(%rbp), %rax
	movq	%rax, %rdi
	call	strings.len
	popq	%rdi
	popq	%rdx
	movq	-8(%rbp), %rdx
	movl	%eax, %ebx
	movl	%ebx, 0(%rdx)
	movq	-8(%rbp), %rdx
	movq	-16(%rbp), %rbx
	movq	%rbx, 4(%rdx)
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
	movq	-8(%rbp), %rdx
	movq	-16(%rbp), %rbx
	movq	%rbx, 4(%rdx)
	movq	-8(%rbp), %rdx
	movl	$0, %ebx
	movl	%ebx, 0(%rdx)
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
	movq	-8(%rbp), %rdx
	movq	-16(%rbp), %rbx
	movq	%rbx, 4(%rdx)
	movq	-8(%rbp), %rdx
	movl	$0, %ebx
	movl	%ebx, 0(%rdx)
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
	movq	-8(%rbp), %rdx
	movq	%rax, %rbx
	movq	%rbx, 12(%rdx)
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
	movq	-8(%rbp), %rdx
	movq	%rax, %rbx
	movq	%rbx, 20(%rdx)
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
	pushq	%rdx
	call	sys_fork
	popq	%rdx
	movq	-8(%rbp), %rdx
	movl	%eax, %ebx
	movl	%ebx, 0(%rdx)
	movq	-8(%rbp), %r14
	movq	4(%r14), %rbx
	movq	%rbx, -24(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	$0, %edx
	movq	-8(%rbp), %r14
	movl	0(%r14), %edi
	cmpl	%edx, %edi
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.Lstart0
	pushq	%rdx
	movq	-24(%rbp), %r15
	pushq	%rdi
	movq	-16(%rbp), %rax
	movq	%rax, %rdi
	call	*%r15
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
	pushq	%rdx
	call	sys_fork
	popq	%rdx
	movq	-8(%rbp), %rdx
	movl	%eax, %ebx
	movl	%ebx, 0(%rdx)
	movq	-8(%rbp), %r14
	movq	4(%r14), %rbx
	movq	%rbx, -24(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	$0, %edx
	movq	-8(%rbp), %r14
	movl	0(%r14), %edi
	cmpl	%edx, %edi
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.Lstart1
	pushq	%rdx
	movq	-24(%rbp), %r15
	pushq	%rdi
	movq	-16(%rbp), %rax
	movq	%rax, %rdi
	movq	-8(%rbp), %r14
	pushq	%rsi
	movq	12(%r14), %rax
	movq	%rax, %rsi
	movq	-8(%rbp), %r14
	pushq	%rdx
	movq	20(%r14), %rax
	movq	%rax, %rdx
	call	*%r15
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
pub_Process_getPid:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$16, %rsp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %r14
	movl	0(%r14), %eax
	leave
	ret
pub_MProcess_getPid:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$16, %rsp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %r14
	movl	0(%r14), %eax
	leave
	ret
pub_Process_isRunning:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$16, %rsp
	movq	%rdi, -8(%rbp)
	pushq	%rdx
	movq	-8(%rbp), %r14
	pushq	%rdi
	movl	0(%r14), %eax
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
	movl	-12(%rbp), %edi
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
	pushq	%rsi
	movq	-16(%rbp), %rax
	movq	%rax, %rsi
	call	pub_Message_init
	popq	%rsi
	popq	%rdi
	popq	%rdx
	movq	%rax, %rbx
	movq	%rbx, -24(%rbp)
	movq	-8(%rbp), %r14
	movq	12(%r14), %rbx
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
	movq	-24(%rbp), %r14
	pushq	%rdi
	movl	0(%r14), %eax
	movl	%eax, %edi
	pushq	%rsi
	movq	-52(%rbp), %rax
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
	pushq	%rsi
	movq	-52(%rbp), %rax
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
	movq	-24(%rbp), %r14
	pushq	%rsi
	movq	4(%r14), %rax
	movq	%rax, %rsi
	movq	-24(%rbp), %r14
	pushq	%rdx
	movl	0(%r14), %eax
	movl	%eax, %edx
	call	pub_Pipe_write
	popq	%rdx
	popq	%rsi
	popq	%rdi
	popq	%rdx
	movl	%eax, %ebx
	movl	%ebx, -56(%rbp)
	pushq	%rdx
	pushq	%rdi
	movq	-52(%rbp), %rax
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
	movq	-8(%rbp), %r14
	movq	20(%r14), %rbx
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
	pushq	%rsi
	movq	-24(%rbp), %rax
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
	pushq	%rdi
	movq	-24(%rbp), %rax
	movq	%rax, %rdi
	call	strings.str_toInt
	popq	%rdi
	popq	%rdx
	movl	%eax, %ebx
	movl	%ebx, -28(%rbp)
	pushq	%rdx
	pushq	%rdi
	movq	-24(%rbp), %rax
	movq	%rax, %rdi
	call	free
	popq	%rdi
	popq	%rdx
	pushq	%rdx
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movl	-28(%rbp), %edi
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
	movl	-28(%rbp), %edx
	movq	-36(%rbp), %rdi
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
	pushq	%rsi
	movq	-36(%rbp), %rax
	movq	%rax, %rsi
	pushq	%rdx
	movl	-28(%rbp), %eax
	movl	%eax, %edx
	call	pub_Pipe_read
	popq	%rdx
	popq	%rsi
	popq	%rdi
	popq	%rdx
	movq	-36(%rbp), %rax
	leave
	ret
concurrancy.readPipe:
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
	pushq	%rsi
	movq	-16(%rbp), %rax
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
	pushq	%rdi
	movq	-16(%rbp), %rax
	movq	%rax, %rdi
	call	strings.str_toInt
	popq	%rdi
	popq	%rdx
	movl	%eax, %ebx
	movl	%ebx, -20(%rbp)
	pushq	%rdx
	pushq	%rdi
	movq	-16(%rbp), %rax
	movq	%rax, %rdi
	call	free
	popq	%rdi
	popq	%rdx
	pushq	%rdx
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movl	-20(%rbp), %edi
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
	pushq	%rsi
	movq	-28(%rbp), %rax
	movq	%rax, %rsi
	pushq	%rdx
	movl	-20(%rbp), %eax
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
	pushq	%rsi
	movq	-28(%rbp), %rax
	movq	%rax, %rsi
	call	pub_Message_init
	popq	%rsi
	popq	%rdi
	popq	%rdx
	movq	%rax, %rbx
	movq	%rbx, -36(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	-20(%rbp), %edx
	movq	-28(%rbp), %rdi
	add	%rdx, %rdi
	movq	%rdi, %rax
	popq	%rdx
	popq	%rdi
	movq	%rax, %rbx
	movq	%rbx, -28(%rbp)
	movq	-28(%rbp), %rax
	movb	$0, %bl
	movb	%bl, (%rax)
	movq	-36(%rbp), %rax
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
	movq	-8(%rbp), %r14
	pushq	%rdi
	movl	0(%r14), %eax
	movl	%eax, %edi
	pushq	%rsi
	movq	-24(%rbp), %rax
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
	pushq	%rsi
	movq	-24(%rbp), %rax
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
	movq	-8(%rbp), %r14
	pushq	%rsi
	movq	4(%r14), %rax
	movq	%rax, %rsi
	movq	-8(%rbp), %r14
	pushq	%rdx
	movl	0(%r14), %eax
	movl	%eax, %edx
	call	pub_Pipe_write
	popq	%rdx
	popq	%rsi
	popq	%rdi
	popq	%rdx
	movl	%eax, %ebx
	movl	%ebx, -28(%rbp)
	pushq	%rdx
	pushq	%rdi
	movq	-24(%rbp), %rax
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
	pushq	%rdi
	movq	-16(%rbp), %rax
	movq	%rax, %rdi
	call	sys_pipe
	popq	%rdi
	popq	%rdx
	movq	-16(%rbp), %r14
	movq	-8(%rbp), %rdx
	movl	0(%r14), %ebx
	movl	%ebx, 4(%rdx)
	movq	-16(%rbp), %r14
	movq	-8(%rbp), %rdx
	movl	4(%r14), %ebx
	movl	%ebx, 0(%rdx)
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
	movq	-8(%rbp), %r14
	movl	4(%r14), %ebx
	movl	%ebx, -24(%rbp)
	pushq	%rdx
	pushq	%rdi
	movl	-24(%rbp), %eax
	movl	%eax, %edi
	pushq	%rsi
	movq	-16(%rbp), %rax
	movq	%rax, %rsi
	pushq	%rdx
	movl	-20(%rbp), %eax
	movl	%eax, %edx
	call	sys_read
	popq	%rdx
	popq	%rsi
	popq	%rdi
	popq	%rdx
	movl	%eax, %ebx
	movl	%ebx, -28(%rbp)
	movl	-28(%rbp), %eax
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
	movq	-8(%rbp), %r14
	movl	0(%r14), %ebx
	movl	%ebx, -24(%rbp)
	pushq	%rdx
	pushq	%rdi
	movl	-24(%rbp), %eax
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
	movl	%ebx, -28(%rbp)
	movl	-28(%rbp), %eax
	leave
	ret
concurrancy.exit:
exit:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$16, %rsp
	movl	%edi, -4(%rbp)
	pushq	%rdx
	pushq	%rdi
	movl	-4(%rbp), %eax
	movl	%eax, %edi
	call	sys_exit
	popq	%rdi
	popq	%rdx
	movl	$0, %eax
	leave
	ret
concurrancy.wait:
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
	call	pub_Process_getPid
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

