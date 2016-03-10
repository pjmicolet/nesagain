#include "cpu.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#define overflow(A,B,C) (((~(A^B)) & (B^C)&0x80) ? 1 : 0)  

void jmp_abs(CPU* cpu,unsigned char* buffer)
{
  uint16_t pc = (*cpu).PC;
  (*cpu).PC=((buffer[pc+2]) << 8) | (buffer[pc+1]);
  (*cpu).cycles += 3;
}

void loadx_imm(CPU* cpu, unsigned char* buffer)
{
  uint16_t pc = (*cpu).PC;
  (*cpu).P[1]= (buffer[pc+1] == 0) ? 1 :0 ;
  (*cpu).X = buffer[pc+1];
  (*cpu).PC += 2;
  (*cpu).cycles += 2;
}

void lda_imm(CPU* cpu, unsigned char* buffer)
{
  uint16_t pc = (*cpu).PC;
  (*cpu).P[1]= (buffer[pc+1] == 0) ? 1 :0 ;
  (*cpu).P[7]= (buffer[pc+1] & 0x80) ? 1 : 0;
  (*cpu).A = buffer[pc+1];
  (*cpu).PC += 2;
  (*cpu).cycles += 2;
}

void storex_zp(CPU* cpu, unsigned char* buffer)
{
	uint8_t address = buffer[(*cpu).PC + 1];
	(*cpu).memory[address] = (*cpu).X;
	(*cpu).PC += 2;
	(*cpu).cycles += 3;
}

void sta_zp(CPU* cpu, unsigned char* buffer)
{
	uint8_t address = buffer[(*cpu).PC + 1];
	(*cpu).memory[address] = (*cpu).A;
	(*cpu).PC += 2;
	(*cpu).cycles += 3;
}

void jsr(CPU* cpu, unsigned char* buffer)
{
	uint16_t nextPC = (*cpu).PC + 2;
	uint16_t pc = (*cpu).PC;
	(*cpu).memory[(*cpu).S] = (uint8_t)((nextPC & 0xFF00) >> 8);
	(*cpu).memory[(*cpu).S - 1] = nextPC & 0x00FF;
	(*cpu).S = (*cpu).S - 2;
	(*cpu).PC = ((buffer[pc + 2]) << 8) | (buffer[pc + 1]);
	(*cpu).cycles += 6;
}

void nop(CPU* cpu, unsigned char* buffer) 
{
	(*cpu).PC += 1;
	(*cpu).cycles += 2;
}

void sec(CPU* cpu, unsigned char* buffer)
{
	(*cpu).P[0] = 1;
	(*cpu).PC += 1;
	(*cpu).cycles += 2;
}

void sei(CPU* cpu, unsigned char* buffer)
{
	(*cpu).P[2] = 1;
	(*cpu).PC += 1;
	(*cpu).cycles += 2;
}

void sed(CPU* cpu, unsigned char* buffer)
{
	(*cpu).P[3] = 1;
	(*cpu).PC += 1;
	(*cpu).cycles += 2;
}

void clc(CPU* cpu, unsigned char* buffer)
{
	(*cpu).P[0] = 0;
	(*cpu).PC += 1;
	(*cpu).cycles += 2;
}

void bcs(CPU* cpu, unsigned char* buffer)
{
	uint8_t displacement = buffer[(*cpu).PC + 1];
	if ((*cpu).P[0] == 1)
	{

		if (( (displacement + 2 + 0xFF & (*cpu).PC)) < (0xFF & (*cpu).PC))
		{
			(*cpu).cycles += 5;
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

void bcc(CPU* cpu, unsigned char* buffer)
{
	uint8_t displacement = buffer[(*cpu).PC + 1];
	if ((*cpu).P[0] == 0)
	{

		if (( (displacement + 2 + 0xFF & (*cpu).PC)) < (0xFF & (*cpu).PC))
		{
			(*cpu).cycles += 5;
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

void beq(CPU* cpu, unsigned char* buffer)
{
	uint8_t displacement = buffer[(*cpu).PC + 1];
	if ((*cpu).P[1] == 1)
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

void bne(CPU* cpu, unsigned char* buffer)
{
	uint8_t displacement = buffer[(*cpu).PC + 1];
	if ((*cpu).P[1] == 0)
	{

		if (( (displacement + 2 + 0xFF & (*cpu).PC)) < (0xFF & (*cpu).PC))
		{
			(*cpu).cycles += 5;
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

void bit_zp(CPU* cpu, unsigned char* buffer)
{
        uint8_t address = buffer[(*cpu).PC +1];
	uint8_t mem_and = (*cpu).memory[address] & (*cpu).A;
	(*cpu).P[1] = (mem_and == 0) ? 1 : 0;
	(*cpu).P[6] = ((*cpu).memory[address] & 0x40) ? 1 : 0;
	(*cpu).P[7] = ((*cpu).memory[address] & 0x80) ? 1 : 0;
	(*cpu).cycles += 3;
	(*cpu).PC += 2;
}

void bvs(CPU* cpu, unsigned char* buffer)
{
	uint8_t displacement = buffer[(*cpu).PC + 1];
	if ((*cpu).P[6] == 1)
	{

		if (( (displacement + 2 + 0xFF & (*cpu).PC)) < (0xFF & (*cpu).PC))
		{
			(*cpu).cycles += 5;
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

void bmi(CPU* cpu, unsigned char* buffer)
{
	uint8_t displacement = buffer[(*cpu).PC + 1];
	if ((*cpu).P[7] == 1)
	{

		if (( (displacement + 2 + 0xFF & (*cpu).PC)) < (0xFF & (*cpu).PC))
		{
			(*cpu).cycles += 5;
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

void bvc(CPU* cpu, unsigned char* buffer)
{
	uint8_t displacement = buffer[(*cpu).PC + 1];
	if ((*cpu).P[6] == 0)
	{

		if (( (displacement + 2 + 0xFF & (*cpu).PC)) < (0xFF & (*cpu).PC))
		{
			(*cpu).cycles += 5;
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

void bpl(CPU* cpu, unsigned char* buffer)
{
	uint8_t displacement = buffer[(*cpu).PC + 1];
	if ((*cpu).P[7] == 0)
	{

		if (( (displacement + 2 + 0xFF & (*cpu).PC)) < (0xFF & (*cpu).PC))
		{
			(*cpu).cycles += 5;
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

void rts(CPU* cpu, unsigned char* buffer)
{
        uint8_t lower = (*cpu).memory[(*cpu).S+1];
        uint8_t high = (*cpu).memory[(*cpu).S+2];
        uint16_t pc = high << 8 | lower;
        (*cpu).PC = pc + 1;
        (*cpu).cycles += 6;
	(*cpu).S += 2;
}

void php(CPU* cpu, unsigned char* buffer)
{
	int i = 0;
	uint8_t status = 0;
	for(i = 0; i < 8; i++)
	{
	  status |= (*cpu).P[i] << i;
	}
	(*cpu).memory[(*cpu).S] = status | 0x1 << 4;
	(*cpu).cycles += 3;
	(*cpu).S -= 1;
	(*cpu).PC += 1;
}

void pha(CPU* cpu, unsigned char* buffer)
{
	(*cpu).memory[(*cpu).S] = (*cpu).A;
	(*cpu).cycles += 3;
	(*cpu).S -= 1;
	(*cpu).PC += 1;
}

void pla(CPU* cpu, unsigned char* buffer)
{
	(*cpu).A = (*cpu).memory[(*cpu).S+1];
	(*cpu).P[1]= ((*cpu).A == 0) ? 1 :0 ;
	(*cpu).P[7]= ((*cpu).A & 0x80) ? 1 : 0;
	(*cpu).cycles += 4;
	(*cpu).S += 1;
	(*cpu).PC += 1;
}

void plp(CPU* cpu, unsigned char* buffer)
{
	uint8_t status = (*cpu).memory[(*cpu).S+1];
	int i = 0;
	for(i = 0; i < 8; i++)
	{
	  (*cpu).P[i] = ((status) >> i ) & 0x1;
	}
	// Because it doesn't exist or something
	(*cpu).P[4] = 0;
	(*cpu).P[5] = 1;
	(*cpu).cycles += 4;
	(*cpu).S += 1;
	(*cpu).PC += 1;
}

void and_imm(CPU* cpu, unsigned char* buffer)
{
	(*cpu).A &= buffer[(*cpu).PC+1];
	(*cpu).P[1]= ((*cpu).A == 0) ? 1 :0 ;
	(*cpu).P[7]= ((*cpu).A & 0x80) ? 1 : 0;
	(*cpu).PC += 2;
	(*cpu).cycles += 2;
}

void ora_imm(CPU* cpu, unsigned char* buffer)
{
	(*cpu).A |= buffer[(*cpu).PC+1];
	(*cpu).P[1]= ((*cpu).A == 0) ? 1 :0 ;
	(*cpu).P[7]= ((*cpu).A & 0x80) ? 1 : 0;
	(*cpu).PC += 2;
	(*cpu).cycles += 2;
}

void eor_imm(CPU* cpu, unsigned char* buffer)
{
	(*cpu).A ^= buffer[(*cpu).PC+1];
	(*cpu).P[1]= ((*cpu).A == 0) ? 1 :0 ;
	(*cpu).P[7]= ((*cpu).A & 0x80) ? 1 : 0;
	(*cpu).PC += 2;
	(*cpu).cycles += 2;
}

void cmp_imm(CPU* cpu, unsigned char* buffer)
{
        uint8_t comp = (*cpu).A - buffer[(*cpu).PC+1];
	(*cpu).P[0] = ((*cpu).A >= buffer[(*cpu).PC+1]) ? 1 : 0;
	(*cpu).P[1] = ((*cpu).A == buffer[(*cpu).PC+1]) ? 1 : 0;
	(*cpu).P[7] = (comp & 0x80) ? 1 : 0;
	(*cpu).cycles += 2;
	(*cpu).PC += 2;
}

void cld(CPU *cpu, unsigned char* buffer)
{
        (*cpu).P[3] = 0;
	(*cpu).PC += 1;
	(*cpu).cycles += 2;
}

void clv(CPU *cpu, unsigned char* buffer)
{
        (*cpu).P[6] = 0;
	(*cpu).PC += 1;
	(*cpu).cycles += 2;
}

void adc_imm(CPU *cpu, unsigned char* buffer)
{
        uint8_t imm = buffer[(*cpu).PC + 1];
	uint16_t temp = imm + (*cpu).A + (*cpu).P[0];
	(*cpu).P[1] = ((temp & 0xFF )== 0) ? 1 : 0;
	(*cpu).P[7] = (temp & 0x80) ? 1 : 0;
	(*cpu).P[6] = overflow((*cpu).A,imm,temp);
	(*cpu).P[0] = (temp & 0x100) ? 1 : 0;
	(*cpu).A = (uint8_t)(temp & 0x0FF);
	(*cpu).PC+=2;
	(*cpu).cycles +=2;
}

void ldy_imm(CPU *cpu, unsigned char* buffer)
{
	uint16_t pc = (*cpu).PC;
	(*cpu).P[1] = (buffer[pc + 1] == 0) ? 1 : 0;
	(*cpu).P[7] = (buffer[pc + 1] & 0x80) ? 1 : 0;
	(*cpu).Y = buffer[pc + 1];
	(*cpu).PC += 2;
	(*cpu).cycles += 2;
}

void cpy_imm(CPU* cpu, unsigned char* buffer)
{
	uint8_t comp = (*cpu).Y - buffer[(*cpu).PC + 1];
	(*cpu).P[0] = ((*cpu).Y >= buffer[(*cpu).PC + 1]) ? 1 : 0;
	(*cpu).P[1] = ((*cpu).Y == buffer[(*cpu).PC + 1]) ? 1 : 0;
	(*cpu).P[7] = (comp & 0x80) ? 1 : 0;
	(*cpu).cycles += 2;
	(*cpu).PC += 2;
}

void cpx_imm(CPU* cpu, unsigned char* buffer)
{
	uint8_t comp = (*cpu).X - buffer[(*cpu).PC + 1];
	(*cpu).P[0] = ((*cpu).X >= buffer[(*cpu).PC + 1]) ? 1 : 0;
	(*cpu).P[1] = ((*cpu).X == buffer[(*cpu).PC + 1]) ? 1 : 0;
	(*cpu).P[7] = (comp & 0x80) ? 1 : 0;
	(*cpu).cycles += 2;
	(*cpu).PC += 2;
}