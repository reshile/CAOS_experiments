#define _GNU_SOURCE
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include "signalfd.h"

int signal_fd_read;
int signal_fd_write;

void my_handler(int signum) {
  // printf("HI THERE\n");
  write(signal_fd_write, &signum, 4);
}

int signalfd() {
  int fd[2];
  pipe2(fd, O_CLOEXEC);
  signal_fd_read = fd[0];
  signal_fd_write = fd[1];
  struct sigaction sa;
  memset(&sa, 0, sizeof(sa));
  sa.sa_handler = &my_handler;
  for (int signum = 1; signum < 32; ++signum) {
    if ((signum == SIGKILL) || (signum == SIGSTOP)) { continue; }
    sigaction(signum, &sa, NULL);
  }
  return signal_fd_read;
}
