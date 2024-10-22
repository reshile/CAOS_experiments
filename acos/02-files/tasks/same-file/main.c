#include <stdbool.h>
#include <stdio.h>
#include <sys/stat.h>

bool is_same_file(const char* lhs_path, const char* rhs_path) {
  struct stat lhs_buf;
  struct stat rhs_buf;
  int res = stat(lhs_path, &lhs_buf);
  if (res < 0) {
    // fprintf(stderr, "stat failed.\n");
    return false;
  }
  res = stat(rhs_path, &rhs_buf);
  if (res < 0) {
    // fprintf(stderr, "stat failed.\n");
    return false;
  }
  long int lhs_ino = (long int)lhs_buf.st_ino;
  long int rhs_ino = (long int)rhs_buf.st_ino;
  return (lhs_ino == rhs_ino);
}

int main(int argc, const char* argv[]) {
  if (argc != 3) {
    printf("Error occured!\n");
    return -1;
  }
  // printf("%s %s", argv[1], argv[2]);
  if (is_same_file(argv[1], argv[2])) {
    printf("yes\n");
  } else {
    printf("no\n");
  }
  return 0;
}
