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
	(*cpu).P[6] = 1;
	(*cpu).cycles = 0;
}

void decode_and_execute(CPU *cpu, unsigned char *buffer)
{
     uint8_t instOp = buffer[(*cpu).PC];
	 printf("%x\n", instOp);
     instruction[instOp](cpu,buffer);
}

int enter_cpu(unsigned char *buffer)
{
   CPU cpu;
   initiate_cpu(&cpu);
   int i = 0;
   while(i < 10)
   {
     decode_and_execute(&cpu,buffer);
     i++;
   }
   return 0;
}
