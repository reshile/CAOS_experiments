#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>

#define error(msg) \
               do { perror(msg); exit(EXIT_FAILURE); } while (0)
// Functions below are required to
// get correct arguments from argv[]
int CountPipes(char* argv[]) {
  int cnt = 0;
  int i = 0;
  while (argv[1][i] != '\0') {
    if (argv[1][i] == '|') {
      ++cnt;
    }
    ++i;
  }
  return cnt;
}

// Split argv into arguments of each
// corresponding command between pipes
char** GetArguments(char* argv[], int pipe_cnt) {
  char** args = calloc(pipe_cnt + 1, sizeof(char*));
  char* str;
  char* substr;
  char* saveptr;
  const char* pipe = "|";

  int i;
  for (i = 1, str = argv[1]; ; ++i, str = NULL) {
    substr = strtok_r(str, pipe, &saveptr);
    if (substr == NULL) {
      break;
    }
    args[i - 1] = substr;
  }

  return args;
    // subdivide parts below in main
}

int GetWordCount(char subargs[]) {
  // 0 <-> next char is - space, 1 <-> next char is not a separator
  int state = 0;
  int cnt = 0;
  while(*subargs) {
    if (*subargs == ' ') {
      state = 0;
    } else if (state == 0) {
      state = 1;
      ++cnt;
    }
    ++subargs;
  }
  return cnt;
}

// Divide argument list of each command into
// smaller strings to pass into execvp
char** GetArgumentsFromString(char subargs[]) {
  int word_cnt = GetWordCount(subargs);
  char** exec_args = calloc(word_cnt + 1, sizeof(char*));

  char* str;
  char* substr;
  char* saveptr;
  const char* space = " ";

  int i;
  for (i = 1, str = subargs; ; ++i, str = NULL) {
    substr = strtok_r(str, space, &saveptr);
    if (substr == NULL) {
      break;
    }
    exec_args[i - 1] = substr;
  }
  exec_args[word_cnt] = NULL;
  return exec_args;
}
// End of helper functions

// cmd0 ... | cmd1 ... | cmd2 ... | ... | cmdN-1 ...
// zero-indexing 
void RecursivePipe(int total_cnt, int current, char** args, int input_fd) {
  if (current == total_cnt - 1) {
    int result;
    result = dup2(input_fd, STDIN_FILENO);
    if (result == -1) {
      error("dup2");
    }
    //close(input_fd);
    char** exec_args = GetArgumentsFromString(args[current]);
    pid_t cpid;
    cpid = fork();
    if (cpid == 0) {
      execvp(exec_args[0], exec_args);
    }
    // in case exec fails
    free(exec_args);
    //error("execvp 2");
  } else {
    int fds[2];
    pid_t cpid;

    char** exec_args = GetArgumentsFromString(args[current]);
    if (pipe(fds) == -1) {
      error("pipe");
    }

    cpid = fork();
    if (cpid < 0) {
      error("fork");
    }

    if (cpid == 0) {
      int result;
      close(fds[0]);
      result = dup2(input_fd, STDIN_FILENO);
      if (result == -1) {
        error("dup2");
      }
      result = dup2(fds[1], STDOUT_FILENO);
      if (result == -1) {
        error("dup2");
      }
      result = close(fds[1]);
      if (result == -1) {
        error("close");
      }
      // need to free exec_args
      execvp(exec_args[0], exec_args);
      // in case exec fails
      free(exec_args);
      error("excevp 1");
    }
    free(exec_args);
    close(fds[1]);
    close(input_fd);
    RecursivePipe(total_cnt, ++current, args, fds[0]);
  }
}

int main(int argc, char* argv[]) {
  int pipe_cnt = CountPipes(argv);
  // pass these arguments to processes
  char** args = GetArguments(argv, pipe_cnt);

  RecursivePipe(pipe_cnt + 1, 0, args, STDIN_FILENO);
  wait(NULL);
  /*
  for (int i = 0; i < pipe_cnt + 1; ++i) {
    int wc = GetWordCount(args[i]);
    printf("string is: %s, word count is: %d\n", args[i], wc);
    char** exec_args = GetArgumentsFromString(args[i]);
    printf("   command name is: %s\n", GetCommandName(args[i]));
    for (int j = 0; j <= wc; ++j) {
      printf("    --> %s\n", exec_args[j]);
    }
    free(exec_args);
  }
  */

  free(args);
}
