#include "cpu.h"
#include "instructions.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

void initiate_cpu(CPU *cpu)
{
#ifdef NESTEST
	(*cpu).PC=0xC000;
#endif
	(*cpu).S = 0xFD;
	(*cpu).A, (*cpu).X, (*cpu).Y = 0;
	(*cpu).P[2] = 1;
#ifndef NESTEST
	(*cpu).P[4] = 1;
#endif
	(*cpu).P[5] = 1;
	(*cpu).cycles = 0;
}

uint8_t get_status_flag(uint8_t pflag[])
{
  int i = 0;
  uint8_t status = 0;
  for(i = 0; i < 8; i++)
  {
    status |= pflag[i] << i;
  }
  return status;
}

void get_cpu_state(CPU *cpu)
{
  printf("A:%x X:%x Y:%x P:%x SP:%x CYC:%d\n",(*cpu).A,(*cpu).X,(*cpu).Y,get_status_flag((*cpu).P),(*cpu).S, (*cpu).cycles*3);
}

void debug_inst(CPU *cpu, unsigned char *buffer)
{
  char* instruction = inst_name[buffer[(*cpu).PC]];
  printf("%x ",(*cpu).PC);
  switch(addressing_mode[buffer[(*cpu).PC]])
  {
    case 0:
	printf("%s \t",instruction);
	get_cpu_state(cpu);
	break;
    case 1:
        printf("%s #%x\t",instruction, buffer[(*cpu).PC+1]);
	get_cpu_state(cpu);
	break;
    case 2:
	printf("%s $%x\t",instruction, buffer[(*cpu).PC+1]);
	get_cpu_state(cpu);
	break;
    case 3:
        printf("%s $%x,X\t",instruction,buffer[(*cpu).PC+1]);
	get_cpu_state(cpu);
	break;
    case 4:
        printf("%s $%x,Y\t",instruction,buffer[(*cpu).PC+1]);
	get_cpu_state(cpu);
	break;
    case 5:
        printf("%s ($%x),X\t",instruction,buffer[(*cpu).PC+1]);
	get_cpu_state(cpu);
	break;
    case 6:
        printf("%s ($%x),Y\t",instruction,buffer[(*cpu).PC+1]);
	get_cpu_state(cpu);
	break;
    case 7:
        printf("%s $%x%x\t",instruction,buffer[(*cpu).PC+2],buffer[(*cpu).PC+1]);
	get_cpu_state(cpu);
	break;
    case 8:
        printf("%s $%x%x,X\t",instruction,buffer[(*cpu).PC+2],buffer[(*cpu).PC+1]);
	get_cpu_state(cpu);
	break;
    case 9:
        printf("%s $%x%x,Y\t",instruction,buffer[(*cpu).PC+2],buffer[(*cpu).PC+1]);
	get_cpu_state(cpu);
	break;
    case 10:
        printf("%s $(%x%x)\t",instruction,buffer[(*cpu).PC+2],buffer[(*cpu).PC+1]);
	get_cpu_state(cpu);
	break;
    case 11:
        printf("%s %x\t",instruction,buffer[(*cpu).PC+1]);
	get_cpu_state(cpu);
	break;
  }
}

void decode_and_execute(CPU *cpu, unsigned char *buffer)
{
     debug_inst(cpu, buffer);
     uint8_t instOp = buffer[(*cpu).PC];
     instruction[instOp](cpu,buffer);
}

int enter_cpu(unsigned char *buffer)
{
   CPU cpu;
   initiate_cpu(&cpu);
   int i = 0;
   while(i < 220)
   {
     decode_and_execute(&cpu,buffer);
     i++;
   }
   return 0;
}
