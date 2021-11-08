.global	wait
.global	exit
.global	newPipe
.global	pub_Pipe_write
.global	pub_Pipe_read
.global	pub_Pipe_init
.global	newProcess
.global	pub_Process_isRunning
.global	pub_Process_getPid
.global	pub_Process_start
.global	pub_Process_init


.text

pub_Process_init:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$32, %rsp
	movq	%rdi, -20(%rbp)
	movq	%rsi, -28(%rbp)
	movq	-20(%rbp), %rdx
	movq	-28(%rbp), %rbx
	movq	%rbx, 4(%rdx)
	movq	-20(%rbp), %rdx
	movl	$0, %ebx
	movl	%ebx, 0(%rdx)
	movl	$0, %eax
	leave
	ret
pub_Process_start:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$48, %rsp
	movq	%rdi, -20(%rbp)
	movq	%rsi, -28(%rbp)
	call	sys_fork
	movq	-20(%rbp), %rdx
	movl	%eax, %ebx
	movl	%ebx, 0(%rdx)
	movq	-20(%rbp), %rdx
	movq	4(%rdx), %rbx
	movq	%rbx, -36(%rbp)
	movq	-20(%rbp), %rdx
	movl	0(%rdx), %eax
	movl	$0, %ecx
	cmpl	%ecx, %eax
	jne	.Lstart0
	movq	-36(%rbp), %rcx
	movq	-28(%rbp), %rax
	movq	%rax, %rdi
	call	*%rcx
	movl	$0, %eax
	movl	%eax, %edi
	call	sys_exit
.Lstart0:
	movl	$0, %eax
	leave
	ret
pub_Process_getPid:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$32, %rsp
	movq	%rdi, -20(%rbp)
	movq	-20(%rbp), %rdx
	movl	0(%rdx), %eax
	leave
	ret
pub_Process_isRunning:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$32, %rsp
	movq	%rdi, -20(%rbp)
	movq	-20(%rbp), %rdx
	movl	0(%rdx), %eax
	movl	%eax, %edi
	movl	$0, %eax
	movl	%eax, %esi
	call	sys_kill
	movl	%eax, -24(%rbp)
	movl	-24(%rbp), %eax
	movl	$0, %ecx
	cmpl	%ecx, %eax
	jge	.LisRunning1
	movl	$0, %eax
	leave
	ret
.LisRunning1:
	movl	$1, %eax
	leave
	ret
newProcess:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$16, %rsp
	movq	%rdi, -8(%rbp)
	movl	$12, %eax
	movl	%eax, %edi
	call	malloc
	movq	%rax, -16(%rbp)
	movq	-16(%rbp), %rax
	movq	$0, %rcx
	cmpq	%rcx, %rax
	jne	.LnewProcess2
	movq	$0, %rax
	leave
	ret
.LnewProcess2:
	lea	-16(%rbp), %rax
	movq	(%rax), %rax
	movq	%rax, %rdi
	movq	-8(%rbp), %rax
	movq	%rax, %rsi
	call	pub_Process_init
	movq	-16(%rbp), %rax
	leave
	ret
pub_Pipe_init:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$32, %rsp
	movq	%rdi, -16(%rbp)
	lea	-16(%rbp), %rax
	movq	%rax, -24(%rbp)
	movq	-24(%rbp), %rax
	movq	%rax, %rdi
	call	sys_pipe
	movq	-24(%rbp), %rdx
	movq	-16(%rbp), %rdx
	movl	0(%rdx), %ebx
	movl	%ebx, 4(%rdx)
	movq	-24(%rbp), %rdx
	movq	-16(%rbp), %rdx
	movl	4(%rdx), %ebx
	movl	%ebx, 0(%rdx)
	movl	$0, %eax
	leave
	ret
pub_Pipe_read:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$48, %rsp
	movq	%rdi, -24(%rbp)
	movq	%rsi, -32(%rbp)
	movl	%edx, -36(%rbp)
	movq	-24(%rbp), %rdx
	movl	4(%rdx), %ebx
	movl	%ebx, -40(%rbp)
	movl	-40(%rbp), %eax
	movl	%eax, %edi
	movq	-32(%rbp), %rax
	movq	%rax, %rsi
	movl	-36(%rbp), %eax
	movl	%eax, %edx
	call	sys_read
	movb	%al, -44(%rbp)
	movl	-44(%rbp), %eax
	leave
	ret
pub_Pipe_write:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$48, %rsp
	movq	%rdi, -24(%rbp)
	movq	%rsi, -32(%rbp)
	movl	%edx, -36(%rbp)
	movq	-24(%rbp), %rdx
	movl	0(%rdx), %ebx
	movl	%ebx, -40(%rbp)
	movl	-40(%rbp), %eax
	movl	%eax, %edi
	movq	-32(%rbp), %rax
	movq	%rax, %rsi
	movl	-36(%rbp), %eax
	movl	%eax, %edx
	call	sys_write
	movl	%eax, -44(%rbp)
	movl	-44(%rbp), %eax
	leave
	ret
newPipe:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$16, %rsp
	movl	$16, %eax
	movl	%eax, %edi
	call	malloc
	movq	%rax, -8(%rbp)
	lea	-8(%rbp), %rax
	movq	(%rax), %rax
	movq	%rax, %rdi
	call	pub_Pipe_init
	movq	-8(%rbp), %rax
	leave
	ret
exit:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$16, %rsp
	movl	%edi, -4(%rbp)
	movl	-4(%rbp), %eax
	movl	%eax, %edi
	call	sys_exit
	movl	$0, %eax
	leave
	ret
wait:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$16, %rsp
	movq	%rdi, -8(%rbp)
	lea	-8(%rbp), %rax
	movq	(%rax), %rax
	movq	%rax, %rdi
	call	pub_Process_getPid
	movl	%eax, %eax
	movl	%eax, %esi
	movq	$0, %rax
	movq	%rax, %rdx
	movq	$0, %rax
	movq	%rax, %rcx
	movq	$0, %rax
	movq	%rax, %r8
	call	sys_wait
	movl	$0, %eax
	leave
	ret


.data



.bss

