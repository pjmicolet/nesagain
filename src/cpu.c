#include "cpu.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>



#define overflow(A,B,C) (~((A^B) & 0x80) & ((A^C)&0x80))
#define overflowsbc(A,B,C) (((A^B) & 0x80) && ((A^C)&0x80))

uint16_t fetch_data(CPU *cpu, unsigned char* buffer)
{
	switch (addressing_mode[buffer[cpu->PC]])
	{
	//IMPLIED
	case 0:
		return 0;
	//IMMEDIATE
	case 1:
		return buffer[cpu->PC + 1];
	//ZP
	case 2:
		cpu->cycles++;
		return cpu->memory[buffer[cpu->PC + 1]];
	//ZP X
	case 3:
		cpu->cycles += 2;
		return cpu->memory[(uint8_t)(buffer[cpu->PC + 1] + cpu->X)];
	//ZP Y
	case 4:
		cpu->cycles += 2;
		return cpu->memory[(uint8_t)(buffer[cpu->PC + 1] + cpu->Y)];
	//IND X
	case 5:
		cpu->cycles += 4;
		return cpu->memory[((uint16_t)(cpu->memory[(buffer[cpu->PC + 1] + cpu->X + 1)]) << 8) | cpu->memory[(buffer[cpu->PC + 1] + cpu->X)]];
	//IND Y
	case 6:
		if (((uint16_t)(cpu->memory[buffer[cpu->PC + 1] ])+ cpu->Y) & 0x100)
		{
			cpu->cycles += 4;
		}
		else
		{
			cpu->cycles += 3;
		}
		return cpu->memory[cpu->memory[(((uint16_t)(cpu->memory[(buffer[cpu->PC + 2])]) << 8) | cpu->memory[buffer[cpu->PC + 1]]) + cpu->Y]];
	//ABS
	case 7:
		cpu->cycles += 2;
		return cpu->memory[((buffer[cpu->PC + 2]) << 8) | (buffer[cpu->PC + 1])];
	//ABS X
	case 8:
		if (((uint16_t)(buffer[cpu->PC + 1]) + cpu->X) & 0x100)
		{
			cpu->cycles += 3;
		}
		else
		{
			cpu->cycles += 2;
		}
		return cpu->memory[((buffer[cpu->PC + 2]) << 8) | (buffer[cpu->PC + 1]) + cpu->X];
	//ABS Y
	case 9:
		if (((uint16_t)(buffer[cpu->PC + 1]) + cpu->Y) & 0x100)
		{
			cpu->cycles += 3;
		}
		else
		{
			cpu->cycles += 2;
		}
		return cpu->memory[((buffer[cpu->PC + 2]) << 8) | (buffer[cpu->PC + 1]) + cpu->Y];
	//IND
	case 10:
		cpu->cycles += 4;
		return ((uint16_t)(cpu->memory[((buffer[cpu->PC + 2]))])) << 8 | cpu->memory[(buffer[cpu->PC + 1])];
	//REL
	case 11:
		return buffer[cpu->PC + 1];
	//ZP ADDR
	case 12:
		cpu->cycles += 1;
		return buffer[cpu->PC + 1];
	//ABS ADDR
	case 13:
		cpu->cycles += 2;
		return ((buffer[cpu->PC + 2]) << 8) | (buffer[cpu->PC + 1]);
	//ABSX ADDR
	case 14:
		if (((uint16_t)(buffer[cpu->PC + 1]) + cpu->X) & 0x100)
		{
			cpu->cycles += 3;
		}
		else
		{
			cpu->cycles += 2;
		}
		return ((buffer[cpu->PC + 2]) << 8) | (buffer[cpu->PC + 1]) + cpu->X;
	//ABSY ADDR
	case 15:
		if (((uint16_t)(buffer[cpu->PC + 1]) + cpu->Y) & 0x100)
		{
			cpu->cycles += 3;
		}
		else
		{
			cpu->cycles += 2;
		}
		return ((buffer[cpu->PC + 2]) << 8) | (buffer[cpu->PC + 1]) + cpu->Y;
	//ZPX ADDR
	case 16:
		cpu->cycles += 2;
		return (uint8_t)(buffer[cpu->PC + 1] + cpu->X);
	//ZPY ADDR
	case 17:
		cpu->cycles += 2;
		return (uint8_t)(buffer[cpu->PC + 1] + cpu->Y);
	//INDX ADDR
	case 18:
		cpu->cycles += 4;
		return ((uint16_t)(cpu->memory[(buffer[cpu->PC + 1] + cpu->X + 1)]) << 8) | cpu->memory[(buffer[cpu->PC + 1] + cpu->X)];
	//INDY ADDR
	case 19:		
		if (((uint16_t)(cpu->memory[buffer[cpu->PC + 1]]) + cpu->Y) & 0x100)
		{
			cpu->cycles += 4;
		}
		else
		{
			cpu->cycles += 3;
		}
		return cpu->memory[(((uint16_t)(cpu->memory[(buffer[cpu->PC + 2])]) << 8) | cpu->memory[buffer[cpu->PC + 1]]) + cpu->Y];
	default:
		return 0;
	}
}

uint16_t shift_address(CPU* cpu, unsigned char* buffer)
{
	switch (addressing_mode[buffer[cpu->PC]])
	{
		//ZP
	case 2:
		cpu->cycles++;
		return buffer[cpu->PC + 1];
		//ZP X
	case 3:
		cpu->cycles ++;
		return (uint8_t)(buffer[cpu->PC + 1] + cpu->X);
		//ABS
	case 7:
		cpu->cycles ++;
		return ((buffer[cpu->PC + 2]) << 8) | (buffer[cpu->PC + 1]);
		//ABS X
	case 8:
		cpu->cycles += 2;
		return ((buffer[cpu->PC + 2]) << 8) | (buffer[cpu->PC + 1]) + cpu->X;
	}
}

void jmp(CPU* cpu, unsigned char* buffer)
{
	uint16_t pc = cpu->PC;
	uint16_t addr = fetch_data(cpu, buffer);
	cpu->PC = addr;
	cpu->cycles += 1;
}

void ldx(CPU* cpu, unsigned char* buffer)
{
	uint16_t pc = cpu->PC;
	uint16_t data = fetch_data(cpu, buffer);
	cpu->P[1] = (data == 0) ? 1 : 0;
	cpu->P[7] = (data & 0x80) ? 1 : 0;
	cpu->X = (uint8_t)(data & 0xFF);
	cpu->PC += address_bytes[buffer[cpu->PC]] + 1;
	cpu->cycles += 2;
}

void lda(CPU* cpu, unsigned char* buffer)
{
	uint16_t pc = cpu->PC;
	uint16_t data = fetch_data(cpu, buffer);
	cpu->P[1] = (data == 0) ? 1 : 0;
	cpu->P[7] = (data & 0x80) ? 1 : 0;
	cpu->A = (uint8_t)(data & 0xFF);
	cpu->PC += address_bytes[buffer[cpu->PC]] + 1;
	cpu->cycles += 2;
}

void stx(CPU* cpu, unsigned char* buffer)
{
	uint16_t address = fetch_data(cpu,buffer);
	cpu->memory[address] = cpu->X;
	cpu->PC += address_bytes[buffer[cpu->PC]] + 1;
	cpu->cycles += 2;
}

void sta(CPU* cpu, unsigned char* buffer)
{
	uint16_t address = fetch_data(cpu, buffer);
	cpu->memory[address] = cpu->A;
	cpu->PC += address_bytes[buffer[cpu->PC]] + 1;
	cpu->cycles += 2;
}

void sty(CPU* cpu, unsigned char* buffer)
{
	uint16_t address = fetch_data(cpu, buffer);
	cpu->memory[address] = cpu->Y;
	cpu->PC += address_bytes[buffer[cpu->PC]] + 1;
	cpu->cycles += 2;
}

void jsr(CPU* cpu, unsigned char* buffer)
{
	uint16_t nextPC = cpu->PC + 2;
	uint16_t pc = cpu->PC;
	cpu->memory[0x100 | cpu->S] = (uint8_t)((nextPC & 0xFF00) >> 8);
	cpu->memory[0x100 | cpu->S - 1] = nextPC & 0x00FF;
	cpu->S = cpu->S - 2;
	cpu->PC = ((buffer[pc + 2]) << 8) | (buffer[pc + 1]);
	cpu->cycles += 6;
}

void nop(CPU* cpu, unsigned char* buffer)
{
	cpu->PC += address_bytes[buffer[cpu->PC]] + 1;
	cpu->cycles += 2;
}

void sec(CPU* cpu, unsigned char* buffer)
{
	cpu->P[0] = 1;
	cpu->PC += address_bytes[buffer[cpu->PC]] + 1;
	cpu->cycles += 2;
}

void sei(CPU* cpu, unsigned char* buffer)
{
	cpu->P[2] = 1;
	cpu->PC += address_bytes[buffer[cpu->PC]] + 1;
	cpu->cycles += 2;
}

void sed(CPU* cpu, unsigned char* buffer)
{
	cpu->P[3] = 1;
	cpu->PC += address_bytes[buffer[cpu->PC]] + 1;
	cpu->cycles += 2;
}

void clc(CPU* cpu, unsigned char* buffer)
{
	cpu->P[0] = 0;
	cpu->PC += address_bytes[buffer[cpu->PC]] + 1;
	cpu->cycles += 2;
}

void bcs(CPU* cpu, unsigned char* buffer)
{
	uint8_t displacement = buffer[cpu->PC + 1];
	if (cpu->P[0] == 1)
	{

		if (((displacement + 2 + 0xFF & cpu->PC)) < (0xFF & cpu->PC))
		{
			cpu->cycles += 5;
		}
		else
		{
			cpu->cycles += 3;
		}
		cpu->PC += displacement + 2;
	}
	else
	{
		cpu->PC += address_bytes[buffer[cpu->PC]] + 1;
		cpu->cycles += 2;
	}
}

void bcc(CPU* cpu, unsigned char* buffer)
{
	uint8_t displacement = buffer[cpu->PC + 1];
	if (cpu->P[0] == 0)
	{

		if (((displacement + 2 + 0xFF & cpu->PC)) < (0xFF & cpu->PC))
		{
			cpu->cycles += 5;
		}
		else
		{
			cpu->cycles += 3;
		}
		cpu->PC += displacement + 2;
	}
	else
	{
		cpu->PC += address_bytes[buffer[cpu->PC]] + 1;
		cpu->cycles += 2;
	}
}

void beq(CPU* cpu, unsigned char* buffer)
{
	uint8_t displacement = buffer[cpu->PC + 1];
	if (cpu->P[1] == 1)
	{

		if (((displacement + 2 + 0xFF & cpu->PC)) < (0xFF & cpu->PC))
		{
			cpu->cycles += 4;
		}
		else
		{
			cpu->cycles += 3;
		}
		cpu->PC += displacement + 2;
	}
	else
	{
		cpu->PC += address_bytes[buffer[cpu->PC]] + 1;
		cpu->cycles += 2;
	}
}

void bne(CPU* cpu, unsigned char* buffer)
{
	uint8_t displacement = buffer[cpu->PC + 1];
	if (cpu->P[1] == 0)
	{

		if (((displacement + 2 + 0xFF & cpu->PC)) < (0xFF & cpu->PC))
		{
			cpu->cycles += 5;
		}
		else
		{
			cpu->cycles += 3;
		}
		cpu->PC += displacement + 2;
	}
	else
	{
		cpu->PC += address_bytes[buffer[cpu->PC]] + 1;
		cpu->cycles += 2;
	}
}

void bit_zp(CPU* cpu, unsigned char* buffer)
{
	uint8_t address = buffer[cpu->PC + 1];
	uint8_t mem_and = cpu->memory[address] & cpu->A;
	cpu->P[1] = (mem_and == 0) ? 1 : 0;
	cpu->P[6] = (cpu->memory[address] & 0x40) ? 1 : 0;
	cpu->P[7] = (cpu->memory[address] & 0x80) ? 1 : 0;
	cpu->cycles += 3;
	cpu->PC += address_bytes[buffer[cpu->PC]] + 1;
}

void bvs(CPU* cpu, unsigned char* buffer)
{
	uint8_t displacement = buffer[cpu->PC + 1];
	if (cpu->P[6] == 1)
	{

		if (((displacement + 2 + 0xFF & cpu->PC)) < (0xFF & cpu->PC))
		{
			cpu->cycles += 5;
		}
		else
		{
			cpu->cycles += 3;
		}
		cpu->PC += displacement + 2;
	}
	else
	{
		cpu->PC += address_bytes[buffer[cpu->PC]] + 1;
		cpu->cycles += 2;
	}
}

void bmi(CPU* cpu, unsigned char* buffer)
{
	uint8_t displacement = buffer[cpu->PC + 1];
	if (cpu->P[7] == 1)
	{

		if (((displacement + 2 + 0xFF & cpu->PC)) < (0xFF & cpu->PC))
		{
			cpu->cycles += 5;
		}
		else
		{
			cpu->cycles += 3;
		}
		cpu->PC += displacement + 2;
	}
	else
	{
		cpu->PC += address_bytes[buffer[cpu->PC]] + 1;
		cpu->cycles += 2;
	}
}

void bvc(CPU* cpu, unsigned char* buffer)
{
	uint8_t displacement = buffer[cpu->PC + 1];
	if (cpu->P[6] == 0)
	{

		if (((displacement + 2 + 0xFF & cpu->PC)) < (0xFF & cpu->PC))
		{
			cpu->cycles += 5;
		}
		else
		{
			cpu->cycles += 3;
		}
		cpu->PC += displacement + 2;
	}
	else
	{
		cpu->PC += address_bytes[buffer[cpu->PC]] + 1;
		cpu->cycles += 2;
	}
}

void bpl(CPU* cpu, unsigned char* buffer)
{
	uint8_t displacement = buffer[cpu->PC + 1];
	if (cpu->P[7] == 0)
	{

		if (((displacement + 2 + 0xFF & cpu->PC)) < (0xFF & cpu->PC))
		{
			cpu->cycles += 5;
		}
		else
		{
			cpu->cycles += 3;
		}
		cpu->PC += displacement + 2;
	}
	else
	{
		cpu->PC += address_bytes[buffer[cpu->PC]] + 1;
		cpu->cycles += 2;
	}
}

void rts(CPU* cpu, unsigned char* buffer)
{
	uint8_t lower = cpu->memory[ 0x100 | cpu->S + 1];
	uint8_t high = cpu->memory[ 0x100 | cpu->S + 2];
	uint16_t pc = high << 8 | lower;
	cpu->PC = pc + 1;
	cpu->cycles += 6;
	cpu->S += 2;
}

void php(CPU* cpu, unsigned char* buffer)
{
	int i = 0;
	uint8_t status = 0;
	for (i = 0; i < 8; i++)
	{
		status |= cpu->P[i] << i;
	}
	cpu->memory[ 0x100 | cpu->S] = status | 0x1 << 4;
	cpu->cycles += 3;
	cpu->S -= 1;
	cpu->PC += address_bytes[buffer[cpu->PC]] + 1;
}

void pha(CPU* cpu, unsigned char* buffer)
{
	cpu->memory[0x100|cpu->S] = cpu->A;
	cpu->cycles += 3;
	cpu->S -= 1;
	cpu->PC += address_bytes[buffer[cpu->PC]] + 1;
}

void pla(CPU* cpu, unsigned char* buffer)
{
	cpu->A = cpu->memory[0x100|cpu->S + 1];
	cpu->P[1] = (cpu->A == 0) ? 1 : 0;
	cpu->P[7] = (cpu->A & 0x80) ? 1 : 0;
	cpu->cycles += 4;
	cpu->S += 1;
	cpu->PC += address_bytes[buffer[cpu->PC]] + 1;
}

void plp(CPU* cpu, unsigned char* buffer)
{
	uint8_t status = cpu->memory[0x100 | cpu->S + 1];
	int i = 0;
	for (i = 0; i < 8; i++)
	{
		cpu->P[i] = ((status) >> i) & 0x1;
	}
	// Because it doesn't exist or something
	cpu->P[4] = 0;
	cpu->P[5] = 1;
	cpu->cycles += 4;
	cpu->S += 1;
	cpu->PC += address_bytes[buffer[cpu->PC]] + 1;
}

void and(CPU* cpu, unsigned char* buffer)
{
	cpu->A &= fetch_data(cpu,buffer);
	cpu->P[1] = (cpu->A == 0) ? 1 : 0;
	cpu->P[7] = (cpu->A & 0x80) ? 1 : 0;
	cpu->PC += address_bytes[buffer[cpu->PC]] + 1;
	cpu->cycles += 2;
}

void ora(CPU* cpu, unsigned char* buffer)
{
	cpu->A |= fetch_data(cpu,buffer);
	cpu->P[1] = (cpu->A == 0) ? 1 : 0;
	cpu->P[7] = (cpu->A & 0x80) ? 1 : 0;
	cpu->PC += address_bytes[buffer[cpu->PC]] + 1;
	cpu->cycles += 2;
}

void eor(CPU* cpu, unsigned char* buffer)
{
	cpu->A ^= fetch_data(cpu,buffer);
	cpu->P[1] = (cpu->A == 0) ? 1 : 0;
	cpu->P[7] = (cpu->A & 0x80) ? 1 : 0;
	cpu->PC += address_bytes[buffer[cpu->PC]] + 1;
	cpu->cycles += 2;
}

void cmp(CPU* cpu, unsigned char* buffer)
{
	uint8_t value = (uint8_t)(fetch_data(cpu, buffer) &0xFF);
	uint8_t comp = cpu->A - value;
	cpu->P[0] = (cpu->A >= value) ? 1 : 0;
	cpu->P[1] = (cpu->A == value) ? 1 : 0;
	cpu->P[7] = (comp & 0x80) ? 1 : 0;
	cpu->cycles += 2;
	cpu->PC += address_bytes[buffer[cpu->PC]] + 1;
}

void cld(CPU *cpu, unsigned char* buffer)
{
	cpu->P[3] = 0;
	cpu->PC += address_bytes[buffer[cpu->PC]] + 1;
	cpu->cycles += 2;
}

void clv(CPU *cpu, unsigned char* buffer)
{
	cpu->P[6] = 0;
	cpu->PC += address_bytes[buffer[cpu->PC]] + 1;
	cpu->cycles += 2;
}

void adc(CPU *cpu, unsigned char* buffer)
{
	uint16_t imm = fetch_data(cpu,buffer);
	uint16_t temp = imm + cpu->A + cpu->P[0];
	cpu->P[1] = ((temp & 0xFF) == 0) ? 1 : 0;
	cpu->P[7] = (temp & 0x80) ? 1 : 0;
	cpu->P[6] = overflow(cpu->A, imm, temp);
	cpu->P[0] = (temp & 0x100) ? 1 : 0;
	cpu->A = (uint8_t)(temp & 0x0FF);
	cpu->PC += address_bytes[buffer[cpu->PC]] + 1;
	cpu->cycles += 2;
}

void sbc(CPU *cpu, unsigned char* buffer)
{
	uint8_t imm = (uint8_t)(fetch_data(cpu, buffer) & 0xFF);
	uint16_t temp = cpu->A - imm - (1 - cpu->P[0]);
	cpu->P[1] = ((temp & 0xFF) == 0) ? 1 : 0;
	cpu->P[7] = (temp & 0x80) ? 1 : 0;
	cpu->P[6] = overflowsbc(cpu->A, imm, temp);
	cpu->P[0] = (temp & 0x100) ? 0 : 1;
	cpu->A = (uint8_t)(temp & 0x0FF);
	cpu->PC += address_bytes[buffer[cpu->PC]] + 1;
	cpu->cycles += 2;
}

void ldy(CPU *cpu, unsigned char* buffer)
{

	uint8_t data = (uint8_t)(fetch_data(cpu, buffer)&0xFF);
	cpu->P[1] = (data == 0) ? 1 : 0;
	cpu->P[7] = (data & 0x80) ? 1 : 0;
	cpu->Y = data;
	cpu->PC += address_bytes[buffer[cpu->PC]] + 1;
	cpu->cycles += 2;
}

void cpy(CPU* cpu, unsigned char* buffer)
{
	uint8_t value = (uint8_t)(fetch_data(cpu, buffer)&0xFF);
	uint8_t comp = cpu->Y - value;
	cpu->P[0] = (cpu->Y >= value) ? 1 : 0;
	cpu->P[1] = (cpu->Y == value) ? 1 : 0;
	cpu->P[7] = (comp & 0x80) ? 1 : 0;
	cpu->cycles += 2;
	cpu->PC += address_bytes[buffer[cpu->PC]] + 1;
}

void cpx(CPU* cpu, unsigned char* buffer)
{
	uint8_t value = (uint8_t)(fetch_data(cpu, buffer)&0xFF);
	uint8_t comp = cpu->X - value;
	cpu->P[0] = (cpu->X >= value) ? 1 : 0;
	cpu->P[1] = (cpu->X == value) ? 1 : 0;
	cpu->P[7] = (comp & 0x80) ? 1 : 0;
	cpu->cycles += 2;
	cpu->PC += address_bytes[buffer[cpu->PC]] + 1;
}

void iny(CPU* cpu, unsigned char* buffer)
{
	cpu->Y++;
	cpu->P[1] = (cpu->Y == 0) ? 1 : 0;
	cpu->P[7] = (cpu->Y & 0x80) ? 1 : 0;
	cpu->cycles += 2;
	cpu->PC++;
}

void dey(CPU* cpu, unsigned char* buffer)
{
	cpu->Y--;
	cpu->P[1] = (cpu->Y == 0) ? 1 : 0;
	cpu->P[7] = (cpu->Y & 0x80) ? 1 : 0;
	cpu->cycles += 2;
	cpu->PC++;
}

void tay(CPU* cpu, unsigned char* buffer)
{
	cpu->Y = cpu->A;
	cpu->P[1] = (cpu->Y == 0) ? 1 : 0;
	cpu->P[7] = (cpu->Y & 0x80) ? 1 : 0;
	cpu->cycles += 2;
	cpu->PC++;
}

void tsy(CPU* cpu, unsigned char* buffer)
{
	cpu->Y = cpu->A;
	cpu->P[1] = (cpu->Y == 0) ? 1 : 0;
	cpu->P[7] = (cpu->Y & 0x80) ? 1 : 0;
	cpu->cycles += 2;
	cpu->PC++;
}

void inx(CPU* cpu, unsigned char* buffer)
{
	cpu->X++;
	cpu->P[1] = (cpu->X == 0) ? 1 : 0;
	cpu->P[7] = (cpu->X & 0x80) ? 1 : 0;
	cpu->cycles += 2;
	cpu->PC++;
}

void dex(CPU* cpu, unsigned char* buffer)
{
	cpu->X--;
	cpu->P[1] = (cpu->X == 0) ? 1 : 0;
	cpu->P[7] = (cpu->X & 0x80) ? 1 : 0;
	cpu->cycles += 2;
	cpu->PC++;
}

void tax(CPU* cpu, unsigned char* buffer)
{
	cpu->X=cpu->A;
	cpu->P[1] = (cpu->X == 0) ? 1 : 0;
	cpu->P[7] = (cpu->X & 0x80) ? 1 : 0;
	cpu->cycles += 2;
	cpu->PC++;
}

void tsx(CPU* cpu, unsigned char* buffer)
{
	cpu->X = cpu->S;
	cpu->P[1] = (cpu->X == 0) ? 1 : 0;
	cpu->P[7] = (cpu->X & 0x80) ? 1 : 0;
	cpu->cycles += 2;
	cpu->PC++;
}
void txs(CPU* cpu, unsigned char* buffer)
{
	cpu->S = cpu->X;
	cpu->cycles += 2;
	cpu->PC++;
}
void txa(CPU* cpu, unsigned char* buffer)
{
	cpu->A = cpu->X;
	cpu->P[1] = (cpu->A == 0) ? 1 : 0;
	cpu->P[7] = (cpu->A & 0x80) ? 1 : 0;
	cpu->cycles += 2;
	cpu->PC++;
}

void tya(CPU* cpu, unsigned char* buffer)
{
	cpu->A = cpu->Y;
	cpu->P[1] = (cpu->A == 0) ? 1 : 0;
	cpu->P[7] = (cpu->A & 0x80) ? 1 : 0;
	cpu->cycles += 2;
	cpu->PC++;
}

void rti(CPU* cpu, unsigned char* buffer)
{
	uint8_t P = cpu->memory[0x100 | cpu->S + 1];
	uint8_t pc1 = cpu->memory[0x100 | cpu->S+2 ];
	uint8_t pc2 = cpu->memory[0x100 | cpu->S + 3];
	cpu->S += 3;
	cpu->cycles += 6;
	cpu->PC = (pc2 << 8) | pc1;
	int i = 0;
	for (i = 0; i < 8; i++)
	{
		cpu->P[i] = P >> i & 0x1;
	}

#ifndef NESTEST
	cpu->P[4] = 1;
#else
	cpu->P[4] = 0;
#endif
	cpu->P[5] = 1;
}

void lsr(CPU* cpu, unsigned char* buffer)
{
	uint8_t shift_value;
	if (buffer[cpu->PC] == 0x4A) {
		shift_value = cpu->A;
		cpu->P[0] = cpu->A & 0x1;
		cpu->A = cpu->A >> 1;
		cpu->cycles += 2;
		cpu->PC += 1;
		cpu->P[1] = (cpu->A == 0) ? 1 : 0;
		cpu->P[7] = (cpu->A & 0x80) ? 1 : 0;
	}
	else
	{
		shift_value = fetch_data(cpu, buffer);
		cpu->P[0] = shift_value & 0x1;
		shift_value = shift_value >> 1;
		cpu->P[1] = (shift_value == 0) ? 1 : 0;
		cpu->P[7] = (shift_value & 0x80) ? 1 : 0;
		cpu->memory[shift_address(cpu, buffer)] = shift_value;
		cpu->cycles += 3;
		cpu->PC += address_bytes[buffer[cpu->PC]] + 1;
	}
}

void asl(CPU* cpu, unsigned char* buffer)
{
	uint8_t shift_value;
	if (buffer[cpu->PC] == 0x0A) {
		cpu->P[0] = (cpu->A & 0x80) ? 1 : 0;
		cpu->A = cpu->A << 1;
		cpu->cycles += 2;
		cpu->PC += 1;
		cpu->P[1] = (cpu->A == 0) ? 1 : 0;
		cpu->P[7] = (cpu->A & 0x80) ? 1 : 0;
	}
	else
	{
		shift_value = fetch_data(cpu, buffer);
		cpu->P[0] = (shift_value & 0x80) ? 1 : 0;
		shift_value = shift_value << 1;
		cpu->P[1] = (shift_value == 0) ? 1 : 0;
		cpu->P[7] = (shift_value & 0x80) ? 1 : 0;
		cpu->memory[shift_address(cpu, buffer)] = shift_value;
		cpu->cycles += 3;
		cpu->PC += address_bytes[buffer[cpu->PC]] + 1;
	}
}


void ror(CPU* cpu, unsigned char* buffer)
{
	uint8_t shift_value;
	if (buffer[cpu->PC] == 0x6A) {
		shift_value = cpu->A;
		uint8_t oldP = cpu->P[0];
		cpu->P[0] = cpu->A & 0x1;
		cpu->A = cpu->A >> 1 | (oldP << 7);
		cpu->cycles += 2;
		cpu->PC += 1;
		cpu->P[1] = (cpu->A == 0) ? 1 : 0;
		cpu->P[7] = (cpu->A & 0x80) ? 1 : 0;
	}
	else
	{
		shift_value = fetch_data(cpu, buffer);
		uint8_t oldP = cpu->P[0];
		cpu->P[0] = shift_value & 0x1;
		shift_value = shift_value >> 1 | (oldP << 7);
		cpu->P[1] = (shift_value == 0) ? 1 : 0;
		cpu->P[7] = (shift_value & 0x80) ? 1 : 0;
		cpu->memory[shift_address(cpu, buffer)] = shift_value;
		cpu->cycles += 3;
		cpu->PC += address_bytes[buffer[cpu->PC]] + 1;
	}
}

void inc(CPU* cpu, unsigned char* buffer)
{
	uint8_t shift_value = fetch_data(cpu, buffer) & 0xFF;
	uint8_t oldP = cpu->P[0];
	shift_value++;
	cpu->P[1] = (shift_value == 0) ? 1 : 0;
	cpu->P[7] = (shift_value & 0x80) ? 1 : 0;
	cpu->memory[shift_address(cpu, buffer)&0xFF] = shift_value;
	cpu->cycles += 4;
	cpu->PC += address_bytes[buffer[cpu->PC]] + 1;

}

void dec(CPU* cpu, unsigned char* buffer)
{
	uint8_t shift_value = fetch_data(cpu, buffer) & 0xFF;
	uint8_t oldP = cpu->P[0];
	shift_value--;
	cpu->P[1] = (shift_value == 0) ? 1 : 0;
	cpu->P[7] = (shift_value & 0x80) ? 1 : 0;
	cpu->memory[shift_address(cpu, buffer)] = shift_value;
	cpu->cycles += 4;
	cpu->PC += address_bytes[buffer[cpu->PC]] + 1;

}

void rol (CPU* cpu, unsigned char* buffer)
{
	if (buffer[cpu->PC] == 0x2A) {
		uint8_t oldP = cpu->P[0];
		cpu->P[0] = (cpu->A & 0x80) ? 1 : 0;
		cpu->A = cpu->A << 1 | (oldP);
		cpu->cycles += 2;
		cpu->PC += 1;
		cpu->P[1] = (cpu->A == 0) ? 1 : 0;
		cpu->P[7] = (cpu->A & 0x80) ? 1 : 0;
	}
	else
	{
		uint8_t shift_value = fetch_data(cpu, buffer);
		uint8_t oldP = cpu->P[0];
		cpu->P[0] = (shift_value & 0x80) ? 1:0;
		shift_value = ((shift_value << 1 )| (oldP));
		cpu->P[1] = (shift_value == 0) ? 1 : 0;
		cpu->P[7] = (shift_value & 0x80) ? 1 : 0;
		cpu->memory[shift_address(cpu, buffer)] = shift_value;
		cpu->cycles += 3;
		cpu->PC += address_bytes[buffer[cpu->PC]] + 1;
	}
}


void initiate_cpu(CPU *cpu)
{
#ifdef NESTEST
	cpu->PC=0xC000;
#endif
	cpu->S = 0xFD;
	cpu->A = 0;
	cpu->X = 0;
	cpu->Y = 0;
	cpu->P[0] = 0;
	cpu->P[1] = 0;
	cpu->P[2] = 1;
	cpu->P[3] = 0;
#ifndef NESTEST
	cpu->P[4] = 1;
#else
	cpu->P[4] = 0;
#endif
	cpu->P[5] = 1;
	cpu->P[6] = 0;
	cpu->P[7] = 0;
	cpu->cycles = 0;
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
  printf("A:%x X:%x Y:%x P:%x SP:%x CYC:%d\n",cpu->A,cpu->X,cpu->Y,get_status_flag(cpu->P),cpu->S, cpu->cycles*3);
}

void debug_inst(CPU *cpu, unsigned char *buffer)
{
  char* instruction = inst_name[buffer[cpu->PC]];
  printf("%x ",cpu->PC);
  switch(addressing_mode[buffer[cpu->PC]])
  {
    case 0:
	printf("%s \t",instruction);
	get_cpu_state(cpu);
	break;
    case 1:
        printf("%s #%x\t",instruction, buffer[cpu->PC+1]);
	get_cpu_state(cpu);
	break;
    case 2:
	printf("%s $%x\t",instruction, buffer[cpu->PC+1]);
	get_cpu_state(cpu);
	break;
    case 3:
        printf("%s $%x,X\t",instruction,buffer[cpu->PC+1]);
	get_cpu_state(cpu);
	break;
    case 4:
        printf("%s $%x,Y\t",instruction,buffer[cpu->PC+1]);
	get_cpu_state(cpu);
	break;
    case 5:
        printf("%s ($%x),X\t",instruction,buffer[cpu->PC+1]);
	get_cpu_state(cpu);
	break;
    case 6:
        printf("%s ($%x),Y\t",instruction,buffer[cpu->PC+1]);
	get_cpu_state(cpu);
	break;
    case 7:
        printf("%s $%x%x\t",instruction,buffer[cpu->PC+2],buffer[cpu->PC+1]);
	get_cpu_state(cpu);
	break;
    case 8:
        printf("%s $%x%x,X\t",instruction,buffer[cpu->PC+2],buffer[cpu->PC+1]);
	get_cpu_state(cpu);
	break;
    case 9:
        printf("%s $%x%x,Y\t",instruction,buffer[cpu->PC+2],buffer[cpu->PC+1]);
	get_cpu_state(cpu);
	break;
    case 10:
        printf("%s $(%x%x)\t",instruction,buffer[cpu->PC+2],buffer[cpu->PC+1]);
	get_cpu_state(cpu);
	break;
    case 11:
        printf("%s %x\t",instruction,buffer[cpu->PC+1]);
	get_cpu_state(cpu);
	break;
	case 12:
		printf("%s $%x\t", instruction, buffer[cpu->PC + 1]);
		get_cpu_state(cpu);
		break;
	case 13:
		printf("%s $%x%x\t", instruction, buffer[cpu->PC + 2], buffer[cpu->PC + 1]);
		get_cpu_state(cpu);
		break;
	case 14:
		printf("%s $%x%x,X\t", instruction, buffer[cpu->PC + 2], buffer[cpu->PC + 1]);
		get_cpu_state(cpu);
		break;
	case 15:
		printf("%s $%x%x,Y\t", instruction, buffer[cpu->PC + 2], buffer[cpu->PC + 1]);
		get_cpu_state(cpu);
		break;
	case 16:
		printf("%s $%x,X\t", instruction, buffer[cpu->PC + 1]);
		get_cpu_state(cpu);
		break;
	case 17:
		printf("%s $%x,Y\t", instruction, buffer[cpu->PC + 1]);
		get_cpu_state(cpu);
		break;
	case 18:
		printf("%s ($%x),X\t", instruction, buffer[cpu->PC + 1]);
		get_cpu_state(cpu);
		break;
	case 19:
		printf("%s ($%x),Y\t", instruction, buffer[cpu->PC + 1]);
		get_cpu_state(cpu);
		break;
  }
}

void decode_and_execute(CPU *cpu, unsigned char *buffer)
{
     debug_inst(cpu, buffer);
     uint8_t instOp = buffer[cpu->PC];
     instruction[instOp](cpu,buffer);
}

int enter_cpu(unsigned char *buffer)
{
   CPU cpu;
   initiate_cpu(&cpu);
   int i = 1;
   while(i <= 2300)
   {
	   printf("%d ", i);
     decode_and_execute(&cpu,buffer);
     i++;
   }
   return 0;
}
