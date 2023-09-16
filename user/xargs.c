#include "kernel/param.h"
#include "kernel/types.h"
#include "user/user.h"

char* string_append(char* str, char c, int cap, int* new_cap) {
  if (str == 0) {
    *new_cap = 32;
    str = malloc(sizeof(char) * (*new_cap));
    str[0] = c;
    str[1] = 0;
    return str;
  } else if (strlen(str) <= cap - 2) {
    int l = strlen(str);
    str[l] = c;
    str[l + 1] = 0;
    *new_cap = cap;
    return str;
  } else {
    *new_cap = cap * 2;
    char* new_str = malloc(sizeof(char) * (*new_cap));
    strcpy(new_str, str);
    free(str);
    int l = strlen(new_str);
    new_str[l] = c;
    new_str[l + 1] = 0;
    return new_str;
  }
}

char** read_args(int argc, char* argv[], int* have_new_args) {
  char** args = malloc(sizeof(char*) * MAXARG);
  for (int i = 0; i < MAXARG; i++) args[i] = 0;
  for (int i = 1; i < argc; i++) args[i - 1] = argv[i];

  int i = argc - 1, cap = 0;

  do {
    char c;
    int n = read(0, &c, 1);
    if (n == 0 || c == '\n') break;
    if (c == ' ') {
      if (args[i] != 0) {
        i += 1;
        cap = 0;
      }
    } else {
      args[i] = string_append(args[i], c, cap, &cap);
    }
  } while (1);

  if (args[i] != 0) i += 1;
  *have_new_args = (i > argc - 1);
  return args;
}

void free_args(int argc, char** args) {
  for (int i = argc - 1; i < MAXARG; i++) {
    if (args[i] != 0) {
      free(args[i]);
    } else {
      return;
    }
  }
}

int main(int argc, char* argv[]) {
  while (1) {
    int have_new_args;
    char** args = read_args(argc, argv, &have_new_args);
    if (have_new_args) {
      int pid = fork();
      if (pid == 0) {
        exec(argv[1], args);
      } else {
        wait(0);
      }
    }
    free_args(argc, args);
    if (!have_new_args) break;
  }

  exit(0);
}