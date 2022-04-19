.global	DateTime.mmddyyyyToDate
.global	DateTime.Now
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

pub_ICollection_init:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$16, %rsp
	movq	%rdi, -8(%rbp)
	pushq	%r14
	movq	-8(%rbp), %r14
	movq	$lambda_1650382340930_0, %rbx
	movq	%rbx, 0(%r14)
	popq	%r14
	pushq	%r14
	movq	-8(%rbp), %r14
	movq	$lambda_1650382340930_2, %rbx
	movq	%rbx, 8(%r14)
	popq	%r14
	movq	-8(%rbp), %r15
	movq	%r15, %rax
	leave
	ret
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
	movl	-4(%rbp), %r15d
	movl	%r15d, %edi
	cmpl	%edx, %edi
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.LdaysInMonth4
	pushq	%rdi
	pushq	%rdx
	movl	$0, %edx
	pushq	%rdi
	pushq	%rdx
	movl	-8(%rbp), %r15d
	movl	%r15d, %eax
	movl	$4, %ecx
	cltd
	idiv	%ecx
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
	je	.LdaysInMonth5
	pushq	%rdi
	pushq	%rdx
	movl	$0, %edx
	pushq	%rdi
	pushq	%rdx
	movl	-8(%rbp), %r15d
	movl	%r15d, %eax
	movl	$100, %ecx
	cltd
	idiv	%ecx
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
	je	.LdaysInMonth6
	pushq	%rdi
	pushq	%rdx
	movl	$0, %edx
	pushq	%rdi
	pushq	%rdx
	movl	-8(%rbp), %r15d
	movl	%r15d, %eax
	movl	$400, %ecx
	cltd
	idiv	%ecx
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
	je	.LdaysInMonth7
	movl	$29, %eax
	leave
	ret
.LdaysInMonth7:
	movl	$28, %eax
	leave
	ret
.LdaysInMonth6:
	movl	$29, %eax
	leave
	ret
.LdaysInMonth5:
	movl	$28, %eax
	leave
	ret
.LdaysInMonth4:
	pushq	%rdi
	pushq	%rdx
	pushq	%rdi
	pushq	%rdx
	movl	$11, %edx
	movl	-4(%rbp), %r15d
	movl	%r15d, %edi
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
	movl	-4(%rbp), %r15d
	movl	%r15d, %edi
	cmpl	%edx, %edi
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %dl
	pushq	%rdi
	pushq	%rdx
	movl	$6, %edx
	movl	-4(%rbp), %r15d
	movl	%r15d, %edi
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
	movl	-4(%rbp), %r15d
	movl	%r15d, %edi
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
	je	.LdaysInMonth8
	movl	$30, %eax
	leave
	ret
.LdaysInMonth8:
	movl	$31, %eax
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
	movl	-4(%rbp), %r15d
	movl	%r15d, %eax
	movl	$4, %ecx
	cltd
	idiv	%ecx
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
	je	.LdaysInYear9
	pushq	%rdi
	pushq	%rdx
	movl	$0, %edx
	pushq	%rdi
	pushq	%rdx
	movl	-4(%rbp), %r15d
	movl	%r15d, %eax
	movl	$100, %ecx
	cltd
	idiv	%ecx
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
	je	.LdaysInYear10
	pushq	%rdi
	pushq	%rdx
	movl	$0, %edx
	pushq	%rdi
	pushq	%rdx
	movl	-4(%rbp), %r15d
	movl	%r15d, %eax
	movl	$400, %ecx
	cltd
	idiv	%ecx
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
	je	.LdaysInYear11
	movl	$366, %eax
	leave
	ret
.LdaysInYear11:
	movl	$365, %eax
	leave
	ret
.LdaysInYear10:
	movl	$366, %eax
	leave
	ret
.LdaysInYear9:
	movl	$365, %eax
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
	jmp	.LdaysSinceJan113
.LdaysSinceJan112:
	pushq	%rdi
	pushq	%rdx
	pushq	%rdx
	movl	-20(%rbp), %r15d
	pushq	%rdi
	movl	%r15d, %eax
	movl	%eax, %edi
	movl	-12(%rbp), %r15d
	pushq	%rsi
	movl	%r15d, %eax
	movl	%eax, %esi
	call	daysInMonth
	popq	%rsi
	popq	%rdi
	popq	%rdx
	movl	%eax, %edx
	movl	-16(%rbp), %r15d
	movl	%r15d, %edi
	add	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -16(%rbp)
	add	$1, -20(%rbp)
.LdaysSinceJan113:
	pushq	%rdi
	pushq	%rdx
	movl	-4(%rbp), %r15d
	movl	%r15d, %edx
	movl	-20(%rbp), %r15d
	movl	%r15d, %edi
	cmpl	%edx, %edi
	setl	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$1, %al
	je	.LdaysSinceJan112
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	pushq	%rdi
	pushq	%rdx
	movl	-8(%rbp), %r15d
	movl	%r15d, %edx
	movl	-16(%rbp), %r15d
	movl	%r15d, %edi
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
	movl	-16(%rbp), %r15d
	movl	%r15d, %eax
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
	movl	-4(%rbp), %r15d
	pushq	%rdi
	movl	%r15d, %eax
	movl	%eax, %edi
	movl	-8(%rbp), %r15d
	pushq	%rsi
	movl	%r15d, %eax
	movl	%eax, %esi
	movl	-12(%rbp), %r15d
	pushq	%rdx
	movl	%r15d, %eax
	movl	%eax, %edx
	call	daysSinceJan1
	popq	%rdx
	popq	%rsi
	popq	%rdi
	popq	%rdx
	movl	%eax, %ebx
	movl	%ebx, -20(%rbp)
	movl	-16(%rbp), %r15d
	movl	%r15d, %ebx
	movl	%ebx, -24(%rbp)
	jmp	.LdateRelativeTo15
.LdateRelativeTo14:
	pushq	%rdi
	pushq	%rdx
	pushq	%rdx
	movl	-24(%rbp), %r15d
	pushq	%rdi
	movl	%r15d, %eax
	movl	%eax, %edi
	call	daysInYear
	popq	%rdi
	popq	%rdx
	movl	%eax, %edx
	movl	-20(%rbp), %r15d
	movl	%r15d, %edi
	add	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -20(%rbp)
	add	$1, -24(%rbp)
.LdateRelativeTo15:
	pushq	%rdi
	pushq	%rdx
	movl	-12(%rbp), %r15d
	movl	%r15d, %edx
	movl	-24(%rbp), %r15d
	movl	%r15d, %edi
	cmpl	%edx, %edi
	setl	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$1, %al
	je	.LdateRelativeTo14
	movl	-20(%rbp), %r15d
	movl	%r15d, %eax
	leave
	ret
pub_DateTime_epochToDate:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$48, %rsp
	movq	%rdi, -8(%rbp)
	pushq	%r14
	movq	-8(%rbp), %r14
	movl	0(%r14), %r15d
	popq	%r14
	movl	%r15d, %ebx
	movl	%ebx, -12(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	-12(%rbp), %r15d
	movl	%r15d, %eax
	movl	$86400, %ecx
	cltd
	idiv	%ecx
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
	jmp	.LepochToDate17
.LepochToDate16:
	pushq	%rdi
	pushq	%rdx
	pushq	%rdx
	movl	-20(%rbp), %r15d
	pushq	%rdi
	movl	%r15d, %eax
	movl	%eax, %edi
	call	daysInYear
	popq	%rdi
	popq	%rdx
	movl	%eax, %edx
	movl	-16(%rbp), %r15d
	movl	%r15d, %edi
	sub	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -16(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movl	-20(%rbp), %r15d
	movl	%r15d, %edi
	add	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -20(%rbp)
.LepochToDate17:
	pushq	%rdi
	pushq	%rdx
	pushq	%rdx
	movl	-20(%rbp), %r15d
	pushq	%rdi
	movl	%r15d, %eax
	movl	%eax, %edi
	call	daysInYear
	popq	%rdi
	popq	%rdx
	movl	%eax, %edx
	movl	-16(%rbp), %r15d
	movl	%r15d, %edi
	cmpl	%edx, %edi
	setge	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$1, %al
	je	.LepochToDate16
	jmp	.LepochToDate19
.LepochToDate18:
	pushq	%rdi
	pushq	%rdx
	pushq	%rdx
	movl	-24(%rbp), %r15d
	pushq	%rdi
	movl	%r15d, %eax
	movl	%eax, %edi
	movl	-20(%rbp), %r15d
	pushq	%rsi
	movl	%r15d, %eax
	movl	%eax, %esi
	call	daysInMonth
	popq	%rsi
	popq	%rdi
	popq	%rdx
	movl	%eax, %edx
	movl	-16(%rbp), %r15d
	movl	%r15d, %edi
	sub	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -16(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	$1, %edx
	movl	-24(%rbp), %r15d
	movl	%r15d, %edi
	add	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -24(%rbp)
.LepochToDate19:
	pushq	%rdi
	pushq	%rdx
	pushq	%rdx
	movl	-24(%rbp), %r15d
	pushq	%rdi
	movl	%r15d, %eax
	movl	%eax, %edi
	movl	-20(%rbp), %r15d
	pushq	%rsi
	movl	%r15d, %eax
	movl	%eax, %esi
	call	daysInMonth
	popq	%rsi
	popq	%rdi
	popq	%rdx
	movl	%eax, %edx
	movl	-16(%rbp), %r15d
	movl	%r15d, %edi
	cmpl	%edx, %edi
	setge	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$1, %al
	je	.LepochToDate18
	pushq	%rdi
	pushq	%rdx
	movl	-16(%rbp), %r15d
	movl	%r15d, %edx
	movl	-28(%rbp), %r15d
	movl	%r15d, %edi
	add	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -28(%rbp)
	pushq	%r14
	movq	-8(%rbp), %r14
	movl	-20(%rbp), %r15d
	movl	%r15d, %ebx
	movl	%ebx, 12(%r14)
	popq	%r14
	pushq	%r14
	movq	-8(%rbp), %r14
	movl	-24(%rbp), %r15d
	movl	%r15d, %ebx
	movl	%ebx, 8(%r14)
	popq	%r14
	pushq	%r14
	movq	-8(%rbp), %r14
	movl	-28(%rbp), %r15d
	movl	%r15d, %ebx
	movl	%ebx, 4(%r14)
	popq	%r14
	pushq	%rdi
	pushq	%rdx
	movl	-12(%rbp), %r15d
	movl	%r15d, %eax
	movl	$86400, %ecx
	cltd
	idiv	%ecx
	movl	%edx, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -32(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	-32(%rbp), %r15d
	movl	%r15d, %eax
	movl	$3600, %ecx
	cltd
	idiv	%ecx
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -36(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	-32(%rbp), %r15d
	movl	%r15d, %eax
	movl	$3600, %ecx
	cltd
	idiv	%ecx
	movl	%edx, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -32(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	-32(%rbp), %r15d
	movl	%r15d, %eax
	movl	$60, %ecx
	cltd
	idiv	%ecx
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -40(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	-32(%rbp), %r15d
	movl	%r15d, %eax
	movl	$60, %ecx
	cltd
	idiv	%ecx
	movl	%edx, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -44(%rbp)
	pushq	%r14
	movq	-8(%rbp), %r14
	movl	-36(%rbp), %r15d
	movl	%r15d, %ebx
	movl	%ebx, 16(%r14)
	popq	%r14
	pushq	%r14
	movq	-8(%rbp), %r14
	movl	-40(%rbp), %r15d
	movl	%r15d, %ebx
	movl	%ebx, 20(%r14)
	popq	%r14
	pushq	%r14
	movq	-8(%rbp), %r14
	movl	-44(%rbp), %r15d
	movl	%r15d, %ebx
	movl	%ebx, 24(%r14)
	popq	%r14
	movl	$0, %eax
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
	movl	-4(%rbp), %r15d
	pushq	%rdi
	movl	%r15d, %eax
	movl	%eax, %edi
	movl	-8(%rbp), %r15d
	pushq	%rsi
	movl	%r15d, %eax
	movl	%eax, %esi
	movl	-12(%rbp), %r15d
	pushq	%rdx
	movl	%r15d, %eax
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
	movl	-16(%rbp), %r15d
	movl	%r15d, %eax
	movl	$7, %ecx
	cltd
	idiv	%ecx
	movl	%edx, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %eax
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
	movl	-4(%rbp), %r15d
	movl	%r15d, %edi
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
	movl	$1, %edx
	movl	-4(%rbp), %r15d
	movl	%r15d, %edi
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
	movl	$2, %edx
	movl	-4(%rbp), %r15d
	movl	%r15d, %edi
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
	movl	$3, %edx
	movl	-4(%rbp), %r15d
	movl	%r15d, %edi
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
	movl	$4, %edx
	movl	-4(%rbp), %r15d
	movl	%r15d, %edi
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
	pushq	%rdi
	pushq	%rdx
	movl	$5, %edx
	movl	-4(%rbp), %r15d
	movl	%r15d, %edi
	cmpl	%edx, %edi
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.LdowToString30
	movq	$.strdowToString31, %rax
	leave
	ret
.LdowToString30:
	pushq	%rdi
	pushq	%rdx
	movl	$6, %edx
	movl	-4(%rbp), %r15d
	movl	%r15d, %edi
	cmpl	%edx, %edi
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.LdowToString32
	movq	$.strdowToString33, %rax
	leave
	ret
.LdowToString32:
	pushq	%rdx
	pushq	%rdi
	movq	$.strdowToString34, %rax
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
	pushq	%r14
	movq	-8(%rbp), %r14
	movl	-12(%rbp), %r15d
	movl	%r15d, %ebx
	movl	%ebx, 0(%r14)
	popq	%r14
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
	pushq	%r14
	movq	-8(%rbp), %r14
	movl	0(%r14), %r15d
	popq	%r14
	movl	%r15d, %eax
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
	pushq	%r14
	movq	-8(%rbp), %r14
	movl	0(%r14), %r15d
	popq	%r14
	movl	%r15d, %eax
	movl	$60, %ecx
	cltd
	idiv	%ecx
	popq	%rdx
	popq	%rdi
	movl	%eax, %eax
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
	pushq	%r14
	movq	-8(%rbp), %r14
	movl	0(%r14), %r15d
	popq	%r14
	movl	%r15d, %eax
	movl	$3600, %ecx
	cltd
	idiv	%ecx
	popq	%rdx
	popq	%rdi
	movl	%eax, %eax
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
	pushq	%r14
	movq	-8(%rbp), %r14
	movl	0(%r14), %r15d
	popq	%r14
	movl	%r15d, %eax
	movl	$86400, %ecx
	cltd
	idiv	%ecx
	popq	%rdx
	popq	%rdi
	movl	%eax, %eax
	leave
	ret
pub_DateTime_getDay:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$16, %rsp
	movq	%rdi, -8(%rbp)
	pushq	%r14
	movq	-8(%rbp), %r14
	movl	4(%r14), %r15d
	popq	%r14
	movl	%r15d, %eax
	leave
	ret
pub_DateTime_getMonth:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$16, %rsp
	movq	%rdi, -8(%rbp)
	pushq	%r14
	movq	-8(%rbp), %r14
	movl	8(%r14), %r15d
	popq	%r14
	movl	%r15d, %eax
	leave
	ret
pub_DateTime_getYear:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$16, %rsp
	movq	%rdi, -8(%rbp)
	pushq	%r14
	movq	-8(%rbp), %r14
	movl	12(%r14), %r15d
	popq	%r14
	movl	%r15d, %eax
	leave
	ret
pub_DateTime_getSecond:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$16, %rsp
	movq	%rdi, -8(%rbp)
	pushq	%r14
	movq	-8(%rbp), %r14
	movl	24(%r14), %r15d
	popq	%r14
	movl	%r15d, %eax
	leave
	ret
pub_DateTime_getMinute:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$16, %rsp
	movq	%rdi, -8(%rbp)
	pushq	%r14
	movq	-8(%rbp), %r14
	movl	20(%r14), %r15d
	popq	%r14
	movl	%r15d, %eax
	leave
	ret
pub_DateTime_getHour:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$16, %rsp
	movq	%rdi, -8(%rbp)
	pushq	%r14
	movq	-8(%rbp), %r14
	movl	16(%r14), %r15d
	popq	%r14
	movl	%r15d, %eax
	leave
	ret
DateTime.Now:
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
	movl	-4(%rbp), %r15d
	movl	%r15d, %edi
	imul	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -4(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	-4(%rbp), %r15d
	movl	%r15d, %edx
	movl	-8(%rbp), %r15d
	movl	%r15d, %edi
	add	%edx, %edi
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
	movl	-8(%rbp), %r15d
	pushq	%rsi
	movl	%r15d, %eax
	movl	%eax, %esi
	call	pub_DateTime_init
	popq	%rsi
	popq	%rdi
	popq	%rdx
	movq	%rax, %rax
	leave
	ret
DateTime.mmddyyyyToDate:
mmddyyyyToDate:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$64, %rsp
	movq	%rdi, -8(%rbp)
	movb	%sil, -9(%rbp)
	pushq	%rdi
	pushq	%rdx
	movb	$0, %dl
	movb	-9(%rbp), %r15b
	movb	%r15b, %dil
	cmpb	%dl, %dil
	sete	%al
	popq	%rdx
	popq	%rdi
	movb	%al, %al
	cmpb	$0, %al
	je	.LmmddyyyyToDate35
	movb	$45, %bl
	movb	%bl, -9(%rbp)
.LmmddyyyyToDate35:
	pushq	%rdx
	lea	-8(%rbp), %rax
	pushq	%rdi
	movq	(%rax), %rax
	movq	%rax, %rdi
	movb	-9(%rbp), %r15b
	pushq	%rsi
	movb	%r15b, %al
	movb	%al, %sil
	call	pub_String_split
	popq	%rsi
	popq	%rdi
	popq	%rdx
	movq	%rax, %rbx
	movq	%rbx, -17(%rbp)
	pushq	%rdx
	pushq	%rdi
	pushq	%rdx
	movl	$3, %edx
	pushq	%rdx
	lea	-17(%rbp), %rax
	pushq	%rdi
	movq	(%rax), %rax
	movq	%rax, %rdi
	call	pub_LinkedList_size
	popq	%rdi
	popq	%rdx
	movl	%eax, %edi
	cmpl	%edx, %edi
	sete	%al
	popq	%rdx
	popq	%rdi
	pushq	%rdi
	movb	%al, %al
	movb	%al, %dil
	pushq	%rsi
	movq	$.strmmddyyyyToDate36, %rax
	movq	%rax, %rsi
	call	assert
	popq	%rsi
	popq	%rdi
	popq	%rdx
	pushq	%rdx
	lea	-17(%rbp), %rax
	pushq	%rdi
	movq	(%rax), %rax
	movq	%rax, %rdi
	pushq	%rsi
	movl	$0, %eax
	movl	%eax, %esi
	call	pub_LinkedList_get
	popq	%rsi
	popq	%rdi
	popq	%rdx
	movq	%rax, %rbx
	movq	%rbx, -25(%rbp)
	pushq	%rdx
	lea	-17(%rbp), %rax
	pushq	%rdi
	movq	(%rax), %rax
	movq	%rax, %rdi
	pushq	%rsi
	movl	$1, %eax
	movl	%eax, %esi
	call	pub_LinkedList_get
	popq	%rsi
	popq	%rdi
	popq	%rdx
	movq	%rax, %rbx
	movq	%rbx, -33(%rbp)
	pushq	%rdx
	lea	-17(%rbp), %rax
	pushq	%rdi
	movq	(%rax), %rax
	movq	%rax, %rdi
	pushq	%rsi
	movl	$2, %eax
	movl	%eax, %esi
	call	pub_LinkedList_get
	popq	%rsi
	popq	%rdi
	popq	%rdx
	movq	%rax, %rbx
	movq	%rbx, -41(%rbp)
	pushq	%rdx
	lea	-25(%rbp), %rax
	pushq	%rdi
	movq	(%rax), %rax
	movq	%rax, %rdi
	call	pub_String_toInt
	popq	%rdi
	popq	%rdx
	movl	%eax, %ebx
	movl	%ebx, -45(%rbp)
	pushq	%rdx
	lea	-33(%rbp), %rax
	pushq	%rdi
	movq	(%rax), %rax
	movq	%rax, %rdi
	call	pub_String_toInt
	popq	%rdi
	popq	%rdx
	movl	%eax, %ebx
	movl	%ebx, -49(%rbp)
	pushq	%rdx
	lea	-41(%rbp), %rax
	pushq	%rdi
	movq	(%rax), %rax
	movq	%rax, %rdi
	call	pub_String_toInt
	popq	%rdi
	popq	%rdx
	movl	%eax, %ebx
	movl	%ebx, -53(%rbp)
	pushq	%rdx
	movl	-45(%rbp), %r15d
	pushq	%rdi
	movl	%r15d, %eax
	movl	%eax, %edi
	movl	-49(%rbp), %r15d
	pushq	%rsi
	movl	%r15d, %eax
	movl	%eax, %esi
	movl	-53(%rbp), %r15d
	pushq	%rdx
	movl	%r15d, %eax
	movl	%eax, %edx
	pushq	%rcx
	movl	$1970, %eax
	movl	%eax, %ecx
	call	dateRelativeTo
	popq	%rcx
	popq	%rdx
	popq	%rsi
	popq	%rdi
	popq	%rdx
	movl	%eax, %ebx
	movl	%ebx, -57(%rbp)
	pushq	%rdi
	pushq	%rdx
	movl	$86400, %edx
	movl	-57(%rbp), %r15d
	movl	%r15d, %edi
	imul	%edx, %edi
	movl	%edi, %eax
	popq	%rdx
	popq	%rdi
	movl	%eax, %ebx
	movl	%ebx, -61(%rbp)
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
	movl	-61(%rbp), %r15d
	pushq	%rsi
	movl	%r15d, %eax
	movl	%eax, %esi
	call	pub_DateTime_init
	popq	%rsi
	popq	%rdi
	popq	%rdx
	movq	%rax, %rax
	leave
	ret
lambda_1650382340930_0:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$16, %rsp
	pushq	%rdx
	pushq	%rdi
	movq	$.strICollection.lambda_1650382340930_01, %rax
	movq	%rax, %rdi
	call	panic
	popq	%rdi
	popq	%rdx
	leave
	ret
lambda_1650382340930_2:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$16, %rsp
	pushq	%rdx
	pushq	%rdi
	movq	$.strICollection.lambda_1650382340930_23, %rax
	movq	%rax, %rdi
	call	panic
	popq	%rdi
	popq	%rdx
	leave
	ret


.data

.strICollection.lambda_1650382340930_23:
	.asciz	 "Function (where) not implemented on this instance"
.strICollection.lambda_1650382340930_01:
	.asciz	 "Function (forEach) not implemented on this instance"
.strmmddyyyyToDate36:
	.asciz	 "Invalid date format passed to mmddyyyyToDate"
.strdowToString34:
	.asciz	 "Invalid day of week"
.strdowToString33:
	.asciz	 "Saturday"
.strdowToString31:
	.asciz	 "Friday"
.strdowToString29:
	.asciz	 "Thursday"
.strdowToString27:
	.asciz	 "Wednesday"
.strdowToString25:
	.asciz	 "Tuesday"
.strdowToString23:
	.asciz	 "Monday"
.strdowToString21:
	.asciz	 "Sunday"


.bss

