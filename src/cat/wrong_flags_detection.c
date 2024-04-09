#include "wrong_flags_detection.h"

int str_len(char *str) {
  int i;

  i = 0;
  while (str[i]) i++;
  return i;
}

int are_equal(char *str1, char *str2) {
  int i;

  i = 0;
  if (str_len(str1) != str_len(str2)) return 0;
  while (str1[i]) {
    if (str1[i] != str2[i]) return 0;
    i++;
  }
  return 1;
}

int is_flag(char c) {
  char *flags = "benstvET";
  int i;
  int found;

  i = 0;
  found = 0;
  while (flags[i]) {
    if (c == flags[i]) {
      found = 1;
    }
    i++;
  }
  return found;
}

int is_gnu_flag(char *arg) {
  return (are_equal(arg, "--number-nonblank") || are_equal(arg, "--number") ||
          are_equal(arg, "--squeeze-blank"));
}

int find_not_flag(char *arg, char *wrong_flag) {
  int i;
  int found_illegal;

  i = 1;
  found_illegal = 0;
  while (arg[i]) {
    if (!is_flag(arg[i])) {
      found_illegal = 1;
      *wrong_flag = arg[i];
    }
    i++;
  }
  if (is_gnu_flag(arg)) found_illegal = 0;
  return found_illegal;
}

int find_illegal_flags(int argc, char **argv) {
  int i;

  i = 1;
  char wrong_flag;

  while (i < argc && argv[i][0] == '-') {
    // for GNU
    if (argv[i][1] == '-' && find_not_flag(argv[i], &wrong_flag)) {
      fprintf(stderr, "cat: illegal option -- %s\n", argv[i]);
      return 1;
    }
    // for SIMPLE flags
    else if (find_not_flag(argv[i], &wrong_flag)) {
      fprintf(stderr, "cat: illegal option -- %c\n", wrong_flag);
      fprintf(stderr, "usage: cat [-benstuv] [file ...]\n");
      return 1;
    }
    i++;
  }

  return 0;
}