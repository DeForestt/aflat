.global brk
.global sys_write
.global sys_create
.global sys_open
.global sys_read
.global sys_close
.global sys_fstat
.global sys_fork
.global sys_pause
.global sys_nsleep
.global sys_time
.global sys_times
.global sys_exit
.global sys_wait
.global sys_pipe
.global sys_kill
.global sys_socket
.global sys_setsockopt
.global sys_bind
.global sys_listen
.global sys_accept
.global sys_sendto
.global sys_connect
.global sys_execve
.global sys_clock_gettime
.global sys_seek

brk:
    pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	subq	$16, %rsp
    movq    $12, %rax
    syscall
    leave
	ret

sys_write:
    pushq	%rbp
    movq	%rsp, %rbp
    pushq	%rbx
    subq	$16, %rsp
    movq    $1, %rax
    syscall
    leave
    ret

sys_read:
    pushq	%rbp
    movq	%rsp, %rbp
    pushq	%rbx
    subq	$16, %rsp
    movq    $0, %rax
    syscall
    leave
    ret

sys_create:
    pushq	%rbp
    movq	%rsp, %rbp
    pushq	%rbx
    subq	$16, %rsp
    movq    $85, %rax
    syscall
    leave
    ret

sys_open:
    pushq	%rbp
    movq	%rsp, %rbp
    pushq	%rbx
    subq	$16, %rsp
    movq    $2, %rax
    syscall
    leave
    ret

sys_close:
    pushq	%rbp
    movq	%rsp, %rbp
    pushq	%rbx
    subq	$16, %rsp
    movq    $3, %rax
    syscall
    leave
    ret

sys_fstat:
    pushq	%rbp
    movq	%rsp, %rbp
    pushq	%rbx
    subq	$16, %rsp
    movq    $5, %rax
    syscall
    leave
    ret

sys_fork:
    pushq	%rbp
    movq	%rsp, %rbp
    pushq	%rbx
    subq	$16, %rsp
    movq    $57, %rax
    syscall
    leave
    ret

sys_pause:
    pushq	%rbp
    movq	%rsp, %rbp
    pushq	%rbx
    subq	$16, %rsp
    movq    $34, %rax
    syscall
    leave
    ret

sys_nsleep:
    pushq	%rbp
    movq	%rsp, %rbp
    pushq	%rbx
    subq	$16, %rsp
    movq    $35, %rax
    syscall
    leave
    ret

sys_time:
    pushq	%rbp
    movq	%rsp, %rbp
    pushq	%rbx
    subq	$16, %rsp
    movq    $201, %rax
    syscall
    leave
    ret

sys_times:
    pushq	%rbp
    movq	%rsp, %rbp
    pushq	%rbx
    subq	$16, %rsp
    movq    $100, %rax
    syscall
    leave
    ret

sys_exit:
    pushq	%rbp
    movq	%rsp, %rbp
    pushq	%rbx
    subq	$16, %rsp
    movq    $60, %rax
    syscall
    leave
    ret

sys_wait:
    pushq	%rbp
    movq	%rsp, %rbp
    pushq	%rbx
    subq	$16, %rsp
    movq    $61, %rax
    syscall
    leave
    ret

sys_pipe:
    pushq	%rbp
    movq	%rsp, %rbp
    pushq	%rbx
    subq	$16, %rsp
    movq    $22, %rax
    syscall
    leave
    ret

sys_kill:
    pushq	%rbp
    movq	%rsp, %rbp
    pushq	%rbx
    subq	$16, %rsp
    movq    $62, %rax
    syscall
    leave
    ret

sys_socket:
    pushq	%rbp
    movq	%rsp, %rbp
    pushq	%rbx
    subq	$16, %rsp
    movq    $41, %rax
    syscall
    leave
    ret

sys_setsockopt:
    pushq	%rbp
    movq	%rsp, %rbp
    pushq	%rbx
    subq	$16, %rsp
    movq    $54, %rax
    syscall
    leave
    ret

sys_bind:
    pushq	%rbp
    movq	%rsp, %rbp
    pushq	%rbx
    subq	$16, %rsp
    movq    $49, %rax
    syscall
    leave
    ret

sys_listen:
    pushq	%rbp
    movq	%rsp, %rbp
    pushq	%rbx
    subq	$16, %rsp
    movq    $50, %rax
    syscall
    leave
    ret

sys_accept:
    pushq	%rbp
    movq	%rsp, %rbp
    pushq	%rbx
    subq	$16, %rsp
    movq    $43, %rax
    syscall
    leave
    ret
    
sys_sendto:
    pushq	%rbp
    movq	%rsp, %rbp
    pushq	%rbx
    subq	$16, %rsp
    movq    $44, %rax
    syscall
    leave
    ret

sys_connect:
    pushq	%rbp
    movq	%rsp, %rbp
    pushq	%rbx
    subq	$16, %rsp
    movq    $42, %rax
    syscall
    leave
    ret

sys_execve:
    pushq	%rbp
    movq	%rsp, %rbp
    pushq	%rbx
    subq	$16, %rsp
    movq    $59, %rax
    syscall
    leave
    ret

sys_clock_gettime:
    pushq	%rbp
    movq	%rsp, %rbp
    pushq	%rbx
    subq	$16, %rsp
    movq    $228, %rax
    syscall
    leave
    ret

sys_seek:
    pushq	%rbp
    movq	%rsp, %rbp
    pushq	%rbx
    subq	$16, %rsp
    movq    $8, %rax
    syscall
    leave
    ret

