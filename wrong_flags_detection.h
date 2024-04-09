#ifndef WRONG_FLAGS_DETECTION_H
#define WRONG_FLAGS_DETECTION_H

#include <stdio.h>
int is_flag(char c);
int is_gnu_flag(char *arg);
int find_not_flag(char *arg, char *wrong_flag);
int find_illegal_flags(int argc, char **argv);

#endif