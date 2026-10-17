/* Exercise the assembly boundary independently of the AFlat compiler. */
#include <assert.h>
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

extern long poisoned_setsockopt(int, int, int, const void *, unsigned);
extern long poisoned_sendto(int, const void *, unsigned, int, const void *,
                            unsigned);

/* Make the old missing rcx -> r10 move fail deterministically. */
__asm__(".text\n"
        "poisoned_setsockopt:\n xor %r10d, %r10d\n jmp sys_setsockopt\n"
        "poisoned_sendto:\n mov $1, %r10d\n jmp sys_sendto\n");

int main(void) {
  int pair[2];
  assert(socketpair(AF_UNIX, SOCK_STREAM, 0, pair) == 0);
  int value = 4096;
  assert(poisoned_setsockopt(pair[0], SOL_SOCKET, SO_SNDBUF, &value,
                             sizeof(value)) == 0);
  socklen_t length = sizeof(value);
  value = 0;
  assert(getsockopt(pair[0], SOL_SOCKET, SO_SNDBUF, &value, &length) == 0);
  assert(value == 8192); /* Linux doubles SO_SNDBUF. */
  assert(poisoned_setsockopt(pair[0], SOL_SOCKET, SO_SNDBUF, NULL,
                             sizeof(value)) == -EFAULT);

  char payload[65536];
  memset(payload, 0x5a, sizeof(payload));
  char received[65536];
  /* A missing fourth argument would turn this into MSG_OOB, so there
   * would be no ordinary byte available to recv. Keep it nonblocking. */
  assert(poisoned_sendto(pair[0], payload, 1, MSG_DONTWAIT | MSG_NOSIGNAL, NULL,
                         0) == 1);
  assert(recv(pair[1], received, 1, MSG_DONTWAIT) == 1);
  assert(received[0] == payload[0]);
  long sent = poisoned_sendto(pair[0], payload, sizeof(payload),
                              MSG_DONTWAIT | MSG_NOSIGNAL, NULL, 0);
  assert(sent > 0 && sent < (long)sizeof(payload));
  assert(recv(pair[1], received, sizeof(received), MSG_DONTWAIT) == sent);
  assert(memcmp(payload, received, sent) == 0);
  /* Fill the send queue and verify raw negative errno, without hanging. */
  do {
    sent = poisoned_sendto(pair[0], payload, sizeof(payload),
                           MSG_DONTWAIT | MSG_NOSIGNAL, NULL, 0);
  } while (sent > 0);
  assert(sent == -EAGAIN);
  close(pair[1]);
  signal(SIGPIPE, SIG_DFL);
  assert(poisoned_sendto(pair[0], payload, 1, MSG_NOSIGNAL, NULL, 0) == -EPIPE);
  close(pair[0]);
  puts("socket ABI: options, partial sends, EAGAIN, EFAULT, and MSG_NOSIGNAL "
       "passed");
}
