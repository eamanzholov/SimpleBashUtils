#ifndef FILL_OPTIONS_H_
#define FILL_OPTIONS_H_

typedef struct {
  int b;
  int e;
  int n;
  int s;
  int t;
  int v;
} flags;

extern flags options;

void flags_init();
void fill_each_letter(char *arg);
void fill_flags(int argc, char **argv);
int are_equal(char *str1, char *str2);

#endif