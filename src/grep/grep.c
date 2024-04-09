#include "grep.h"

int main(int argc, char **argv) { grep(argc, argv); }

void grep(int argc, char **argv) {
  if (argc > 1) {
    flags options = {0};
    char *pattern = NULL;

    for (int opt = fill_flags(argc, argv, &options, &pattern); opt < argc;
         opt++) {
      if (pattern && strlen(pattern) > 0) {
        do_file(argv[optind], options, pattern);
        optind++;
      }
    }
    if (pattern) {
      free(pattern);
    }
  }
}

int fill_flags(int argc, char **argv, flags *options, char **pattern) {
  char opt;
  while ((opt = getopt_long(argc, argv, "e:ivclnhsf:o", NULL, NULL)) != (-1)) {
    switch (opt) {
      case 'e':
        set_pattern_for_e(options, pattern, optarg);
        break;
      case 'i':
        options->i = 1;
        break;
      case 'v':
        options->v = 1;
        break;
      case 'c':
        options->c = 1;
        break;
      case 'l':
        options->l = 1;
        break;
      case 'n':
        options->n = 1;
        break;
      case 'h':
        options->h = 1;
        break;
      case 's':
        options->s = 1;
        break;
      case 'f':
        set_pattern_file_for_f(options, pattern, optarg);
        break;
      case 'o':
        options->o = 1;
        break;
    }
  }
  if (!there_is_flag_e(argc, argv) && !there_is_flag_f(argc, argv)) {
    read_pattern(pattern, argv[optind]);
    optind++;
  }
  count_files(options, optind, argc, argv);
  return optind;
}

int there_is_flag_f(int argc, char **argv) {
  int i = 0;
  size_t j;
  while (i < argc) {
    j = 1;
    if (argv[i][0] == '-') {
      while (j < strlen(argv[i])) {
        if (argv[i][j] == 'f') {
          return 1;
        }
        j++;
      }
    }
    i++;
  }
  return 0;
}

int there_is_flag_e(int argc, char **argv) {
  int i = 0;
  size_t j;
  while (i < argc) {
    j = 1;
    if (argv[i][0] == '-') {
      while (j < strlen(argv[i])) {
        if (argv[i][j] == 'e') {
          return 1;
        }
        j++;
      }
    }
    i++;
  }
  return 0;
}

void count_files(flags *options, int optind, int argc, char **argv) {
  int count;
  count = 0;
  while (optind < argc) {
    if (argv[optind]) {
      count++;
    }
    optind++;
  }
  options->count_files = count;
}

void set_pattern_for_e(flags *options, char **pattern, char *optarg) {
  *pattern = (char *)malloc(strlen(optarg) + 1);
  if (*pattern) {
    if (strcpy(*pattern, optarg)) {
      if (strcmp(*pattern, optarg) == 0) {
        options->e = 1;
      }
    } else {
      free(pattern);
    }
  }
}

void set_pattern_file_for_f(flags *options, char **pattern, char *optarg) {
  FILE *file;
  file = fopen(optarg, "r");
  if ((!file)) {
    fprintf(stderr, "grep: %s: No such file or directory\n", optarg);
  }
  if (file) {
    *pattern = (char *)malloc(strlen(optarg) + 1);
    if (*pattern) {
      if (strcpy(*pattern, optarg)) {
        if (strcmp(*pattern, optarg) == 0 && file) {
          options->f = 1;
        }
      } else {
        free(pattern);
      }
    }
    fclose(file);
  }
}

void read_pattern(char **pattern, char *arg) {
  if (strlen(arg) > 0) {
    *pattern = (char *)malloc(strlen(arg) + 1);
    if (*pattern) {
      strcpy(*pattern, arg);
    }
  }
}

void do_file(char *filename, flags options, char *pattern) {
  FILE *file;
  char *line = NULL;
  size_t len = 0;
  options.line_num = 1;
  int count_matching_lines = 0;

  file = fopen(filename, "r");
  if (!file && !options.s) {
    fprintf(stderr, "grep: %s: No such file or directory\n", filename);
  }
  if (file) {
    while (getline(&line, &len, file) != -1) {
      if (strlen(line) > 1 && line[strlen(line) - 1] == '\n') {
        line[strlen(line) - 1] = '\0';
      }
      do_line(pattern, line, options, &count_matching_lines, filename);
      options.line_num++;
    }
    if (options.c) {
      if (!options.h) {
        if (options.count_files > 1) {
          printf("%s:", filename);
        }
      }
      if (options.l) {
        if (count_matching_lines > 0) {
          printf("1\n");
        } else {
          printf("0\n");
        }
      } else {
        printf("%d\n", count_matching_lines);
      }
    }
    if (options.l) {
      if (count_matching_lines >= 1) {
        printf("%s\n", filename);
      }
    }
    free(line);
    fclose(file);
  }
}

int matches(char *string, char *pattern, flags options) {
  regex_t re;
  if (options.i) {
    if (regcomp(&re, pattern, REG_ICASE) != 0) {
      regfree(&re);
      return 0;
    }
  } else if (regcomp(&re, pattern, REG_EXTENDED) != 0) {
    regfree(&re);
    return 0;
  }
  int status = regexec(&re, string, 0, NULL, 0);
  // printf("pattern: %s\n", pattern);
  // printf("line: %s\n", string);
  // printf("status: %d\n", status);
  regfree(&re);
  if (status != 0) return 0;
  return 1;
}

void do_line(char *pattern, char *line, flags options,
             int *count_matching_lines, char *filename) {
  if (options.v) {
    if (options.f && matches_none_from_file(pattern, line, options)) {
      (*count_matching_lines)++;
      do_n_h_o(pattern, line, options, filename);
    } else if (options.o && matches(line, pattern, options)) {
    } else if (options.o && !matches(line, pattern, options)) {
      (*count_matching_lines)++;
      do_n_h_o(pattern, line, options, filename);
      if (!options.c && !options.l) {
        printf("%s", line);
        if (line[0] != '\n') {
          printf("\n");
        }
      }
    } else if (!options.f && !matches(line, pattern, options)) {
      (*count_matching_lines)++;
      do_n_h_o(pattern, line, options, filename);
    }
  } else {
    if (options.f && matches_any_from_file(pattern, line, options)) {
      (*count_matching_lines)++;
      do_n_h_o(pattern, line, options, filename);
    } else if (!options.f && matches(line, pattern, options)) {
      (*count_matching_lines)++;
      do_n_h_o(pattern, line, options, filename);
    }
  }
}

int matches_none_from_file(char *pattern_file, char *line, flags options) {
  FILE *file;
  file = fopen(pattern_file, "r");
  char *pattern_line = NULL;
  size_t len = 0;
  if (file) {
    while (getline(&pattern_line, &len, file) != -1) {
      if (strlen(pattern_line) > 1 &&
          pattern_line[strlen(pattern_line) - 1] == '\n') {
        pattern_line[strlen(pattern_line) - 1] = '\0';
      }
      if (strlen(line) > 1 && line[strlen(line) - 1] == '\n') {
        line[strlen(line) - 1] = '\0';
      }
      if (matches(line, pattern_line, options)) {
        fclose(file);
        free(pattern_line);
        return 0;
      }
    }
    free(pattern_line);
    fclose(file);
  }
  return 1;
}

int matches_any_from_file(char *pattern_file, char *line, flags options) {
  FILE *pfile;
  pfile = fopen(pattern_file, "r");
  char *pattern_line = NULL;
  size_t len = 0;

  if (pfile) {
    while (getline(&pattern_line, &len, pfile) != -1) {
      if (strlen(pattern_line) > 1 &&
          pattern_line[strlen(pattern_line) - 1] == '\n') {
        pattern_line[strlen(pattern_line) - 1] = '\0';
      }
      if (strlen(line) > 1 && line[strlen(line) - 1] == '\n') {
        line[strlen(line) - 1] = '\0';
      }
      if (matches(line, pattern_line, options)) {
        fclose(pfile);
        free(pattern_line);
        return 1;
      }
    }
    free(pattern_line);
    fclose(pfile);
  }
  return 0;
}

void do_n_h_o(char *pattern, char *line, flags options, char *filename) {
  if (!options.l && !options.c) {
    if (!options.h) {
      if (options.count_files > 1) {
        printf("%s:", filename);
      }
    }
    if (options.n) {
      printf("%d:", options.line_num);
    }
    if (options.o) {
      if (options.f) {
        do_o_f(pattern, line, options);
      } else if (!options.v) {
        do_o(pattern, line, options);
      }
    } else {
      printf("%s", line);
      if (line[0] != '\n') {
        printf("\n");
      }
    }
  }
}

void do_o_f(char *pattern_file, char *line, flags options) {
  FILE *pfile;
  pfile = fopen(pattern_file, "r");
  char *pattern_line = NULL;
  size_t len = 0;

  char *copy;
  regoff_t i;
  regex_t regex;
  regmatch_t match;
  regmatch_t closest_match;
  int rc;
  copy = line;

  if (pfile && matches_any_from_file(pattern_file, line, options)) {
    while (copy && matches_any_from_file(pattern_file, copy, options)) {
      closest_match.rm_eo = strlen(copy);
      closest_match.rm_so = strlen(copy);
      while (getline(&pattern_line, &len, pfile) != -1) {
        if (pattern_line[strlen(pattern_line) - 1] == '\n') {
          pattern_line[strlen(pattern_line) - 1] = '\0';
        }
        if (options.i) {
          rc = regcomp(&regex, pattern_line, REG_EXTENDED | REG_ICASE);
        } else {
          rc = regcomp(&regex, pattern_line, REG_EXTENDED);
        }
        if (rc != 0) {
          regfree(&regex);
        }
        rc = regexec(&regex, copy, 1, &match, 0);
        regfree(&regex);
        if (rc == 0 && match.rm_so < closest_match.rm_so) {
          closest_match.rm_eo = match.rm_eo;
          closest_match.rm_so = match.rm_so;
        }
        // if (match.rm_so == closest_match.rm_so &&
        //     match.rm_eo > closest_match.rm_eo) {
        //   closest_match.rm_eo = match.rm_eo;
        // }
      }
      rewind(pfile);
      i = closest_match.rm_so;
      while (i < closest_match.rm_eo) {
        printf("%c", copy[i]);
        i++;
      }
      printf("\n");
      copy = copy + closest_match.rm_eo;
    }
    free(pattern_line);
    fclose(pfile);
  }
}

char *strstr_regex(char *haystack, const char *pattern, flags options) {
  regex_t regex;
  regmatch_t match;
  int rc;
  if (options.i) {
    rc = regcomp(&regex, pattern, REG_EXTENDED | REG_ICASE);
  } else {
    rc = regcomp(&regex, pattern, REG_EXTENDED);
  }
  if (rc != 0) {
    regfree(&regex);
    return NULL;
  }
  rc = regexec(&regex, haystack, 1, &match, 0);
  regfree(&regex);
  if (rc == 0) {
    return haystack + match.rm_so;
  }
  return NULL;
}

void do_o(char *pattern, char *line, flags options) {
  char *copy;
  regoff_t i;
  regex_t regex;
  regmatch_t match;
  int rc;
  copy = line;

  while (copy && strstr_regex(copy, pattern, options)) {
    if (options.i) {
      rc = regcomp(&regex, pattern, REG_EXTENDED | REG_ICASE);
    } else {
      rc = regcomp(&regex, pattern, REG_EXTENDED);
    }
    if (rc != 0) {
      regfree(&regex);
    }
    rc = regexec(&regex, copy, 1, &match, 0);
    regfree(&regex);
    i = match.rm_so;
    while (i < match.rm_eo) {
      printf("%c", copy[i]);
      i++;
    }
    printf("\n");
    copy = copy + match.rm_eo;
  }
}

void do_o_v(char *pattern, char *line, flags options) {
  char *copy;
  regoff_t i;
  regex_t regex;
  regmatch_t match;
  int rc;

  copy = line;
  while (copy && matches(copy, pattern, options)) {
    if (options.i) {
      rc = regcomp(&regex, pattern, REG_EXTENDED | REG_ICASE);
    } else {
      rc = regcomp(&regex, pattern, REG_EXTENDED);
    }
    if (rc != 0) {
      regfree(&regex);
    }
    rc = regexec(&regex, copy, 1, &match, 0);
    regfree(&regex);
    if (rc == 0) {
      i = 0;
      while (i < match.rm_so) {
        printf("%c", copy[i]);
        i++;
      }
      copy = copy + match.rm_eo;
    }
  }
  if (matches(line, pattern, options)) {
    printf("\n");
  }
}
