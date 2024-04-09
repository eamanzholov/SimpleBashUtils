#include "fill_options.h"
#include "wrong_flags_detection.h"

flags options;

void print_tabs_and_num(int number) { printf("%6d\t", number); }

int is_tab(char c) { return (c == '\t'); }

int is_newline(char c) { return (c == '\n'); }

int no_flags() {
  return (options.b == 0 && options.e == 0 && options.n == 0 &&
          options.s == 0 && options.t == 0 && options.v == 0);
}

int is_empty_line(FILE *file) {
  char d;
  int empty;

  empty = 0;

  d = getc(file);
  if (is_newline(d)) empty = 1;
  ungetc(d, file);
  return empty;
}

void print_char(unsigned char c) {
  if (options.v) {
    if (is_tab(c) && !options.t)
      printf("%c", c);
    else if (c < 32) {
      c += 64;
      printf("^%c", c);
    } else if (c == 127) {
      printf("^?");
    } else if (c > 127 && c < 160) {
      c -= 64;
      printf("M-^%c", c);
    } else if (c >= 160 && c <= 192) {
      c -= 128;
      printf("M-%c", c);
    } else
      printf("%c", c);
  } else
    printf("%c", c);
}

void do_line(int *i, int *first_sym, unsigned char c) {
  if (no_flags()) {
    printf("%c", c);
  } else {
    if (*first_sym) {
      *first_sym = 0;
      if (options.b || options.n) {
        print_tabs_and_num(*i);
        (*i)++;
      }
      if (is_tab(c) && options.t)
        printf("^I");
      else {
        print_char(c);
      }
    } else {
      if (is_tab(c) && options.t)
        printf("^I");
      else {
        print_char(c);
      }
    }
  }
}

void do_end_of_line(int *i, int *first_sym, unsigned char c, FILE *file) {
  unsigned char d;

  if (*first_sym && options.s) {
    while (is_empty_line(file)) c = fgetc(file);
  }
  if (*first_sym && options.b) {
    d = getc(file);
    if (!is_newline(d)) {
    }
    ungetc(d, file);
  } else if (*first_sym && options.n) {
    print_tabs_and_num(*i);
    (*i)++;
  }
  if (options.e) {
    printf("$");
  }
  printf("%c", c);
}

void print_file(char *filename) {
  FILE *file;
  int i;
  int first_sym;
  char c;
  file = fopen(filename, "r");
  i = 1;
  c = fgetc(file);
  while (c != EOF) {
    first_sym = 1;
    while (c != EOF && !is_newline(c)) {
      do_line(&i, &first_sym, c);
      c = fgetc(file);
    }
    if (c != EOF) {
      do_end_of_line(&i, &first_sym, c, file);
      c = fgetc(file);
    }
  }
  fclose(file);
}

int file_exists(char *filename) {
  FILE *file;
  int exists;

  exists = 0;
  file = fopen(filename, "r");
  if (file) {
    fclose(file);
    exists = 1;
  }
  return exists;
}

void iterating_over_files(int argc, char **argv) {
  int i;

  i = 1;
  while (i < argc && argv[i][0] == '-') {
    i++;
  }
  while (i < argc) {
    if (!file_exists(argv[i])) {
      fprintf(stderr, "cat: %s: No such file or directory\n", argv[i]);
    } else {
      print_file(argv[i]);
    }
    i++;
  }
}

int start(int argc, char **argv) {
  if (find_illegal_flags(argc, argv)) {
    return 1;
  }
  fill_flags(argc, argv);
  iterating_over_files(argc, argv);
  return 0;
}

int main(int argc, char **argv) { start(argc, argv); }
