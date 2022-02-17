.global	str_forEach
.global	str_at
.global	str_reverse
.global	str_subString
.global	float_toString
.global	sPrintFloat
.global	str_toFloat
.global	charToFloat
.global	str_split
.global	str_find
.global	int_toString
.global	str_toInt
.global	str_concat
.global	str_comp
.global	len
.global	ascii


.text

ascii:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$12, %rsp
	movl	%edi, -4(%rbp)
	lea	-4(%rbp), %rax
	movq	%rax, -12(%rbp)
	movq	-12(%rbp), %rax
	movb	(%rax), %al
	movb	%al, %al
	leave
	ret
len:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$25, %rsp
	movq	%rdi, -20(%rbp)
	movl	$0, %eax
	movl	%eax, %edi
	call	ascii
	movb	%al, -21(%rbp)
	movl	$0, -25(%rbp)
	jmp	.Llen1
.Llen0:
	mov	$1, %edx
	mov	-25(%rbp), %eax
	add	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -25(%rbp)
	mov	$1, %edx
	mov	-20(%rbp), %rax
	add	%rdx, %rax
	movl	%eax, %ebx
	movl	%ebx, -20(%rbp)
.Llen1:
	movq	-20(%rbp), %rax
	movb	(%rax), %al
	movb	%al, %al
	movb	-21(%rbp), %cl
	cmpb	%cl, %al
	jne	.Llen0
	movl	-25(%rbp), %eax
	leave
	ret
str_comp:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$55, %rsp
	movq	%rdi, -33(%rbp)
	movq	%rsi, -41(%rbp)
	movq	-33(%rbp), %rax
	movq	%rax, %rdi
	call	len
	movl	%eax, -45(%rbp)
	movq	-41(%rbp), %rax
	movq	%rax, %rdi
	call	len
	movl	%eax, -49(%rbp)
	movl	-45(%rbp), %eax
	movl	-49(%rbp), %ecx
	cmpl	%ecx, %eax
	je	.Lstr_comp2
	movl	$0, %eax
	leave
	ret
.Lstr_comp2:
	movl	$0, -53(%rbp)
	jmp	.Lstr_comp6
.Lstr_comp5:
	mov	$1, %edx
	mov	-53(%rbp), %eax
	add	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -53(%rbp)
	movq	-33(%rbp), %rax
	movb	(%rax), %al
	movb	%al, -54(%rbp)
	movq	-41(%rbp), %rax
	movb	(%rax), %al
	movb	%al, -55(%rbp)
	movb	-54(%rbp), %al
	movb	-55(%rbp), %cl
	cmpb	%cl, %al
	je	.Lstr_comp7
	movl	$0, %eax
	leave
	ret
.Lstr_comp7:
	mov	$1, %edx
	mov	-41(%rbp), %rax
	add	%rdx, %rax
	movl	%eax, %ebx
	movl	%ebx, -41(%rbp)
	mov	$1, %edx
	mov	-33(%rbp), %rax
	add	%rdx, %rax
	movl	%eax, %ebx
	movl	%ebx, -33(%rbp)
.Lstr_comp6:
	movl	-53(%rbp), %eax
	movl	-45(%rbp), %ecx
	cmpl	%ecx, %eax
	jl	.Lstr_comp5
	movl	$1, %eax
	leave
	ret
str_concat:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$81, %rsp
	movq	%rdi, -63(%rbp)
	movq	%rsi, -71(%rbp)
	movq	%rdx, -79(%rbp)
	movl	$0, %eax
	movl	%eax, %edi
	call	ascii
	movb	%al, -80(%rbp)
	jmp	.Lstr_concat11
.Lstr_concat10:
	movq	-63(%rbp), %rax
	movb	(%rax), %al
	movb	%al, %bl
	movb	%bl, -81(%rbp)
	movq	-79(%rbp), %rax
	movb	-81(%rbp), %bl
	movb	%bl, (%rax)
	mov	$1, %edx
	mov	-63(%rbp), %rax
	add	%rdx, %rax
	movl	%eax, %ebx
	movl	%ebx, -63(%rbp)
	mov	$1, %edx
	mov	-79(%rbp), %rax
	add	%rdx, %rax
	movl	%eax, %ebx
	movl	%ebx, -79(%rbp)
.Lstr_concat11:
	movq	-63(%rbp), %rax
	movb	(%rax), %al
	movb	%al, %al
	movb	-80(%rbp), %cl
	cmpb	%cl, %al
	jne	.Lstr_concat10
	jmp	.Lstr_concat13
.Lstr_concat12:
	movq	-71(%rbp), %rax
	movb	(%rax), %al
	movb	%al, %bl
	movb	%bl, -81(%rbp)
	movq	-79(%rbp), %rax
	movb	-81(%rbp), %bl
	movb	%bl, (%rax)
	mov	$1, %edx
	mov	-71(%rbp), %rax
	add	%rdx, %rax
	movl	%eax, %ebx
	movl	%ebx, -71(%rbp)
	mov	$1, %edx
	mov	-79(%rbp), %rax
	add	%rdx, %rax
	movl	%eax, %ebx
	movl	%ebx, -79(%rbp)
.Lstr_concat13:
	movq	-71(%rbp), %rax
	movb	(%rax), %al
	movb	%al, %al
	movb	-80(%rbp), %cl
	cmpb	%cl, %al
	jne	.Lstr_concat12
	movq	-79(%rbp), %rax
	movb	-80(%rbp), %bl
	movb	%bl, (%rax)
	movl	$0, %eax
	leave
	ret
str_toInt:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$116, %rsp
	movq	%rdi, -89(%rbp)
	movl	$1, -93(%rbp)
	movq	-89(%rbp), %rax
	movb	(%rax), %al
	movb	%al, -94(%rbp)
	movb	-94(%rbp), %al
	movb	$45, %cl
	cmpb	%cl, %al
	jne	.Lstr_toInt14
	movl	$-1, %ebx
	movl	%ebx, -93(%rbp)
	mov	$1, %rdx
	mov	-89(%rbp), %rax
	add	%rdx, %rax
	movl	%eax, %ebx
	movl	%ebx, -89(%rbp)
.Lstr_toInt14:
	movl	$0, %eax
	movl	%eax, %edi
	call	ascii
	movb	%al, -95(%rbp)
	movl	$0, -99(%rbp)
	movq	-89(%rbp), %rax
	movq	%rax, %rdi
	call	len
	movl	%eax, -103(%rbp)
	movl	$0, -107(%rbp)
	mov	$1, %edx
	mov	-103(%rbp), %eax
	sub	%edx, %eax
	movl	%eax, -111(%rbp)
	jmp	.Lstr_toInt18
.Lstr_toInt17:
	movl	$10, %eax
	movl	%eax, %edi
	movl	-111(%rbp), %eax
	movl	%eax, %esi
	call	exp
	movl	%eax, %ebx
	movl	%ebx, -99(%rbp)
	movq	-89(%rbp), %rax
	movb	(%rax), %al
	movb	%al, -112(%rbp)
	movb	-112(%rbp), %al
	movb	%al, %dil
	call	toInt
	movl	%eax, -116(%rbp)
	mov	-116(%rbp), %edx
	mov	-99(%rbp), %eax
	imul	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -99(%rbp)
	mov	-99(%rbp), %edx
	mov	-107(%rbp), %eax
	add	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -107(%rbp)
	mov	$1, %edx
	mov	-111(%rbp), %eax
	sub	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -111(%rbp)
	mov	$1, %edx
	mov	-89(%rbp), %rax
	add	%rdx, %rax
	movl	%eax, %ebx
	movl	%ebx, -89(%rbp)
.Lstr_toInt18:
	movl	-111(%rbp), %eax
	movl	$-1, %ecx
	cmpl	%ecx, %eax
	jg	.Lstr_toInt17
	mov	-93(%rbp), %edx
	mov	-107(%rbp), %eax
	imul	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -107(%rbp)
	movl	-107(%rbp), %eax
	leave
	ret
int_toString:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$146, %rsp
	movl	%edi, -120(%rbp)
	movq	%rsi, -128(%rbp)
	movl	$0, -132(%rbp)
	movl	$10, -136(%rbp)
	movl	$0, -140(%rbp)
	movl	$0, -144(%rbp)
	movl	$0, %eax
	movl	%eax, %edi
	call	ascii
	movb	%al, -145(%rbp)
	movl	-120(%rbp), %eax
	movl	$0, %ecx
	cmpl	%ecx, %eax
	jne	.Lint_toString19
	movq	-128(%rbp), %rax
	movb	$48, %bl
	movb	%bl, (%rax)
	movq	-128(%rbp), %rax
	movb	-145(%rbp), %bl
	movb	%bl, (%rax)
	mov	$1, %rdx
	mov	-128(%rbp), %rax
	add	%rdx, %rax
	movl	%eax, %ebx
	movl	%ebx, -128(%rbp)
	movl	$0, %eax
	leave
	ret
.Lint_toString19:
	movl	-120(%rbp), %eax
	movl	$0, %ecx
	cmpl	%ecx, %eax
	jge	.Lint_toString22
	movq	-128(%rbp), %rax
	movb	$45, %bl
	movb	%bl, (%rax)
	mov	$1, %rdx
	mov	-128(%rbp), %rax
	add	%rdx, %rax
	movl	%eax, %ebx
	movl	%ebx, -128(%rbp)
	mov	$-1, %edx
	mov	-120(%rbp), %eax
	imul	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -120(%rbp)
.Lint_toString22:
	mov	$1, %edx
	mov	-120(%rbp), %eax
	imul	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -132(%rbp)
	jmp	.Lint_toString26
.Lint_toString25:
	mov	-132(%rbp), %eax
	cltd
	idivl	-136(%rbp)
	movl	%eax, %ebx
	movl	%ebx, -132(%rbp)
	mov	$1, %edx
	mov	-144(%rbp), %eax
	add	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -144(%rbp)
.Lint_toString26:
	movl	-132(%rbp), %eax
	movl	$0, %ecx
	cmpl	%ecx, %eax
	jne	.Lint_toString25
	mov	$1, %edx
	mov	-144(%rbp), %eax
	sub	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -144(%rbp)
	movb	$32, -146(%rbp)
	jmp	.Lint_toString28
.Lint_toString27:
	movl	$10, %eax
	movl	%eax, %edi
	movl	-144(%rbp), %eax
	movl	%eax, %esi
	call	exp
	movl	%eax, %ebx
	movl	%ebx, -140(%rbp)
	mov	-120(%rbp), %eax
	cltd
	idivl	-140(%rbp)
	movl	%eax, %ebx
	movl	%ebx, -132(%rbp)
	movl	-132(%rbp), %eax
	movl	%eax, %edi
	call	toChar
	movb	%al, %bl
	movb	%bl, -146(%rbp)
	movq	-128(%rbp), %rax
	movb	-146(%rbp), %bl
	movb	%bl, (%rax)
	mov	$1, %rdx
	mov	-128(%rbp), %rax
	add	%rdx, %rax
	movl	%eax, %ebx
	movl	%ebx, -128(%rbp)
	mov	-140(%rbp), %edx
	mov	-132(%rbp), %eax
	imul	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -132(%rbp)
	mov	-132(%rbp), %edx
	mov	-120(%rbp), %eax
	sub	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -120(%rbp)
	mov	$1, %edx
	mov	-144(%rbp), %eax
	sub	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -144(%rbp)
.Lint_toString28:
	movl	-144(%rbp), %eax
	movl	$-1, %ecx
	cmpl	%ecx, %eax
	jg	.Lint_toString27
	movq	-128(%rbp), %rax
	movb	-145(%rbp), %bl
	movb	%bl, (%rax)
	movl	$0, %eax
	leave
	ret
str_find:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$164, %rsp
	movq	%rdi, -154(%rbp)
	movb	%sil, -155(%rbp)
	movl	$0, -159(%rbp)
	movq	-154(%rbp), %rax
	movq	%rax, %rdi
	call	len
	movl	%eax, -163(%rbp)
	jmp	.Lstr_find30
.Lstr_find29:
	mov	$1, %edx
	mov	-154(%rbp), %rax
	add	%rdx, %rax
	movl	%eax, %ebx
	movl	%ebx, -154(%rbp)
	movq	-154(%rbp), %rax
	movb	(%rax), %al
	movb	%al, -164(%rbp)
	movb	-164(%rbp), %al
	movb	-155(%rbp), %cl
	cmpb	%cl, %al
	jne	.Lstr_find31
	movl	-159(%rbp), %eax
	leave
	ret
.Lstr_find31:
	mov	$1, %edx
	mov	-159(%rbp), %eax
	add	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -159(%rbp)
.Lstr_find30:
	movl	-159(%rbp), %eax
	movl	-163(%rbp), %ecx
	cmpl	%ecx, %eax
	jl	.Lstr_find29
	movl	$-1, %eax
	leave
	ret
str_split:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$191, %rsp
	movq	%rdi, -172(%rbp)
	movb	%sil, -173(%rbp)
	movq	%rdx, -181(%rbp)
	movq	%rcx, -189(%rbp)
	movl	$0, %eax
	movl	%eax, %edi
	call	ascii
	movb	%al, -190(%rbp)
	movq	-172(%rbp), %rax
	movb	(%rax), %al
	movb	%al, -191(%rbp)
	jmp	.Lstr_split35
.Lstr_split34:
	movb	-191(%rbp), %al
	movb	-190(%rbp), %cl
	cmpb	%cl, %al
	jne	.Lstr_split36
	movl	$-1, %eax
	leave
	ret
.Lstr_split36:
	movb	-191(%rbp), %al
	movb	-173(%rbp), %cl
	cmpb	%cl, %al
	je	.Lstr_split39
	movq	-181(%rbp), %rax
	movb	-191(%rbp), %bl
	movb	%bl, (%rax)
.Lstr_split39:
	mov	$1, %edx
	mov	-172(%rbp), %rax
	add	%rdx, %rax
	movl	%eax, %ebx
	movl	%ebx, -172(%rbp)
	mov	$1, %edx
	mov	-181(%rbp), %rax
	add	%rdx, %rax
	movl	%eax, %ebx
	movl	%ebx, -181(%rbp)
	movq	-172(%rbp), %rax
	movb	(%rax), %al
	movb	%al, %bl
	movb	%bl, -191(%rbp)
.Lstr_split35:
	movb	-191(%rbp), %al
	movb	-173(%rbp), %cl
	cmpb	%cl, %al
	jne	.Lstr_split34
	movq	-181(%rbp), %rax
	movb	-190(%rbp), %bl
	movb	%bl, (%rax)
	mov	$1, %edx
	mov	-172(%rbp), %rax
	add	%rdx, %rax
	movl	%eax, %ebx
	movl	%ebx, -172(%rbp)
	movq	-172(%rbp), %rax
	movb	(%rax), %al
	movb	%al, %al
	movb	-190(%rbp), %cl
	cmpb	%cl, %al
	jne	.Lstr_split42
	movl	$-1, %eax
	leave
	ret
.Lstr_split42:
	movq	$.strstr_split45, %rax
	movq	%rax, %rdi
	movq	-172(%rbp), %rax
	movq	%rax, %rsi
	movq	-189(%rbp), %rax
	movq	%rax, %rdx
	call	str_concat
	movl	$0, %eax
	leave
	ret
charToFloat:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$196, %rsp
	movb	%dil, -192(%rbp)
	movss	.floatcharToFloat46, %xmm0
	movss	%xmm0, -196(%rbp)
	movb	-192(%rbp), %al
	movb	$48, %cl
	cmpb	%cl, %al
	jne	.LcharToFloat47
	movss	.floatcharToFloat50, %xmm0
	movss	%xmm0, %xmm0
	movss	%xmm0, -196(%rbp)
.LcharToFloat47:
	movb	-192(%rbp), %al
	movb	$49, %cl
	cmpb	%cl, %al
	jne	.LcharToFloat51
	movss	.floatcharToFloat54, %xmm0
	movss	%xmm0, %xmm0
	movss	%xmm0, -196(%rbp)
.LcharToFloat51:
	movb	-192(%rbp), %al
	movb	$50, %cl
	cmpb	%cl, %al
	jne	.LcharToFloat55
	movss	.floatcharToFloat58, %xmm0
	movss	%xmm0, %xmm0
	movss	%xmm0, -196(%rbp)
.LcharToFloat55:
	movb	-192(%rbp), %al
	movb	$51, %cl
	cmpb	%cl, %al
	jne	.LcharToFloat59
	movss	.floatcharToFloat62, %xmm0
	movss	%xmm0, %xmm0
	movss	%xmm0, -196(%rbp)
.LcharToFloat59:
	movb	-192(%rbp), %al
	movb	$52, %cl
	cmpb	%cl, %al
	jne	.LcharToFloat63
	movss	.floatcharToFloat66, %xmm0
	movss	%xmm0, %xmm0
	movss	%xmm0, -196(%rbp)
.LcharToFloat63:
	movb	-192(%rbp), %al
	movb	$53, %cl
	cmpb	%cl, %al
	jne	.LcharToFloat67
	movss	.floatcharToFloat70, %xmm0
	movss	%xmm0, %xmm0
	movss	%xmm0, -196(%rbp)
.LcharToFloat67:
	movb	-192(%rbp), %al
	movb	$54, %cl
	cmpb	%cl, %al
	jne	.LcharToFloat71
	movss	.floatcharToFloat74, %xmm0
	movss	%xmm0, %xmm0
	movss	%xmm0, -196(%rbp)
.LcharToFloat71:
	movb	-192(%rbp), %al
	movb	$55, %cl
	cmpb	%cl, %al
	jne	.LcharToFloat75
	movss	.floatcharToFloat78, %xmm0
	movss	%xmm0, %xmm0
	movss	%xmm0, -196(%rbp)
.LcharToFloat75:
	movb	-192(%rbp), %al
	movb	$56, %cl
	cmpb	%cl, %al
	jne	.LcharToFloat79
	movss	.floatcharToFloat82, %xmm0
	movss	%xmm0, %xmm0
	movss	%xmm0, -196(%rbp)
.LcharToFloat79:
	movb	-192(%rbp), %al
	movb	$57, %cl
	cmpb	%cl, %al
	jne	.LcharToFloat83
	movss	.floatcharToFloat86, %xmm0
	movss	%xmm0, %xmm0
	movss	%xmm0, -196(%rbp)
.LcharToFloat83:
	movl	-196(%rbp), %eax
	leave
	ret
str_toFloat:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$290, %rsp
	movq	%rdi, -204(%rbp)
	movq	-204(%rbp), %rax
	movq	%rax, %rdi
	movb	$46, %al
	movb	%al, %sil
	lea	-224(%rbp), %rax
	movq	%rax, %rax
	movq	%rax, %rdx
	lea	-244(%rbp), %rax
	movq	%rax, %rax
	movq	%rax, %rcx
	call	str_split
	lea	-224(%rbp), %rax
	movq	%rax, %rax
	movq	%rax, %rdi
	call	len
	movl	%eax, -248(%rbp)
	mov	$1, %edx
	mov	-248(%rbp), %eax
	sub	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -248(%rbp)
	movss	.floatstr_toFloat87, %xmm0
	movss	%xmm0, -252(%rbp)
	movl	$0, -256(%rbp)
	movss	.floatstr_toFloat88, %xmm0
	movss	%xmm0, -260(%rbp)
	jmp	.Lstr_toFloat90
.Lstr_toFloat89:
	movss	.floatstr_toFloat92, %xmm0
	movss	%xmm0, %xmm1
	movss	-260(%rbp), %xmm0
	mulss	%xmm1, %xmm0
	movss	%xmm0, %xmm0
	movss	%xmm0, -260(%rbp)
	mov	$1, %edx
	mov	-256(%rbp), %eax
	add	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -256(%rbp)
.Lstr_toFloat90:
	movl	-256(%rbp), %eax
	movl	-248(%rbp), %ecx
	cmpl	%ecx, %eax
	jl	.Lstr_toFloat89
	movl	$0, %ebx
	movl	%ebx, -256(%rbp)
	movss	.floatstr_toFloat93, %xmm0
	movss	%xmm0, -264(%rbp)
	movl	$0, %eax
	movl	%eax, %edi
	call	ascii
	movb	%al, -265(%rbp)
	movq	-204(%rbp), %rax
	movb	(%rax), %al
	movb	%al, -266(%rbp)
	movss	.floatstr_toFloat94, %xmm0
	movss	%xmm0, -270(%rbp)
	jmp	.Lstr_toFloat96
.Lstr_toFloat95:
	movb	-266(%rbp), %al
	movb	$46, %cl
	cmpb	%cl, %al
	jne	.Lstr_toFloat97
	movss	.floatstr_toFloat100, %xmm0
	movss	%xmm0, %xmm0
	movss	%xmm0, -260(%rbp)
.Lstr_toFloat97:
	movb	-266(%rbp), %al
	movb	$46, %cl
	cmpb	%cl, %al
	je	.Lstr_toFloat101
	movb	-266(%rbp), %al
	movb	%al, %dil
	call	charToFloat
	movl	%eax, %ebx
	movl	%ebx, -264(%rbp)
	movss	.floatstr_toFloat105, %xmm0
	movss	%xmm0, %xmm1
	movss	-252(%rbp), %xmm0
	addss	%xmm1, %xmm0
	movss	%xmm0, -274(%rbp)
	movss	-260(%rbp), %xmm1
	movss	-264(%rbp), %xmm0
	mulss	%xmm1, %xmm0
	movss	%xmm0, %xmm0
	movss	%xmm0, -252(%rbp)
	movss	-274(%rbp), %xmm1
	movss	-252(%rbp), %xmm0
	addss	%xmm1, %xmm0
	movss	%xmm0, %xmm0
	movss	%xmm0, -252(%rbp)
	movss	-270(%rbp), %xmm1
	movss	-260(%rbp), %xmm0
	divss	%xmm1, %xmm0
	movss	%xmm0, %xmm0
	movss	%xmm0, -260(%rbp)
.Lstr_toFloat101:
	mov	$1, %edx
	mov	-204(%rbp), %rax
	add	%rdx, %rax
	movl	%eax, %ebx
	movl	%ebx, -204(%rbp)
	movq	-204(%rbp), %rax
	movb	(%rax), %al
	movb	%al, %bl
	movb	%bl, -266(%rbp)
	mov	$1, %edx
	mov	-256(%rbp), %eax
	add	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -256(%rbp)
.Lstr_toFloat96:
	movb	-266(%rbp), %al
	movb	-265(%rbp), %cl
	cmpb	%cl, %al
	jne	.Lstr_toFloat95
	lea	-252(%rbp), %rax
	movq	%rax, -282(%rbp)
	movss	.floatstr_toFloat106, %xmm0
	movss	%xmm0, -286(%rbp)
	movss	-286(%rbp), %xmm1
	movss	-286(%rbp), %xmm0
	mulss	%xmm1, %xmm0
	movss	%xmm0, %xmm0
	movss	%xmm0, -286(%rbp)
	movq	-282(%rbp), %rax
	movl	(%rax), %eax
	movl	%eax, -290(%rbp)
	movl	-290(%rbp), %eax
	leave
	ret
sPrintFloat:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$298, %rsp
	movq	%rdi, -298(%rbp)
	movq	-298(%rbp), %rax
	movq	%rax, %rdi
	call	printFloat
	movl	$0, %eax
	leave
	ret
float_toString:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$416, %rsp
	movq	%rdi, -306(%rbp)
	movq	%rsi, -314(%rbp)
	movl	%edx, -318(%rbp)
	movq	-306(%rbp), %rax
	movq	$0, %rcx
	cmpq	%rcx, %rax
	jne	.Lfloat_toString107
	movl	$-1, %eax
	leave
	ret
.Lfloat_toString107:
	movl	$6, -322(%rbp)
	movq	$.strfloat_toString110, -330(%rbp)
	movq	-306(%rbp), %rax
	movl	(%rax), %eax
	movl	%eax, -334(%rbp)
	movl	-334(%rbp), %eax
	movl	$0, %ecx
	cmpl	%ecx, %eax
	jne	.Lfloat_toString111
	movl	$0, %eax
	movl	%eax, %edi
	call	printInt
	movl	$0, %eax
	leave
	ret
.Lfloat_toString111:
	mov	$31, %edx
	mov	-334(%rbp), %eax
	movl	%edx, %ecx
	sarl	%cl, %eax
	movl	%eax, -338(%rbp)
	mov	$23, %edx
	mov	-334(%rbp), %eax
	movl	%edx, %ecx
	sarl	%cl, %eax
	movl	%eax, -342(%rbp)
	mov	$255, %edx
	mov	-342(%rbp), %eax
	andl	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -342(%rbp)
	mov	$127, %edx
	mov	-342(%rbp), %eax
	sub	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -342(%rbp)
	mov	$8388607, %edx
	mov	-334(%rbp), %eax
	andl	%edx, %eax
	movl	%eax, -346(%rbp)
	movl	$0, %eax
	movl	%eax, %edi
	call	newBit
	movq	%rax, -354(%rbp)
	movq	-354(%rbp), %rbx
	movq	%rbx, -362(%rbp)
	movl	$0, -366(%rbp)
	jmp	.Lfloat_toString115
.Lfloat_toString114:
	mov	$1, %edx
	mov	-346(%rbp), %eax
	andl	%edx, %eax
	movl	%eax, %eax
	movl	%eax, %edi
	call	newBit
	movq	%rax, -374(%rbp)
	movq	-362(%rbp), %rdx
	movq	-374(%rbp), %rbx
	movq	%rbx, 12(%rdx)
	movq	-374(%rbp), %rdx
	movq	-362(%rbp), %rbx
	movq	%rbx, 4(%rdx)
	movq	-374(%rbp), %rbx
	movq	%rbx, -362(%rbp)
	mov	$1, %edx
	mov	-346(%rbp), %eax
	movl	%edx, %ecx
	sarl	%cl, %eax
	movl	%eax, %ebx
	movl	%ebx, -346(%rbp)
	mov	$1, %edx
	mov	-366(%rbp), %eax
	add	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -366(%rbp)
.Lfloat_toString115:
	movl	-366(%rbp), %eax
	movl	$23, %ecx
	cmpl	%ecx, %eax
	jl	.Lfloat_toString114
	movl	$1, %eax
	movl	%eax, %edi
	call	newBit
	movq	%rax, -382(%rbp)
	movq	-362(%rbp), %rdx
	movq	-382(%rbp), %rbx
	movq	%rbx, 12(%rdx)
	movq	-382(%rbp), %rdx
	movq	-362(%rbp), %rbx
	movq	%rbx, 4(%rdx)
	movq	-382(%rbp), %rbx
	movq	%rbx, -362(%rbp)
	movq	-354(%rbp), %rbx
	movq	%rbx, -362(%rbp)
	jmp	.Lfloat_toString117
.Lfloat_toString116:
	movq	-362(%rbp), %rdx
	movq	12(%rdx), %rbx
	movq	%rbx, -362(%rbp)
.Lfloat_toString117:
	movq	-362(%rbp), %rdx
	movq	12(%rdx), %rax
	movq	$0, %rcx
	cmpq	%rcx, %rax
	jne	.Lfloat_toString116
	movl	-338(%rbp), %eax
	movl	$0, %ecx
	cmpl	%ecx, %eax
	je	.Lfloat_toString118
	movq	$.strfloat_toString121, %rbx
	movq	%rbx, -330(%rbp)
.Lfloat_toString118:
	movl	-318(%rbp), %eax
	movl	$1, %ecx
	cmpl	%ecx, %eax
	jne	.Lfloat_toString122
	movl	-338(%rbp), %eax
	movl	$0, %ecx
	cmpl	%ecx, %eax
	jne	.Lfloat_toString125
	movq	$.strfloat_toString128, %rbx
	movq	%rbx, -330(%rbp)
.Lfloat_toString125:
.Lfloat_toString122:
	movl	-342(%rbp), %ebx
	movl	%ebx, -386(%rbp)
	movl	$0, -394(%rbp)
	jmp	.Lfloat_toString130
.Lfloat_toString129:
	movq	-362(%rbp), %rdx
	movl	0(%rdx), %eax
	movl	$1, %ecx
	cmpl	%ecx, %eax
	jne	.Lfloat_toString131
	movl	$2, %eax
	movl	%eax, %edi
	movl	-386(%rbp), %eax
	movl	%eax, %esi
	call	exp
	movl	%eax, -398(%rbp)
	mov	-398(%rbp), %edx
	mov	-390(%rbp), %eax
	add	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -390(%rbp)
.Lfloat_toString131:
	movq	-362(%rbp), %rdx
	movq	4(%rdx), %rbx
	movq	%rbx, -362(%rbp)
	mov	$1, %edx
	mov	-386(%rbp), %eax
	sub	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -386(%rbp)
	mov	$1, %edx
	mov	-394(%rbp), %eax
	add	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -394(%rbp)
.Lfloat_toString130:
	movl	-386(%rbp), %eax
	movl	$-1, %ecx
	cmpl	%ecx, %eax
	jg	.Lfloat_toString129
	movl	$10, %eax
	movl	%eax, %edi
	movl	-322(%rbp), %eax
	movl	%eax, %esi
	call	exp
	movl	%eax, %ebx
	movl	%ebx, -386(%rbp)
	mov	$5, %edx
	mov	-386(%rbp), %eax
	imul	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -386(%rbp)
	movl	$2, -402(%rbp)
	movl	$0, -406(%rbp)
	jmp	.Lfloat_toString135
.Lfloat_toString134:
	movq	-362(%rbp), %rdx
	movl	0(%rdx), %eax
	movl	$1, %ecx
	cmpl	%ecx, %eax
	jne	.Lfloat_toString136
	mov	-386(%rbp), %edx
	mov	-406(%rbp), %eax
	add	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -406(%rbp)
.Lfloat_toString136:
	movq	-362(%rbp), %rdx
	movq	4(%rdx), %rbx
	movq	%rbx, -362(%rbp)
	mov	-386(%rbp), %eax
	cltd
	idivl	-402(%rbp)
	movl	%eax, %ebx
	movl	%ebx, -386(%rbp)
	mov	$1, %edx
	mov	-394(%rbp), %eax
	add	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -394(%rbp)
.Lfloat_toString135:
	movl	-394(%rbp), %eax
	movl	$23, %ecx
	cmpl	%ecx, %eax
	jl	.Lfloat_toString134
	lea	-330(%rbp), %rax
	movq	$1, %rdi
	movq	%rax, %rsi
	movq	$1, %rdx
	movq	$1, %rax
	syscall	
	movl	-390(%rbp), %eax
	movl	%eax, %edi
	call	printInt
	movq	$.strfloat_toString139, %rax
	movq	%rax, %rdi
	call	print
	movl	-406(%rbp), %eax
	movl	%eax, %edi
	call	printInt
	movl	-390(%rbp), %eax
	movl	%eax, %edi
	lea	-416(%rbp), %rax
	movq	%rax, %rax
	movq	%rax, %rsi
	call	int_toString
	movq	-330(%rbp), %rax
	movq	%rax, %rdi
	lea	-416(%rbp), %rax
	movq	%rax, %rax
	movq	%rax, %rsi
	movq	-314(%rbp), %rax
	movq	%rax, %rdx
	call	str_concat
	movq	-314(%rbp), %rax
	movq	%rax, %rdi
	movq	$.strfloat_toString140, %rax
	movq	%rax, %rsi
	movq	-314(%rbp), %rax
	movq	%rax, %rdx
	call	str_concat
	movl	-406(%rbp), %eax
	movl	%eax, %edi
	lea	-416(%rbp), %rax
	movq	%rax, %rax
	movq	%rax, %rsi
	call	int_toString
	movq	-314(%rbp), %rax
	movq	%rax, %rdi
	lea	-416(%rbp), %rax
	movq	%rax, %rax
	movq	%rax, %rsi
	movq	-314(%rbp), %rax
	movq	%rax, %rdx
	call	str_concat
	movl	$0, %eax
	leave
	ret
str_subString:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$469, %rsp
	movl	%edi, -420(%rbp)
	movl	%esi, -424(%rbp)
	movq	%rdx, -432(%rbp)
	movq	%rcx, -440(%rbp)
	movq	$0, -448(%rbp)
	lea	-420(%rbp), %rax
	movq	%rax, %rbx
	movq	%rbx, -448(%rbp)
	movq	-448(%rbp), %rax
	movl	(%rax), %eax
	movl	%eax, -456(%rbp)
	mov	-456(%rbp), %rdx
	mov	-432(%rbp), %rax
	add	%rdx, %rax
	movl	%eax, %ebx
	movl	%ebx, -432(%rbp)
	movq	-440(%rbp), %rbx
	movq	%rbx, -464(%rbp)
	movl	$0, -468(%rbp)
	jmp	.Lstr_subString142
.Lstr_subString141:
	movq	-432(%rbp), %rax
	movb	(%rax), %al
	movb	%al, -469(%rbp)
	movq	-464(%rbp), %rax
	movb	-469(%rbp), %bl
	movb	%bl, (%rax)
	mov	$1, %rdx
	mov	-464(%rbp), %rax
	add	%rdx, %rax
	movl	%eax, %ebx
	movl	%ebx, -464(%rbp)
	mov	$1, %rdx
	mov	-432(%rbp), %rax
	add	%rdx, %rax
	movl	%eax, %ebx
	movl	%ebx, -432(%rbp)
	mov	$1, %edx
	mov	-468(%rbp), %eax
	add	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -468(%rbp)
.Lstr_subString142:
	movl	-468(%rbp), %eax
	movl	-424(%rbp), %ecx
	cmpl	%ecx, %eax
	jl	.Lstr_subString141
	movq	-464(%rbp), %rax
	movb	$0, %bl
	movb	%bl, (%rax)
	movq	-440(%rbp), %rax
	leave
	ret
str_reverse:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$510, %rsp
	movq	%rdi, -477(%rbp)
	movq	%rsi, -485(%rbp)
	movq	-477(%rbp), %rax
	movq	%rax, %rdi
	call	len
	movl	%eax, -489(%rbp)
	movq	-485(%rbp), %rbx
	movq	%rbx, -497(%rbp)
	mov	$1, %edx
	mov	-489(%rbp), %eax
	sub	%edx, %eax
	movl	%eax, -501(%rbp)
	jmp	.Lstr_reverse144
.Lstr_reverse143:
	mov	-501(%rbp), %edx
	mov	-477(%rbp), %rax
	add	%rdx, %rax
	movl	%eax, -509(%rbp)
	movq	-509(%rbp), %rax
	movb	(%rax), %al
	movb	%al, -510(%rbp)
	movq	-485(%rbp), %rax
	movb	-510(%rbp), %bl
	movb	%bl, (%rax)
	mov	$1, %edx
	mov	-485(%rbp), %rax
	add	%rdx, %rax
	movl	%eax, %ebx
	movl	%ebx, -485(%rbp)
	mov	$1, %edx
	mov	-501(%rbp), %eax
	sub	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -501(%rbp)
.Lstr_reverse144:
	movl	-501(%rbp), %eax
	movl	$-1, %ecx
	cmpl	%ecx, %eax
	jg	.Lstr_reverse143
	movq	-485(%rbp), %rax
	movb	$0, %bl
	movb	%bl, (%rax)
	movq	-497(%rbp), %rax
	leave
	ret
str_at:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$530, %rsp
	movq	%rdi, -518(%rbp)
	movl	%esi, -522(%rbp)
	mov	-522(%rbp), %edx
	mov	-518(%rbp), %rax
	add	%rdx, %rax
	movl	%eax, -530(%rbp)
	movq	-530(%rbp), %rax
	movb	(%rax), %al
	movb	%al, %al
	leave
	ret
str_forEach:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$567, %rsp
	movq	%rdi, -538(%rbp)
	movq	%rsi, -546(%rbp)
	movq	%rdx, -554(%rbp)
	movq	-538(%rbp), %rax
	movq	%rax, %rdi
	call	len
	movl	%eax, -558(%rbp)
	movl	$0, -562(%rbp)
	jmp	.Lstr_forEach146
.Lstr_forEach145:
	movq	-538(%rbp), %rax
	movq	%rax, %rdi
	movl	-562(%rbp), %eax
	movl	%eax, %esi
	call	str_at
	movb	%al, -563(%rbp)
	movq	-546(%rbp), %rcx
	movb	-563(%rbp), %al
	movb	%al, %dil
	movl	-562(%rbp), %eax
	movl	%eax, %esi
	movq	-554(%rbp), %rax
	movq	%rax, %rdx
	call	*%rcx
	movl	%eax, -567(%rbp)
	movl	-567(%rbp), %eax
	movl	$1, %ecx
	cmpl	%ecx, %eax
	jne	.Lstr_forEach147
	movl	-567(%rbp), %eax
	leave
	ret
.Lstr_forEach147:
	mov	$1, %edx
	mov	-562(%rbp), %eax
	add	%edx, %eax
	movl	%eax, %ebx
	movl	%ebx, -562(%rbp)
.Lstr_forEach146:
	movl	-562(%rbp), %eax
	movl	-558(%rbp), %ecx
	cmpl	%ecx, %eax
	jl	.Lstr_forEach145
	movl	$0, %eax
	leave
	ret


.data

.strfloat_toString140:
	.asciz	 "."
.strfloat_toString139:
	.asciz	 "."
.strfloat_toString128:
	.asciz	 "+"
.strfloat_toString121:
	.asciz	 "-"
.strfloat_toString110:
	.asciz	 ""
.floatstr_toFloat106:
	.float	0.0
.floatstr_toFloat105:
	.float	0.00
.floatstr_toFloat100:
	.float	0.1
.floatstr_toFloat94:
	.float	10.0
.floatstr_toFloat93:
	.float	0.00
.floatstr_toFloat92:
	.float	10.0
.floatstr_toFloat88:
	.float	1.0
.floatstr_toFloat87:
	.float	0.00
.floatcharToFloat86:
	.float	9.00
.floatcharToFloat82:
	.float	8.00
.floatcharToFloat78:
	.float	7.00
.floatcharToFloat74:
	.float	6.00
.floatcharToFloat70:
	.float	5.00
.floatcharToFloat66:
	.float	4.00
.floatcharToFloat62:
	.float	3.00
.floatcharToFloat58:
	.float	2.00
.floatcharToFloat54:
	.float	1.00
.floatcharToFloat50:
	.float	0.00
.floatcharToFloat46:
	.float	0.00
.strstr_split45:
	.asciz	 ""


.bss

