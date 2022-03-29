.global	Now
.global	pub_DateTime_getHour
.global	pub_DateTime_getMinute
.global	pub_DateTime_getSecond
.global	pub_DateTime_getYear
.global	pub_DateTime_getMonth
.global	pub_DateTime_getDay
.global	pub_DateTime_getDays
.global	pub_DateTime_getHours
.global	pub_DateTime_getMinutes
.global	pub_DateTime_getSeconds
.global	pub_DateTime_init


.text

daysInMonth:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$16, %rsp
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	$2, %edx
	movl	-4(%rbp), %edi
	cmpl	%edx, %edi
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.LdaysInMonth0
	pushq	%rdi
	pushq	%rdx
	movl	$0, %edx
	pushq	%rdi
	pushq	%rdx
	movl	-8(%rbp), %eax
	movl	$4, %ecx
	cltd
	idivl	%ecx
	movl	%edx, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %edi
	cmpl	%edx, %edi
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.LdaysInMonth1
	pushq	%rdi
	pushq	%rdx
	movl	$0, %edx
	pushq	%rdi
	pushq	%rdx
	movl	-8(%rbp), %eax
	movl	$100, %ecx
	cltd
	idivl	%ecx
	movl	%edx, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %edi
	cmpl	%edx, %edi
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.LdaysInMonth2
	pushq	%rdi
	pushq	%rdx
	movl	$0, %edx
	pushq	%rdi
	pushq	%rdx
	movl	-8(%rbp), %eax
	movl	$400, %ecx
	cltd
	idivl	%ecx
	movl	%edx, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %edi
	cmpl	%edx, %edi
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.LdaysInMonth3
	movl	$29, %eax
	leave
	ret
.LdaysInMonth3:
	movl	$28, %eax
	leave
	ret
.LdaysInMonth2:
	movl	$29, %eax
	leave
	ret
.LdaysInMonth1:
	movl	$28, %eax
	leave
	ret
.LdaysInMonth0:
	pushq	%rdi
	pushq	%rdx
	pushq	%rdi
	pushq	%rdx
	movl	$11, %edx
	movl	-4(%rbp), %edi
	cmpl	%edx, %edi
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %dl
	pushq	%rdi
	pushq	%rdx
	pushq	%rdi
	pushq	%rdx
	pushq	%rdi
	pushq	%rdx
	movl	$9, %edx
	movl	-4(%rbp), %edi
	cmpl	%edx, %edi
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %dl
	pushq	%rdi
	pushq	%rdx
	movl	$6, %edx
	movl	-4(%rbp), %edi
	cmpl	%edx, %edi
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %dil
	orb	%dl, %dil
	movb	%dil, %al
	popq	%rdx
	popq	%rdi
	movb	%al, %dl
	pushq	%rdi
	pushq	%rdx
	movl	$4, %edx
	movl	-4(%rbp), %edi
	cmpl	%edx, %edi
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %dil
	orb	%dl, %dil
	movb	%dil, %al
	popq	%rdx
	popq	%rdi
	movb	%al, %dil
	orb	%dl, %dil
	movb	%dil, %al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.LdaysInMonth4
	movl	$30, %eax
	leave
	ret
.LdaysInMonth4:
	movl	$31, %eax
	leave
	ret
	leave
	ret
daysInYear:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$16, %rsp
	movl	%edi, -4(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	$0, %edx
	pushq	%rdi
	pushq	%rdx
	movl	-4(%rbp), %eax
	movl	$4, %ecx
	cltd
	idivl	%ecx
	movl	%edx, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %edi
	cmpl	%edx, %edi
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.LdaysInYear5
	pushq	%rdi
	pushq	%rdx
	movl	$0, %edx
	pushq	%rdi
	pushq	%rdx
	movl	-4(%rbp), %eax
	movl	$100, %ecx
	cltd
	idivl	%ecx
	movl	%edx, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %edi
	cmpl	%edx, %edi
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.LdaysInYear6
	pushq	%rdi
	pushq	%rdx
	movl	$0, %edx
	pushq	%rdi
	pushq	%rdx
	movl	-4(%rbp), %eax
	movl	$400, %ecx
	cltd
	idivl	%ecx
	movl	%edx, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %edi
	cmpl	%edx, %edi
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.LdaysInYear7
	movl	$366, %eax
	leave
	ret
.LdaysInYear7:
	movl	$365, %eax
	leave
	ret
.LdaysInYear6:
	movl	$366, %eax
	leave
	ret
.LdaysInYear5:
	movl	$365, %eax
	leave
	ret
	leave
	ret
daysSinceJan1:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$32, %rsp
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	movl	%edx, -12(%rbp)
	movl	$0, %ebx
	movl	%ebx, -16(%rbp)
	movl	$1, %ebx
	movl	%ebx, -20(%rbp)
	jmp	.LdaysSinceJan19
.LdaysSinceJan18:
	pushq	%rdi
	pushq	%rdx
	pushq	%rdx
	pushq	%rdi
	movl	-20(%rbp), %eax
	movl	%eax, %edi
	pushq	%rsi
	movl	-12(%rbp), %eax
	movl	%eax, %esi
	call	daysInMonth
	popq	%rsi
	popq	%rdi
	popq	%rdx
	movl	%eax, %edx
	movl	-16(%rbp), %edi
	add	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -16(%rbp)
	add	$1, -20(%rbp)
.LdaysSinceJan19:
	pushq	%rdi
	pushq	%rdx
	movl	-4(%rbp), %edx
	movl	-20(%rbp), %edi
	cmpl	%edx, %edi
	setl	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$1, %al
	je	.LdaysSinceJan18
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	pushq	%rdi
	pushq	%rdx
	movl	-8(%rbp), %edx
	movl	-16(%rbp), %edi
	add	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %edi
	sub	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -16(%rbp)
	movl	-16(%rbp), %eax
	leave
	ret
	leave
	ret
dateRelativeTo:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$32, %rsp
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	movl	%edx, -12(%rbp)
	movl	%ecx, -16(%rbp)
	pushq	%rdx
	pushq	%rdi
	movl	-4(%rbp), %eax
	movl	%eax, %edi
	pushq	%rsi
	movl	-8(%rbp), %eax
	movl	%eax, %esi
	pushq	%rdx
	movl	-12(%rbp), %eax
	movl	%eax, %edx
	call	daysSinceJan1
	popq	%rdx
	popq	%rsi
	popq	%rdi
	popq	%rdx
	movl	%eax, %ebx
	movl	%ebx, -20(%rbp)
	movl	-16(%rbp), %ebx
	movl	%ebx, -24(%rbp)
	jmp	.LdateRelativeTo11
.LdateRelativeTo10:
	pushq	%rdi
	pushq	%rdx
	pushq	%rdx
	pushq	%rdi
	movl	-24(%rbp), %eax
	movl	%eax, %edi
	call	daysInYear
	popq	%rdi
	popq	%rdx
	movl	%eax, %edx
	movl	-20(%rbp), %edi
	add	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -20(%rbp)
	add	$1, -24(%rbp)
.LdateRelativeTo11:
	pushq	%rdi
	pushq	%rdx
	movl	-12(%rbp), %edx
	movl	-24(%rbp), %edi
	cmpl	%edx, %edi
	setl	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$1, %al
	je	.LdateRelativeTo10
	movl	-20(%rbp), %eax
	leave
	ret
	leave
	ret
pub_DateTime_epochToDate:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$48, %rsp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %r14
	movl	0(%r14), %ebx
	movl	%ebx, -12(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	-12(%rbp), %eax
	movl	$86400, %ecx
	cltd
	idivl	%ecx
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -16(%rbp)
	movl	$1970, %ebx
	movl	%ebx, -20(%rbp)
	movl	$1, %ebx
	movl	%ebx, -24(%rbp)
	movl	$1, %ebx
	movl	%ebx, -28(%rbp)
	jmp	.LepochToDate13
.LepochToDate12:
	pushq	%rdi
	pushq	%rdx
	pushq	%rdx
	pushq	%rdi
	movl	-20(%rbp), %eax
	movl	%eax, %edi
	call	daysInYear
	popq	%rdi
	popq	%rdx
	movl	%eax, %edx
	movl	-16(%rbp), %edi
	sub	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -16(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movl	-20(%rbp), %edi
	add	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -20(%rbp)
.LepochToDate13:
	pushq	%rdi
	pushq	%rdx
	pushq	%rdx
	pushq	%rdi
	movl	-20(%rbp), %eax
	movl	%eax, %edi
	call	daysInYear
	popq	%rdi
	popq	%rdx
	movl	%eax, %edx
	movl	-16(%rbp), %edi
	cmpl	%edx, %edi
	setg	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$1, %al
	je	.LepochToDate12
	jmp	.LepochToDate15
.LepochToDate14:
	pushq	%rdi
	pushq	%rdx
	pushq	%rdx
	pushq	%rdi
	movl	-24(%rbp), %eax
	movl	%eax, %edi
	pushq	%rsi
	movl	-20(%rbp), %eax
	movl	%eax, %esi
	call	daysInMonth
	popq	%rsi
	popq	%rdi
	popq	%rdx
	movl	%eax, %edx
	movl	-16(%rbp), %edi
	sub	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -16(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movl	-24(%rbp), %edi
	add	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -24(%rbp)
.LepochToDate15:
	pushq	%rdi
	pushq	%rdx
	pushq	%rdx
	pushq	%rdi
	movl	-24(%rbp), %eax
	movl	%eax, %edi
	pushq	%rsi
	movl	-20(%rbp), %eax
	movl	%eax, %esi
	call	daysInMonth
	popq	%rsi
	popq	%rdi
	popq	%rdx
	movl	%eax, %edx
	movl	-16(%rbp), %edi
	cmpl	%edx, %edi
	setg	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$1, %al
	je	.LepochToDate14
	pushq	%rdi
	pushq	%rdx
	movl	-16(%rbp), %edx
	movl	-28(%rbp), %edi
	add	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -28(%rbp)
	movq	-8(%rbp), %rdx
	movl	-20(%rbp), %ebx
	movl	%ebx, 12(%rdx)
	movq	-8(%rbp), %rdx
	movl	-24(%rbp), %ebx
	movl	%ebx, 8(%rdx)
	movq	-8(%rbp), %rdx
	movl	-28(%rbp), %ebx
	movl	%ebx, 4(%rdx)
	pushq	%rdi
	pushq	%rdx
	movl	-12(%rbp), %eax
	movl	$86400, %ecx
	cltd
	idivl	%ecx
	movl	%edx, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -32(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	-32(%rbp), %eax
	movl	$3600, %ecx
	cltd
	idivl	%ecx
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -36(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	-32(%rbp), %eax
	movl	$3600, %ecx
	cltd
	idivl	%ecx
	movl	%edx, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -32(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	-32(%rbp), %eax
	movl	$60, %ecx
	cltd
	idivl	%ecx
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -40(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	-32(%rbp), %eax
	movl	$60, %ecx
	cltd
	idivl	%ecx
	movl	%edx, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -44(%rbp)
	movq	-8(%rbp), %rdx
	movl	-36(%rbp), %ebx
	movl	%ebx, 16(%rdx)
	movq	-8(%rbp), %rdx
	movl	-40(%rbp), %ebx
	movl	%ebx, 20(%rdx)
	movq	-8(%rbp), %rdx
	movl	-44(%rbp), %ebx
	movl	%ebx, 24(%rdx)
	movl	$0, %eax
	leave
	ret
	leave
	ret
dayOfWeek:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$16, %rsp
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	movl	%edx, -12(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	pushq	%rdx
	pushq	%rdi
	movl	-4(%rbp), %eax
	movl	%eax, %edi
	pushq	%rsi
	movl	-8(%rbp), %eax
	movl	%eax, %esi
	pushq	%rdx
	movl	-12(%rbp), %eax
	movl	%eax, %edx
	pushq	%rcx
	movl	$1, %eax
	movl	%eax, %ecx
	call	dateRelativeTo
	popq	%rcx
	popq	%rdx
	popq	%rsi
	popq	%rdi
	popq	%rdx
	movl	%eax, %edi
	add	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -16(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	-16(%rbp), %eax
	movl	$7, %ecx
	cltd
	idivl	%ecx
	movl	%edx, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %eax
	leave
	ret
	leave
	ret
dowToString:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$16, %rsp
	movl	%edi, -4(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	$0, %edx
	movl	-4(%rbp), %edi
	cmpl	%edx, %edi
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.LdowToString16
	movq	$.strdowToString17, %rax
	leave
	ret
.LdowToString16:
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movl	-4(%rbp), %edi
	cmpl	%edx, %edi
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.LdowToString18
	movq	$.strdowToString19, %rax
	leave
	ret
.LdowToString18:
	pushq	%rdi
	pushq	%rdx
	movl	$2, %edx
	movl	-4(%rbp), %edi
	cmpl	%edx, %edi
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.LdowToString20
	movq	$.strdowToString21, %rax
	leave
	ret
.LdowToString20:
	pushq	%rdi
	pushq	%rdx
	movl	$3, %edx
	movl	-4(%rbp), %edi
	cmpl	%edx, %edi
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.LdowToString22
	movq	$.strdowToString23, %rax
	leave
	ret
.LdowToString22:
	pushq	%rdi
	pushq	%rdx
	movl	$4, %edx
	movl	-4(%rbp), %edi
	cmpl	%edx, %edi
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.LdowToString24
	movq	$.strdowToString25, %rax
	leave
	ret
.LdowToString24:
	pushq	%rdi
	pushq	%rdx
	movl	$5, %edx
	movl	-4(%rbp), %edi
	cmpl	%edx, %edi
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.LdowToString26
	movq	$.strdowToString27, %rax
	leave
	ret
.LdowToString26:
	pushq	%rdi
	pushq	%rdx
	movl	$6, %edx
	movl	-4(%rbp), %edi
	cmpl	%edx, %edi
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.LdowToString28
	movq	$.strdowToString29, %rax
	leave
	ret
.LdowToString28:
	pushq	%rdx
	pushq	%rdi
	movq	$.strdowToString30, %rax
	movq	%rax, %rdi
	call	panic
	popq	%rdi
	popq	%rdx
	leave
	ret
pub_DateTime_init:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$16, %rsp
	movq	%rdi, -8(%rbp)
	movl	%esi, -12(%rbp)
	movq	-8(%rbp), %rdx
	movl	-12(%rbp), %ebx
	movl	%ebx, 0(%rdx)
	pushq	%rdx
	lea	-8(%rbp), %rax
	pushq	%rdi
	movq	(%rax), %rax
	movq	%rax, %rdi
	call	pub_DateTime_epochToDate
	popq	%rdi
	popq	%rdx
	popq	%rbx
	movq	-8(%rbp), %rax
	leave
	ret
pub_DateTime_getSeconds:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$16, %rsp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %r14
	movl	0(%r14), %eax
	leave
	ret
	leave
	ret
pub_DateTime_getMinutes:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$16, %rsp
	movq	%rdi, -8(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	0(%r14), %eax
	movl	$60, %ecx
	cltd
	idivl	%ecx
	popq	%rdx
	popq	%rdi
	movl	%eax, %eax
	leave
	ret
	leave
	ret
pub_DateTime_getHours:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$16, %rsp
	movq	%rdi, -8(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	0(%r14), %eax
	movl	$3600, %ecx
	cltd
	idivl	%ecx
	popq	%rdx
	popq	%rdi
	movl	%eax, %eax
	leave
	ret
	leave
	ret
pub_DateTime_getDays:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$16, %rsp
	movq	%rdi, -8(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	0(%r14), %eax
	movl	$86400, %ecx
	cltd
	idivl	%ecx
	popq	%rdx
	popq	%rdi
	movl	%eax, %eax
	leave
	ret
	leave
	ret
pub_DateTime_getDay:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$16, %rsp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %r14
	movl	4(%r14), %eax
	leave
	ret
	leave
	ret
pub_DateTime_getMonth:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$16, %rsp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %r14
	movl	8(%r14), %eax
	leave
	ret
	leave
	ret
pub_DateTime_getYear:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$16, %rsp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %r14
	movl	12(%r14), %eax
	leave
	ret
	leave
	ret
pub_DateTime_getSecond:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$16, %rsp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %r14
	movl	24(%r14), %eax
	leave
	ret
	leave
	ret
pub_DateTime_getMinute:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$16, %rsp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %r14
	movl	20(%r14), %eax
	leave
	ret
	leave
	ret
pub_DateTime_getHour:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$16, %rsp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %r14
	movl	16(%r14), %eax
	leave
	ret
	leave
	ret
Now:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$16, %rsp
	movl	%edi, -4(%rbp)
	pushq	%rdx
	movq	$0, %rdi
	call	sys_time
	popq	%rdx
	movl	%eax, %ebx
	movl	%ebx, -8(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	$3600, %edx
	movl	-4(%rbp), %edi
	imul	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -4(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	-4(%rbp), %edx
	movl	-8(%rbp), %edi
	sub	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -8(%rbp)
	pushq	%rdx
	pushq	%rdi
	movl	$28, %eax
	movl	%eax, %edi
	call	malloc
	popq	%rdi
	popq	%rdx
	pushq	%rdi
	movq	%rax, %rdi
	pushq	%rdx
	pushq	%rsi
	movl	-8(%rbp), %eax
	movl	%eax, %esi
	call	pub_DateTime_init
	popq	%rsi
	popq	%rdi
	popq	%rdx
	movq	%rax, %rax
	leave
	ret
	leave
	ret


.data

.strdowToString30:
	.asciz	 "Invalid day of week"
.strdowToString29:
	.asciz	 "Saturday"
.strdowToString27:
	.asciz	 "Friday"
.strdowToString25:
	.asciz	 "Thursday"
.strdowToString23:
	.asciz	 "Wednesday"
.strdowToString21:
	.asciz	 "Tuesday"
.strdowToString19:
	.asciz	 "Monday"
.strdowToString17:
	.asciz	 "Sunday"


.bss

