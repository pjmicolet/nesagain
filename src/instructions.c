#include "cpu.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

void jmp_abs(CPU* cpu,unsigned char* buffer)
{
  printf("JUMP\n");
  uint16_t pc = (*cpu).PC;
  (*cpu).PC=((buffer[pc+2]) << 8) | (buffer[pc+1]);
}

void loadx_imm(CPU* cpu, unsigned char* buffer)
{
  printf("HEY\n");
  uint16_t pc = (*cpu).PC;
  (*cpu).X = buffer[pc+1];
  (*cpu).PC += 2;
}

