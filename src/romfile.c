#include "romfile.h"
#include <stdlib.h>
#include <stdio.h>
#include<string.h>
void readHeader(FILE *fp)
{
  size_t result;
  //Create buffer for the header
  char* headerBuffer = (char*) malloc(sizeof(char)*16);
  result = fread (headerBuffer,1,16,fp);
  printf("Size of PGR ROM %d\n",headerBuffer[4]*16*1000);
  printf("Size of CHR ROM %d\n",headerBuffer[5]*16*1000);
}

int loadRom(unsigned char **buffer)
{
  FILE *fp;
  long fileSize;
  size_t result;

  fp = fopen("nestest.nes", "rb");
  readHeader(fp);
  fseek(fp,0,SEEK_END);
  fileSize = ftell (fp) - 16;
  rewind(fp);
  fseek(fp,16,SEEK_SET);
#ifdef NESTEST
  char *buff2 = (char*) malloc(sizeof(char)*fileSize);
  *(buffer) = (char*) malloc((sizeof(char)*fileSize)+(sizeof(char)*0xC000));
  result = fread (buff2,1,fileSize,fp);
  printf("Buffer 2 yeah %x\n",buff2[0]);
  printf("Result %d %d and %d \n",fileSize, result, fileSize*sizeof(char)+sizeof(char)*0xC000);
  memcpy(&(*buffer)[0xC000],buff2,result);
#else
  *(buffer) = (char*) malloc(sizeof(char)*fileSize);
  result = fread (*(buffer),1,fileSize,fp);
#endif
  fclose(fp);
  return result;
}

