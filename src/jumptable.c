#include <stdio.h>
#include <stdlib.h>

typedef void (*Handler)(void);

void func3 (void) {printf("3\n");}
void func2 (void) {printf("2\n");}
void func1 (void) {printf("1\n");}
void func0 (void) {printf("0\n");}


Handler jump_table[4] = {func0, func1, func2, func3};

int main(int argc, char **argv)
{
  int value;
  value = atoi(argv[1]) % 4;

  jump_table[value]();
}
