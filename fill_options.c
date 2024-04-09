#include "fill_options.h"

#include "wrong_flags_detection.h"

void flags_init() {
  options.b = 0;
  options.e = 0;
  options.n = 0;
  options.s = 0;
  options.t = 0;
  options.v = 0;
}

void fill_each_letter(char *arg) {
  int i;

  i = 1;
  while (arg[i]) {
    switch (arg[i]) {
      case 'b':
        options.b = 1;
        break;
      case 'e':
        options.e = 1;
        options.v = 1;
        break;
      case 'n':
        options.n = 1;
        break;
      case 's':
        options.s = 1;
        break;
      case 't':
        options.t = 1;
        options.v = 1;
        break;
      case 'v':
        options.v = 1;
        break;
      case 'E':
        options.e = 1;
        break;
      case 'T':
        options.t = 1;
        break;
    }
    i++;
  }
}

void fill_gnu_flags(char *arg) {
  if (are_equal(arg, "--number")) {
    options.n = 1;
  }
  if (are_equal(arg, "--squeeze-blank")) {
    options.s = 1;
  }
  if (are_equal(arg, "--number-nonblank")) {
    options.b = 1;
  }
}

void fill_flags(int argc, char **argv) {
  int i;

  i = 1;
  flags_init();
  while (i < argc && argv[i][0] == '-') {
    if (is_gnu_flag(argv[i])) {
      fill_gnu_flags(argv[i]);
    } else {
      fill_each_letter(argv[i]);
    }
    i++;
  }
}
