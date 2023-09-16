#include "kernel/types.h"
#include "user/user.h"

int main() {
  int fds[2];
  if (pipe(fds) < 0) {
    fprintf(2, "Fail to create pipe\n");
  }
  int pid = fork();
  if (pid == 0) {
    char buf[1];
    int n = read(fds[0], buf, 1);
    if (n == 1) {
      fprintf(1, "%d: received ping\n", getpid());
    } else {
      fprintf(2, "The child fails to read from the pipe");
      exit(1);
    }

    n = write(fds[1], buf, 1);
    if (n != 1) {
      fprintf(2, "The child fails to write to the pipe");
      exit(1);
    }
    exit(0);
  } else {
    char buf[1] = {'A'};
    int n = write(fds[1], buf, 1);
    if (n != 1) {
      fprintf(2, "The parent fails to write to the pipe");
      exit(1);
    }

    n = read(fds[0], buf, 1);
    if (n != 1) {
      fprintf(2, "The parent fails to read from the pipe");
      exit(1);
    }
    fprintf(1, "%d: received pong\n", getpid());
    exit(0);
  }
}