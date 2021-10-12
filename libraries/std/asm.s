.global brk
.global sys_create
.global sys_write
.global sys_open

.text

brk:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$16, %rsp
	movq    $0x0c, %rax
	syscall
	leave
	ret

sys_write:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$16, %rsp
	movq    $0x01, %rax
	syscall
	leave
	ret

sys_create:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$16, %rsp
	movq    $0x55, %rax
	syscall
	leave
	ret

sys_open:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$16, %rsp
	movq    $0x02, %rax
	syscall
	leave
	ret