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
}

void decode_and_execute(CPU *cpu, unsigned char *buffer)
{
     uint8_t instOp = buffer[(*cpu).PC];
     instruction[instOp](cpu,buffer);
     printf("HEY %x\n",(*cpu).PC);
}

int enter_cpu(unsigned char *buffer)
{
   CPU cpu;
   initiate_cpu(&cpu);
   int i = 0;
   while(i < 3)
   {
     decode_and_execute(&cpu,buffer);
     i++;
   }
   return 0;
}
