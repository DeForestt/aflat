.global brk

.text

brk:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$16, %rsp
	movq    $0x0c, %rax
	leave
	ret
    