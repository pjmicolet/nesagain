#include "romfile.h"
#include <stdlib.h>
#include <stdio.h>
#include<string.h>

void readHeader(FILE *fp, int info[], int* mirroring)
{
  size_t result;
  //Create buffer for the header
  char* headerBuffer = (char*) malloc(sizeof(char)*16);
  result = fread (headerBuffer,1,16,fp);
  info[0] = headerBuffer[4] * 16384;
  info[1] = headerBuffer[5] * 8192;
  printf("Trainer ? %x\n", headerBuffer[6] & 0x4);
  printf("Size of PRG ROM %d\n",headerBuffer[4]*16384);
  printf("Size of CHR ROM %d\n",headerBuffer[5]*8192);
  printf("Size of PRG RAM %d\n", headerBuffer[8] * 16 * 1000);
  printf("Mapper Number %x\n", (headerBuffer[7] & 0xF0) | ((headerBuffer[6] & 0xF0) >> 4));
  printf("Nametable mirroring%x\n", (headerBuffer[6] & 0x0F));
  if ((!(headerBuffer[6] & 0x08)) && (!(headerBuffer[6] & 0x1)))
  {
	  *mirroring = 2;
  }
  else if ((!(headerBuffer[6] & 0x08)) && (headerBuffer[6] & 0x1))
  {
	  *mirroring = 1;
  }

}

void loadRom(unsigned char **buffer, unsigned char** chrbuffer, size_t result[], int* mirroring)
{
  FILE *fp;
  long fileSize;
  long prg_rom;
  long chr_rom;
  int info[8];


//  fp = fopen("../NEStress/NEStress.NES", "rb");
 //fp = fopen("../full_palette/full_palette.nes", "rb");
   fp = fopen("../rom/Excitebike.nes", "rb");
// fp = fopen("./nestest.nes", "rb");
//  fp = fopen("../cpu_interrupts_v2/rom_singles/2-nmi_and_brk.nes", "rb");
  readHeader(fp,info,mirroring);
  fseek(fp,0,SEEK_END);
  fileSize = ftell (fp) - 16;
  rewind(fp);
  fseek(fp,16,SEEK_SET);
  *(buffer) = (char*)malloc((sizeof(char)*info[0]));
  *(chrbuffer) = (char*)malloc((sizeof(char)*info[1]));
  result[0] = fread (*(buffer),1,info[0],fp);
  if (info[1] > 0) {
	  fseek(fp, info[0] + 16, SEEK_SET);
	  result[1] = fread(*(chrbuffer), 1, info[1], fp);
  }
  else {
	  printf("NO CHR\n");
	  result[1] = 0;
  }
  printf("Result %d %d and %d \n",info[0], info[1], fileSize*sizeof(char)+sizeof(char)*0xC000);

  fclose(fp);
}

