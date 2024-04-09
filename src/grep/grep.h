#ifndef GREP_H_
#define GREP_H_

#define _GNU_SOURCE
#include <getopt.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef struct {
  int e;
  int i;
  int v;
  int c;
  int l;
  int n;
  int h;
  int s;
  int f;
  int o;
  int count_files;
  int line_num;
} flags;

void grep(int argc, char **argv);
int fill_flags(int argc, char **argv, flags *options, char **pattern);
void do_file(char *filename, flags options, char *pattern);
void do_line(char *pattern, char *line, flags options, int *count,
             char *filename);
int match(char *string, char *pattern, flags options);
void set_pattern_for_e(flags *options, char **pattern, char *optarg);
void read_pattern(char **pattern, char *arg);
void do_n_h_o(char *pattern, char *line, flags options, char *filename);
void do_o_f(char *pattern_file, char *line, flags options);
void do_o(char *pattern, char *line, flags options);
void do_o_v(char *pattern, char *line, flags options);
void count_files(flags *options, int optind, int argc, char **argv);
void set_pattern_file_for_f(flags *options, char **pattern, char *optarg);
int matches_any_from_file(char *pattern_file, char *line, flags options);
int matches_none_from_file(char *pattern_file, char *line, flags options);
int there_is_flag_f(int argc, char **argv);
int there_is_flag_e(int argc, char **argv);
char *strstr_regex(char *haystack, const char *pattern, flags options);
#endif