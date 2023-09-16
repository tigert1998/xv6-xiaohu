#include "kernel/types.h"
#include "user/user.h"
// clang-format off
#include "kernel/stat.h"
#include "kernel/fs.h"
// clang-format on

void find(const char* dir, const char* filename) {
  int fd;
  struct stat st;
  struct dirent de;
  char buf[512], *p;

  if ((fd = open(dir, 0)) < 0) {
    fprintf(2, "find: cannot open %s\n", dir);
    return;
  }

  if (fstat(fd, &st) < 0) {
    fprintf(2, "find: cannot stat %s\n", dir);
    close(fd);
    return;
  }

  switch (st.type) {
    case T_FILE:
      printf("%s\n", dir);
      break;

    case T_DIR:
      if (strlen(dir) + 1 + DIRSIZ > sizeof(buf)) {
        fprintf(2, "find: path too long\n");
        break;
      }
      strcpy(buf, dir);
      p = buf + strlen(buf);
      *p++ = '/';
      while (read(fd, &de, sizeof(de)) == sizeof(de)) {
        if (de.inum == 0) continue;
        if (strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0) continue;
        strcpy(p, de.name);
        if (stat(buf, &st) < 0) {
          fprintf(2, "find: cannot stat %s\n", buf);
          continue;
        }

        if (st.type == T_FILE && strcmp(de.name, filename) == 0) {
          printf("%s\n", buf);
        } else if (st.type == T_DIR) {
          find(buf, filename);
        }
      }
      break;
  }
  close(fd);
}

int main(int argc, char* argv[]) {
  if (argc != 3) {
    fprintf(2, "Usage: find directory filename\n");
    exit(1);
  }

  find(argv[1], argv[2]);
  exit(0);
}