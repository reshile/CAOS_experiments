#include <dirent.h>
#include <sys/stat.h>
#include <stddef.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h> // getopt
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <getopt.h>

enum mode { default_mode = 0777 };

int MakeDirNoParent(int optind, int argc, char* argv[], mode_t mode) {
  int res = 0;
  for (int i = optind; i < argc; ++i) {
    res = mkdir(argv[i], mode);
    if (res < 0) {
      fprintf(stderr, "Cannot create directory");
      return res;
    }
  }
  return res;
}

int MakeDirWithParent(char* name, mode_t mode, bool recurse) {
  if (recurse) {
  char* sub_dir = strrchr(name, '/');
  if (sub_dir != NULL) {
    *sub_dir = 0;
    int err = MakeDirWithParent(name, 0777, 1);
    printf("ERROR IS = %d\n", err);
    if (err == 0) {
    printf("Created directory: %s\n", name);
    } else {
    perror("mkdir");
    }
    printf("ERROR IS = %d\n", err);
    *sub_dir = '/';
    }
  }
  return mkdir(name, mode);
}

/*
int MakeDirWithParent(char* name, mode_t mode) {
  int res = 0;
  const char* sub_name = strtok(name, "/");
  name = strtok(NULL, "\0");
  printf("SUB DIR = %s\n", sub_name);
  printf("Name = %s\n", name);
  res = mkdir(sub_name, 0777);
  if (res < 0) {
    fprintf(stderr, "Cannot create directory. (exists)\n");
    return res;
  }
  chdir(sub_name);
  if (strchr(name, '/') != NULL) {
    res = MakeDirWithParent(name, mode);
    chdir("../");
    return res;
  }
  mkdir(name, mode);
  printf("HERE GOES MODE = %d\n", 040);
  chdir("../");
  return 0;
}
*/

int MakeDir(int optind, int argc, char* argv[], mode_t mode) {
  int res = 0;
  for (int i = optind; i < argc; ++i) {
    res = MakeDirWithParent(argv[i], mode, true);
  }
  return res;
}

static struct option longopts[] = {
  {"mode", required_argument, 0, 'm'},
  {"parents", no_argument, 0, 'p'},
  {0, 0, 0, 0}
};

int main(int argc, char* argv[]) {
  int opt;
  int flag_p = 0;
  // int flag_m = 0;
  mode_t mode = 0777;

  while ((opt = getopt_long(argc, argv, "pm:", longopts, 0)) != -1) {
    switch (opt) {
      case 'p':
        flag_p = 1;
        break;
      case 'm':
        mode = strtol(optarg, NULL, 8);
        break;
      default:
        fprintf(stderr, "Something went wrong.\n");
        exit(1);
    }
  }

  if (optind >= argc) {
    fprintf(stderr, "Not enought arguments.\n");
    exit(1);
  }
  printf("Mode is = %d\n", mode);
  printf("Str argument is = %s\n", argv[optind]);

  int result = 0;
  if (flag_p == 0) {
    result = MakeDirNoParent(optind, argc, argv, mode);
  } else {
    result = MakeDir(optind, argc, argv, mode);
  }
  return result;
}
