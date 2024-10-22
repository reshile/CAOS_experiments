#include "lca.h"
#include <dirent.h>     /* Defines DT_* constants */
#include <err.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/syscall.h>
#include <unistd.h>

pid_t GetParent(pid_t pid) {
  char path[256];
  sprintf(path, "/proc/%d/stat", pid);
  int fd = open(path, O_RDONLY);
  char buffer[1024];
  int result = read(fd, &buffer, sizeof(buffer));
  int ppid;
  sscanf(buffer, "%*d %*s %*s %d", &ppid);
  close(fd);
  return ppid;
}

pid_t find_lca(pid_t x, pid_t y) {
  int xtree[MAX_TREE_DEPTH];
  int ytree[MAX_TREE_DEPTH];
  xtree[0] = x;
  ytree[0] = y;
  int xlast = 1;
  int ylast = 1;
  pid_t xcur = x;
  pid_t ycur = y;
  pid_t xpar;
  pid_t ypar;
  while ((xpar = GetParent(xcur)) != 0 && xlast < MAX_TREE_DEPTH) {
    xtree[xlast] = xpar;
    xcur = xpar;
    ++xlast;
  }
  while ((ypar = GetParent(ycur)) != 0 && ylast < MAX_TREE_DEPTH) {
    ytree[ylast] = ypar;
    ycur = ypar;
    ++ylast;
  }
  while ((xlast > 0) && (ylast > 0) && (xtree[--xlast] == ytree[--ylast])) {}
  // may be equal, because first expression blocks afterwards checking
  if (xtree[xlast] == ytree[ylast]) {
    return xtree[xlast];
  }
  return xtree[xlast + 1];
}
