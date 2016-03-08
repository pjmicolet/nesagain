#include "romfile.h"

#include<stdio.h>
#include<stdlib.h>
int main(int argc, char **argv)
{
  unsigned char* buffer;
  int size = loadRom(&buffer);
  printf("Size %d\n",size);
  printf("OK %x\n",buffer[0xC000]);
  enter_cpu(buffer);
  free(buffer);
  return 0;
}
