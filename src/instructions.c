#include "cpu.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

void jmp_abs(CPU* cpu,unsigned char* buffer)
{
  printf("JUMP\n");
  uint16_t pc = (*cpu).PC;
  (*cpu).PC=((buffer[pc+2]) << 8) | (buffer[pc+1]);
  (*cpu).cycles += 3;
}

void loadx_imm(CPU* cpu, unsigned char* buffer)
{
  printf("LOADX_IMM\n");
  uint16_t pc = (*cpu).PC;
  (*cpu).X = buffer[pc+1];
  (*cpu).PC += 2;
  (*cpu).cycles += 2;
}

void storex_zp(CPU* cpu, unsigned char* buffer)
{
	uint8_t address = buffer[(*cpu).PC + 1];
	(*cpu).memory[address] = (*cpu).X;
	(*cpu).PC += 2;
	(*cpu).cycles += 3;
	printf("STOREX_ZP\n");
}

void jsr(CPU* cpu, unsigned char* buffer)
{
	uint8_t nextPC = (*cpu).PC + 2;
	uint16_t pc = (*cpu).PC;
	printf("JSR");
	(*cpu).memory[(*cpu).S] = nextPC & 0xFF00;
	(*cpu).memory[(*cpu).S - 1] = nextPC & 0x00FF;
	(*cpu).S = (*cpu).S - 2;
	(*cpu).PC = ((buffer[pc + 2]) << 8) | (buffer[pc + 1]);
	(*cpu).cycles += 6;
}

void nop(CPU* cpu, unsigned char* buffer) 
{
	printf("NOEP\n");
	(*cpu).PC += 1;
	(*cpu).cycles += 2;
}

void sec(CPU* cpu, unsigned char* buffer)
{
	printf("SEC\n");
	(*cpu).P[7] = 1;
	(*cpu).PC += 1;
	(*cpu).cycles += 2;
}

void bcs(CPU* cpu, unsigned char* buffer)
{
	printf("BCS\n");
	uint8_t displacement = buffer[(*cpu).PC + 1];
	if ((*cpu).P[7] == 1)
	{

		if (( (displacement + 2 + 0xFF & (*cpu).PC)) < (0xFF & (*cpu).PC))
		{
			(*cpu).cycles += 4;
		}
		else
		{
			(*cpu).cycles += 3;
		}
		(*cpu).PC += displacement + 2;
	}
	else
	{
		(*cpu).PC += 2;
		(*cpu).cycles += 2;
	}
}