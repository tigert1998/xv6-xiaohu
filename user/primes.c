#include "kernel/fcntl.h"
#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char* argv[]) {
  int left = 2, right = 35;

  int left_fds[2] = {-1, -1}, right_fds[2] = {-1, -1};
  int index, pid;
  for (index = 0; index < right - left; index++) {
    if (left_fds[0] != -1) {
      close(left_fds[0]);
    }
    if (left_fds[1] != -1) {
      close(left_fds[1]);
    }

    memmove(left_fds, right_fds, sizeof(left_fds));

    pipe(right_fds);  // TODO
    pid = fork();
    if (pid != 0) {
      break;
    }
  }

  if (pid == 0) {
    if (left_fds[0] != -1) {
      close(left_fds[0]);
    }
    if (left_fds[1] != -1) {
      close(left_fds[1]);
    }

    memmove(left_fds, right_fds, sizeof(left_fds));

    right_fds[0] = right_fds[1] = -1;
  }

  if (left_fds[1] != -1) {
    close(left_fds[1]);
  }
  if (right_fds[0] != -1) {
    close(right_fds[0]);
  }

  if (index == 0) {
    int prime = left;
    fprintf(1, "prime %d\n", prime);

    for (int i = left + 1; i <= right; i++) {
      if (i % prime == 0) continue;
      write(right_fds[1], (void*)&i, 4);
    }
    close(right_fds[1]);

    wait(0);
    exit(0);
  } else {
    int prime;
    int n = read(left_fds[0], &prime, 4);
    if (n <= 0) {
      close(left_fds[0]);
      if (right_fds[1] != -1) {
        close(right_fds[1]);
      }

      wait(0);
      exit(0);
    } else {
      fprintf(1, "prime %d\n", prime);
    }

    while (1) {
      int i;
      int n = read(left_fds[0], &i, 4);
      if (n <= 0) {
        break;
      } else {
        if (i % prime != 0) {
          write(right_fds[1], &i, 4);
        }
      }
    }
    close(left_fds[0]);
    if (right_fds[1] != -1) {
      close(right_fds[1]);
    }
    wait(0);
    exit(0);
  }
}