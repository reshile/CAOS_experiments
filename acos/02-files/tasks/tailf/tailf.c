#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <unistd.h> /* read(), write(), close() */
#include <fcntl.h> /* open() */
#include <sys/types.h> /* mode_t */
#include <sys/stat.h> /* stat */
#include <stdint.h>
#include <inttypes.h>
#include <stdbool.h>

off_t NewSize(const char* name, struct stat* file_s) {
  stat(name, file_s);
  off_t new_size = file_s->st_size;
  return new_size;
}

int StartFile(const char* name) {
  struct stat file_s;
  int res = stat(name, &file_s);
  if (res < 0) {
    fprintf(stderr, "stat failed.\n"); // Possibly file does not exist.
    return -1;
  }

  int fd = open(name, O_RDWR);
  if (fd < 0) {
    fprintf(stderr, "Could not open file\n.");
    return -1;
  }
  off_t old_size = 0;
  int stdout_fd = 1;
  ssize_t result;

  while (true) {
    off_t new_size = NewSize(name, &file_s);
    if (new_size == old_size) {
      usleep(1000);
      continue;
    }

    uint8_t* buf = (uint8_t*)(malloc(new_size - old_size));
    result = read(fd, buf, (size_t)(new_size - old_size));
    if (result < 0) {
      fprintf(stderr, "Error while reading.\n");
      close(fd);
      free(buf);
      return -1;
    }

    if (result > 0) {
      result = write(stdout_fd, &buf[0], result);
      if (result < 0) {
        fprintf(stderr, "Error while writing.\n");
        close(fd);
        free(buf);
        return -1;
      }
    }
    free(buf);
    usleep(1000);
    old_size = new_size;
  }
  return 0;
}

int main(int argc, const char* argv[]) {
  if (argc != 2) {
    fprintf(stderr, "wrong arguments.\n");
    return -1;
  }
  return StartFile(argv[1]);
}
