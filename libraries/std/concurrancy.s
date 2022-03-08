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
	subq	$16, %rsp
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	movq	-8(%rbp), %rdx
	movq	-16(%rbp), %rbx
	movq	%rbx, 4(%rdx)
	movq	-8(%rbp), %rdx
	movl	$0, %ebx
	movl	%ebx, 0(%rdx)
	movl	$0, %eax
	leave
	ret
	movq	-8(%rbp), %rax
	leave
	ret
pub_Process_start:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$32, %rsp
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	call	sys_fork
	movq	-8(%rbp), %rdx
	movl	%eax, %ebx
	movl	%ebx, 0(%rdx)
	movq	-8(%rbp), %rdx
	movq	4(%rdx), %rbx
	movq	%rbx, -24(%rbp)
	movq	-8(%rbp), %rdx
	movl	0(%rdx), %eax
	movl	$0, %ecx
	cmpl	%ecx, %eax
	jne	.Lstart0
	movq	-24(%rbp), %r15
	pushq	%rdi
	movq	-16(%rbp), %rax
	movq	%rax, %rdi
	call	*%r15
	popq	%rdi
	pushq	%rdi
	movl	$0, %eax
	movl	%eax, %edi
	call	sys_exit
	popq	%rdi
.Lstart0:
	movl	$0, %eax
	leave
	ret
	leave
	ret
pub_Process_getPid:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$16, %rsp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rdx
	movl	0(%rdx), %eax
	leave
	ret
	leave
	ret
pub_Process_isRunning:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$16, %rsp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rdx
	pushq	%rdi
	movl	0(%rdx), %eax
	movl	%eax, %edi
	pushq	%rsi
	movl	$0, %eax
	movl	%eax, %esi
	call	sys_kill
	popq	%rsi
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -12(%rbp)
	movl	-12(%rbp), %eax
	movl	$0, %ecx
	cmpl	%ecx, %eax
	jge	.LisRunning3
	movl	$0, %eax
	leave
	ret
.LisRunning3:
	movl	$1, %eax
	leave
	ret
	leave
	ret
newProcess:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$16, %rsp
	movq	%rdi, -8(%rbp)
	pushq	%rdi
	movl	$12, %eax
	movl	%eax, %edi
	call	malloc
	popq	%rdi
	movq	%rax, %rbx
	movq	%rbx, -16(%rbp)
	movq	-16(%rbp), %rax
	movq	$0, %rcx
	cmpq	%rcx, %rax
	jne	.LnewProcess6
	movq	$0, %rax
	leave
	ret
.LnewProcess6:
	lea	-16(%rbp), %rax
	pushq	%rdi
	movq	(%rax), %rax
	movq	%rax, %rdi
	pushq	%rsi
	movq	-8(%rbp), %rax
	movq	%rax, %rsi
	call	pub_Process_init
	popq	%rsi
	popq	%rdi
	movq	-16(%rbp), %rax
	leave
	ret
	leave
	ret
pub_Pipe_init:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$32, %rsp
	movq	%rdi, -8(%rbp)
	lea	-16(%rbp), %rax
	movq	%rax, %rbx
	movq	%rbx, -24(%rbp)
	pushq	%rdi
	movq	-24(%rbp), %rax
	movq	%rax, %rdi
	call	sys_pipe
	popq	%rdi
	movq	-24(%rbp), %rdx
	movq	-8(%rbp), %rdx
	movl	0(%rdx), %ebx
	movl	%ebx, 4(%rdx)
	movq	-24(%rbp), %rdx
	movq	-8(%rbp), %rdx
	movl	4(%rdx), %ebx
	movl	%ebx, 0(%rdx)
	movl	$0, %eax
	leave
	ret
	movq	-8(%rbp), %rax
	leave
	ret
pub_Pipe_read:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$32, %rsp
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	movl	%edx, -20(%rbp)
	movq	-8(%rbp), %rdx
	movl	4(%rdx), %ebx
	movl	%ebx, -24(%rbp)
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
	movl	%eax, %ebx
	movl	%ebx, -28(%rbp)
	movl	-28(%rbp), %eax
	leave
	ret
	leave
	ret
pub_Pipe_write:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$32, %rsp
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	movl	%edx, -20(%rbp)
	movq	-8(%rbp), %rdx
	movl	0(%rdx), %ebx
	movl	%ebx, -24(%rbp)
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
	movl	%eax, %ebx
	movl	%ebx, -28(%rbp)
	movl	-28(%rbp), %eax
	leave
	ret
	leave
	ret
newPipe:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$16, %rsp
	pushq	%rdi
	movl	$8, %eax
	movl	%eax, %edi
	call	malloc
	popq	%rdi
	movq	%rax, %rbx
	movq	%rbx, -8(%rbp)
	lea	-8(%rbp), %rax
	pushq	%rdi
	movq	(%rax), %rax
	movq	%rax, %rdi
	call	pub_Pipe_init
	popq	%rdi
	movq	-8(%rbp), %rax
	leave
	ret
	leave
	ret
exit:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$16, %rsp
	movl	%edi, -4(%rbp)
	pushq	%rdi
	movl	-4(%rbp), %eax
	movl	%eax, %edi
	call	sys_exit
	popq	%rdi
	movl	$0, %eax
	leave
	ret
	leave
	ret
wait:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$16, %rsp
	movq	%rdi, -8(%rbp)
	lea	-8(%rbp), %rax
	pushq	%rdi
	movq	(%rax), %rax
	movq	%rax, %rdi
	call	pub_Process_getPid
	popq	%rdi
	pushq	%rdi
	movl	%eax, %eax
	movl	%eax, %edi
	pushq	%rsi
	movq	$0, %rax
	movq	%rax, %rsi
	pushq	%rdx
	movq	$0, %rax
	movq	%rax, %rdx
	pushq	%rcx
	movq	$0, %rax
	movq	%rax, %rcx
	call	sys_wait
	popq	%rcx
	popq	%rdx
	popq	%rsi
	popq	%rdi
	movl	$0, %eax
	leave
	ret
	leave
	ret


.data



.bss

