#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <getopt.h>
#include <string.h>
#include <stdbool.h>
#include <dirent.h>

int IsDir(const char* name) {
  struct stat st;
  int res = lstat(name, &st);
  if (res < 0) {
    return -1;
  }
  return (S_ISDIR(st.st_mode) == 1);
}

int IsReg(const char* name) {
  struct stat st;
  int res = lstat(name, &st);
  if (res < 0) {
    return -1;
  }
  return (S_ISREG(st.st_mode) == 1);
}

int FileRemover(int optindex, int argc, char* argv[]) {
  int result = 0;
  for (int i = optindex; i < argc; ++i) {
    int is_dir = IsDir(argv[i]);
    if (is_dir == 0) {
      unlink(argv[i]);
    } else {
      fprintf(stderr, "Add -r to remove directory. (or directory does not exist)\n");
      result = -1; 
    }
  }
  return result;
}

int DirRemover(const char* name) {
  DIR* dir;
  struct dirent* entry;
  int result = 0;

  chdir(name);
  dir = opendir("./");
  if (dir == NULL) {
    fprintf(stderr, "Cannot open directory");
    return -1;
  }

  while ((entry = readdir(dir)) != NULL) {
    if (!strcmp(entry->d_name, "..") || !strcmp(entry->d_name, ".")) {
      continue;
    }
    if (IsDir(entry->d_name) == 1) {
      DirRemover(entry->d_name);
    } else if (IsDir(entry->d_name) < 0) {
      fprintf(stderr, "Cannot remove directory or does not exists. %s\n", entry->d_name);
      continue;
    } else {
      unlink(entry->d_name);
    }
  }
  closedir(dir);
  chdir("../");
  int re = rmdir(name);
  printf("Result of rmdir is = %d\n", re);
  return result;
}

int Remover(int optindex, int argc, char* argv[]) {
  int result = 0;
  for (int i = optindex; i < argc; ++i) {
    if (IsDir(argv[i]) == 1) {
      result = DirRemover(argv[i]);
    } else if (IsDir(argv[i]) < 0) {
      fprintf(stderr, "Smth went wrong (does not exists)\n");
      continue;
    } else {
      result = unlink(argv[i]);
      if (result < 0) {
        fprintf(stderr, "Cannot remove file: %s\n", argv[i]);
      }
    }
  }
  return result;
}

int main(int argc, char* argv[]) {
  bool contains_r = false;

  int opt;
  while ((opt = getopt(argc, argv, "r")) != -1) {
    switch (opt) {
    case 'r':
      contains_r = true;
      break;
    default:
      break;
    }
  }

  if (contains_r && argc <= 2) {
    fprintf(stderr, "Wrong argument list.\n");
    return -1;
  }

  if (contains_r) {
    return Remover(optind, argc, argv);
  }
  return FileRemover(optind, argc, argv); 
}
