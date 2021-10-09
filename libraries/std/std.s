.global brk

.text

brk:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$16, %rsp
	movq	%rdi, -8(rdp)
	movq    $0x0c, %eax
	leave
	ret