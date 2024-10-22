#include <linux/limits.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/ptrace.h>
#include <sys/reg.h>
#include <sys/user.h>
#include <sys/syscall.h>

#define error(msg) \
               do { perror(msg); exit(EXIT_FAILURE); } while (0)

// Divide argument list of each command into
// smaller strings to pass into execvp
char** GetArguments(int argc, char* argv[]) {
  char** args = calloc(argc, sizeof(char*));
  args[0] = argv[1];
  for (int i = 1; i < argc - 1; ++i) {
    args[i] =  argv[i + 1];
  }
  args[argc - 1] = NULL;
  return args;
}

typedef struct Counter{
    char filename[PATH_MAX];
    int counter;
    struct Counter* next;
} Counter;

typedef struct Counters{
    struct Counter* head;
} Counters;

void increment(Counters* counters, char* filename, int value) {
    Counter* current = counters->head;
    while (current != NULL) {
        if (strncmp(current->filename, filename, PATH_MAX) == 0) {
            current->counter += value;
            return;
        }
        current = current->next;
    }
    Counter* new_head = malloc(sizeof(Counter));
    new_head->next = counters->head;
    new_head->counter = value;
    strncpy(new_head->filename, filename, PATH_MAX - 1);
    counters->head = new_head;
}

void print(Counters* counters) {
    Counter* current = counters->head;
    while (current != NULL) {
        printf("%s:%d\n", current->filename, current->counter);
        current = current->next;
    }
}

void Query(char** args, Counters* counters) {
  pid_t cpid;
  int64_t rax_reg;
  int state;
  int syscall_active = 0;
  struct user_regs_struct registers;

  cpid = fork();

  if (cpid == 0) {
    ptrace(PTRACE_TRACEME, 0, NULL, NULL);
    execvp(args[0], args);
  } else {
    while (true) {
      wait(&state);
      if (WIFEXITED(state)) {
        break;
      }
      rax_reg = ptrace(PTRACE_PEEKUSER, cpid, 8 * ORIG_RAX, NULL);
      if (rax_reg == SYS_write) {
        if (syscall_active == 0) {
          syscall_active = 1;
          ptrace(PTRACE_GETREGS, cpid, NULL, &registers);
          char filename[1000];
          char path[PATH_MAX];
          sprintf(path, "/proc/%d/fd/%lld", cpid, registers.rdi);
          readlink(path, filename, 1000);
          increment(counters, filename, registers.rdx);
        } else {
          syscall_active = 0;
        }
      }
      ptrace(PTRACE_SYSCALL, cpid, NULL, NULL);
    }
  }
}

int main(int argc, char *argv[]) {
    Counters* counters = malloc(sizeof(Counter));
    counters->head = NULL;
    char** args = GetArguments(argc, argv);
    Query(args, counters);

    print(counters);
    free(args);
}
