#include "cpu.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>

#define overflow(A,B,C) (~((A^B) & 0x80) & ((A^C)&0x80)) >> 7
#define overflowsbc(A,B,C) (((A^B) & 0x80) & ((A^C)&0x80)) >> 7

uint8_t address_bytes[256] = { 0,1,0,1,1,1,1,1,0,1,0,0,2,2,2,2,
1,1,0,1,1,1,1,1,0,2,0,2,2,2,2,2,
2,1,0,1,1,1,1,1,0,1,0,0,2,2,2,2,
1,1,0,1,1,1,1,1,0,2,0,2,2,2,2,2,
0,1,0,1,1,1,1,1,0,1,0,0,2,2,2,2,
1,1,0,1,1,1,1,1,0,2,0,2,2,2,2,2,
0,1,0,1,1,1,1,1,0,1,0,0,2,2,2,2,
1,1,0,1,1,1,1,1,0,2,0,2,2,2,2,2,
1,1,0,1,1,1,1,1,0,0,0,0,2,2,2,2,
1,1,0,0,1,1,1,1,0,2,0,0,0,2,0,0,
1,1,1,1,1,1,1,1,0,1,0,0,2,2,2,2,
1,1,0,1,1,1,1,1,0,2,0,0,2,2,2,2,
1,1,0,1,1,1,1,1,0,1,0,0,2,2,2,2,
1,1,0,1,1,1,1,1,0,2,0,2,2,2,2,2,
1,1,0,1,1,1,1,1,0,1,0,1,2,2,2,2,
1,1,0,1,1,1,1,1,0,2,0,2,2,2,2,2 };

uint8_t addressing_mode[256] = { 0, 5, 0, 5, 2, 2, 2, 2, 0, 1, 0, 1, 7, 7, 7, 7,
11, 6, 0, 6, 3, 3, 3, 3, 0, 9, 0, 9, 8, 8, 8, 8,
7, 5, 0, 5, 2, 2, 2, 2, 0, 1, 0, 1, 7, 7, 7, 7,
11, 6, 0, 6, 3, 3, 3, 3, 0, 9, 0, 9, 8, 8, 8, 8,
0, 5, 0, 5, 2, 2, 2, 2, 0, 1, 0, 1, 13, 7, 7, 7,
11, 6, 0, 6, 3, 3, 3, 3, 0, 9, 0, 9, 8, 8, 8, 8,
0, 5, 0, 5, 2, 2, 2, 2, 0, 1, 0, 1, 10, 7, 7, 7,
11, 6, 0, 6, 3, 3, 3, 3, 0, 9, 0, 9, 8, 8, 8, 8,
1, 18, 1, 18, 12, 12, 12, 12, 0, 1, 0, 1, 13, 13, 13, 13,
11, 19, 0, 6, 16, 16, 17, 17, 0, 15, 0, 9, 8, 14, 8, 8,
1, 5, 1, 5, 2, 2, 2, 2, 0, 1, 0, 1, 7, 7, 7, 7,
11, 6, 0, 6, 3, 3, 4, 4, 0, 9, 0, 9, 8, 8, 9, 9,
1, 5, 0, 5, 2, 2, 2, 2, 0, 1, 0, 1, 7, 7, 7, 7,
11, 6, 0, 6, 3, 3, 3, 3, 0, 9, 0, 9, 8, 8, 8, 8,
1, 5, 1, 5, 2, 2, 2, 2, 0, 1, 0, 1, 7, 7, 7, 7,
11, 6, 0, 6, 3, 3, 3, 3, 0, 9, 0, 9, 8, 8, 8, 8 };

char* inst_name[256] = { "brk", "ora", "kil", "slo", "nop", "ora", "asl", "slo", "php", "ora", "asl", "anc", "nop", "ora", "asl", "slo",
"bpl", "ora", "kil", "slo", "nop", "ora", "asl", "slo", "clc", "ora", "nop", "slo", "nop", "ora", "asl", "slo",
"jsr", "and", "kil", "rla", "bit", "and", "rol", "rla", "plp", "and", "rol", "anc", "bit", "and", "rol", "rla",
"bmi", "and", "kil", "rla", "nop", "and", "rol", "rla", "sec", "and", "nop", "rla", "nop", "and", "rol", "rla",
"rti", "eor", "kil", "sre", "nop", "eor", "lsr", "sre", "pha", "eor", "lsr", "alr", "jmp", "eor", "lsr", "sre",
"bvc", "eor", "kil", "sre", "nop", "eor", "lsr", "sre", "cli", "eor", "nop", "sre", "nop", "eor", "lsr", "sre",
"rts", "adc", "kil", "rra", "nop", "adc", "ror", "rra", "pla", "adc", "ror", "arr", "jmp", "adc", "ror", "rra",
"bvs", "adc", "kil", "rra", "nop", "adc", "ror", "rra", "sei", "adc", "nop", "rra", "nop", "adc", "ror", "rra",
"nop", "sta", "nop", "sax", "sty", "sta", "stx", "sax", "dey", "nop", "txa", "xaa", "sty", "sta", "stx", "sax",
"bcc", "sta", "kil", "ahx", "sty", "sta", "stx", "sax", "tya", "sta", "txs", "tas", "shy", "sta", "shx", "ahx",
"ldy", "lda", "ldx", "lax", "ldy", "lda", "ldx", "lax", "tay", "lda", "tax", "lax", "ldy", "lda", "ldx", "lax",
"bcs", "lda", "kil", "lax", "ldy", "lda", "ldx", "lax", "clv", "lda", "tsx", "las", "ldy", "lda", "ldx", "lax",
"cpy", "cmp", "nop", "dcp", "cpy", "cmp", "dec", "dcp", "iny", "cmp", "dex", "axs", "cpy", "cmp", "dec", "dcp",
"bne", "cmp", "kil", "dcp", "nop", "cmp", "dec", "dcp", "cld", "cmp", "nop", "dcp", "nop", "cmp", "dec", "dcp",
"cpx", "sbc", "nop", "isc", "cpx", "sbc", "inc", "isc", "inx", "sbc", "nop", "sbc", "cpx", "sbc", "inc", "isc",
"beq", "sbc", "kil", "isc", "nop", "sbc", "inc", "isc", "sed", "sbc", "nop", "isc", "nop", "sbc", "inc", "isc" };


void read_extra(uint16_t address, CPU* cpu)
{
	uint16_t temp_addr = address;
	if (address >= 0x3000 && address <= 0x3EFF)
	{
		temp_addr = address - 0x1000;
	}
	if (temp_addr == 0x2007)
	{
		uint8_t oldbuff = cpu->vram_buff;
		if (cpu->ppu->regV <= 0x2FFF)
		{
			cpu->vram_buff = cpu->ppu->memory[cpu->ppu->regV];
			cpu->memory[0x2007] = oldbuff;
		}
		read_vram_data(cpu->ppu);
	}
	if (temp_addr == 0x2002)
	{
		status_read(cpu->ppu);
		//cpu->memory[0x2002] &= 0x7F;
	}
	if (temp_addr == 0x2001)
	{
		printf("SO.... %x\n", cpu->memory[0x2001]);
	}
}

uint16_t fetch_data(CPU *cpu, unsigned char* memory)
{
	uint8_t data = 0;
	switch (addressing_mode[memory[cpu->PC]])
	{
	//IMPLIED
	case 0:
		return 0;
	//IMMEDIATE
	case 1:
		return memory[cpu->PC + 1];
	//ZP
	case 2:
		cpu->cycles++; 
		return cpu->memory[memory[cpu->PC + 1]];
	//ZP X
	case 3:
		cpu->cycles += 2;
		return cpu->memory[(uint8_t)(memory[cpu->PC + 1] + cpu->X)];
	//ZP Y
	case 4:
		cpu->cycles += 2;
		return cpu->memory[(uint8_t)(memory[cpu->PC + 1] + cpu->Y)];
	//IND X
	case 5:
		cpu->cycles += 4;
		read_extra(((uint16_t)(cpu->memory[0xFF & ((memory[cpu->PC + 1] + cpu->X + 1))]) << 8) | cpu->memory[0xFF & (memory[cpu->PC + 1] + cpu->X)], cpu);
		data = cpu->memory[((uint16_t)(cpu->memory[0xFF & ((memory[cpu->PC + 1] + cpu->X + 1))]) << 8) | cpu->memory[0xFF & (memory[cpu->PC + 1] + cpu->X)]];
		return data;
	//IND Y
	case 6:
		if (((uint16_t)(cpu->memory[memory[cpu->PC + 1] ])+ cpu->Y) & 0x100)
		{
			cpu->cycles += 4;
		}
		else
		{
			cpu->cycles += 3;
		}
		uint16_t addr = (((cpu->memory[ 0xFF&(memory[cpu->PC + 1] + 1)])) << 8) | cpu->memory[memory[cpu->PC + 1]];
		read_extra(0xFFFF & (addr + cpu->Y), cpu);
		data = cpu->memory[0xFFFF & (addr + cpu->Y)];
		return data;
	//ABS
	case 7:
		cpu->cycles += 2;
		read_extra(((memory[cpu->PC + 2]) << 8) | (memory[cpu->PC + 1]), cpu);
		data = cpu->memory[((memory[cpu->PC + 2]) << 8) | (memory[cpu->PC + 1])];
		return data;
	//ABS X
	case 8:
		if (((uint16_t)(memory[cpu->PC + 1]) + cpu->X) & 0x100)
		{
			cpu->cycles += 3;
		}
		else
		{
			cpu->cycles += 2;
		}
		read_extra((((memory[cpu->PC + 2]) << 8) | (memory[cpu->PC + 1])) + cpu->X, cpu);
		data = cpu->memory[(((memory[cpu->PC + 2]) << 8) | (memory[cpu->PC + 1])) + cpu->X];
		return data;
	//ABS Y
	case 9:
		if (((uint16_t)(memory[cpu->PC + 1]) + cpu->Y) & 0x100)
		{
			cpu->cycles += 3;
		}
		else
		{
			cpu->cycles += 2;
		}
		read_extra(0xFFFF & (((memory[cpu->PC + 2] << 8) | (memory[cpu->PC + 1])) + cpu->Y), cpu);
		data = cpu->memory[0xFFFF & (((memory[cpu->PC + 2] << 8) | (memory[cpu->PC + 1])) + cpu->Y)];
		return data;
	//IND
	case 10:
		cpu->cycles += 4;
		uint16_t indaddr = ((uint16_t)(memory[cpu->PC + 2]) << 8) | memory[cpu->PC + 1];
		uint16_t indaddr2 = (indaddr & 0xFF00) | ((indaddr + 1) & 0x00FF);
		return ((uint16_t)(cpu->memory[indaddr2])) << 8 | cpu->memory[indaddr];
	//REL
	case 11:
		return memory[cpu->PC + 1];
	//ZP ADDR
	case 12:
		cpu->cycles += 1;
		return memory[cpu->PC + 1];
	//ABS ADDR
	case 13:
		cpu->cycles += 2;
		return ((memory[cpu->PC + 2]) << 8) | (memory[cpu->PC + 1]);
	//ABSX ADDR
	case 14:
		cpu->cycles += 3;
		return (((memory[cpu->PC + 2]) << 8) | (memory[cpu->PC + 1])) + cpu->X;
	//ABSY ADDR
	case 15:
			cpu->cycles += 3;
		return (((memory[cpu->PC + 2]) << 8) | (memory[cpu->PC + 1])) + cpu->Y;
	//ZPX ADDR
	case 16:
		cpu->cycles += 2;
		return (uint8_t)(memory[cpu->PC + 1] + cpu->X);
	//ZPY ADDR
	case 17:
		cpu->cycles += 2;
		return (uint8_t)(memory[cpu->PC + 1] + cpu->Y);
	//INDX ADDR
	case 18:
		cpu->cycles += 4;
		return ((uint16_t)(cpu->memory[0xFF&((memory[cpu->PC + 1] + cpu->X) + 1)]) << 8) | cpu->memory[0xFF&(memory[cpu->PC + 1] + cpu->X)];
	//INDY ADDR
	case 19:	
		cpu->cycles += 4;
		uint16_t addr2 = (((cpu->memory[0xFF & (memory[cpu->PC + 1] + 1)])) << 8) | cpu->memory[memory[cpu->PC + 1]];
		return (addr2 + cpu->Y)&0xFFFF;
	default:
		return 0;
	}
}

uint16_t shift_address(CPU* cpu, unsigned char* memory)
{
	switch (addressing_mode[memory[cpu->PC]])
	{
		//ZP
	case 2:
		return memory[cpu->PC + 1];
		//ZP X
	case 3:
		return (uint8_t)0xFF&(memory[cpu->PC + 1] + cpu->X);
		//ABS
	case 7:
		return ((memory[cpu->PC + 2]) << 8) | (memory[cpu->PC + 1]);
		//ABS X
	case 8:
		cpu->cycles++;
		return (((memory[cpu->PC + 2]) << 8) | (memory[cpu->PC + 1])) + cpu->X;
	case 5:
		return ((uint16_t)(cpu->memory[0xFF & ((memory[cpu->PC + 1] + cpu->X) + 1)]) << 8) | cpu->memory[0xFF & (memory[cpu->PC + 1] + cpu->X)];
	case 6:
		uint16_t addr2 = (((cpu->memory[0xFF & (memory[cpu->PC + 1] + 1)])) << 8) | cpu->memory[memory[cpu->PC + 1]];
		return (addr2 + cpu->Y) & 0xFFFF;
	case 9:
		return (((memory[cpu->PC + 2]) << 8) | (memory[cpu->PC + 1])) + cpu->Y;
	}
}


void mirror_write(uint16_t address, uint8_t data, CPU* cpu)
{
	while (address <= 0x3FFF) {
		cpu->memory[address] = data;
		address += 8;
	}
}

void store_extra(uint16_t address, uint8_t data, CPU* cpu)
{
	uint16_t temp_addr = address;
	/*if (address >= 0x3000 && address <= 0x3EFF)
	{
		temp_addr = address - 0x1000;
	}*/
	if (temp_addr== 0x2006)
	{
		write_vram_addr(cpu->ppu, data);
	}
	else if (temp_addr == 0x2007)
	{
		write_vram_data(cpu->ppu, data);
	}
	else if (temp_addr == 0x2000)
	{
		write_controll_reg(cpu->ppu, data);
	}
	else if (temp_addr == 0x2005)
	{
		write_scroll_reg(cpu->ppu, data);
	}
	else if (temp_addr == 0x2004)
	{
		sprite_data(cpu->ppu);
	}
	else if (temp_addr == 0x4014)
	{
		//Need to take into account OAMADDR
		uint16_t addr = data << 8;
		for (int i = 0; i < 256; i++)
			cpu->ppu->OAM[0][i] = cpu->memory[addr + i];
		if ((cpu->cycles % 2) == 0)
		{
			cpu->dma_wait_cycles = 513;
		}
		else
		{
			cpu->dma_wait_cycles = 514;
		}
		cpu->dma_wait = 1;
	}
	if (temp_addr >= 0x2000 && temp_addr < 0x2008)
	{
		mirror_write(temp_addr, data, cpu);
	}
}

void jmp(CPU* cpu, unsigned char* memory)
{
	uint16_t pc = cpu->PC;
	uint16_t addr = fetch_data(cpu, memory);
	cpu->PC = addr;
	cpu->cycles += 1;
}

void ldx(CPU* cpu, unsigned char* memory)
{
	uint16_t pc = cpu->PC;
	uint16_t data = fetch_data(cpu, memory);
	cpu->P[1] = (data == 0) ? 1 : 0;
	cpu->P[7] = (data & 0x80) ? 1 : 0;
	cpu->X = (uint8_t)(data & 0xFF);
	cpu->PC += address_bytes[memory[cpu->PC]] + 1;
	cpu->cycles += 2;
}

void lda(CPU* cpu, unsigned char* memory)
{
	uint16_t pc = cpu->PC;
	uint16_t data = fetch_data(cpu, memory);
	cpu->P[1] = (data == 0) ? 1 : 0;
	cpu->P[7] = (data & 0x80) ? 1 : 0;
	cpu->A = (uint8_t)(data & 0xFF);
	cpu->PC += address_bytes[memory[cpu->PC]] + 1;
	cpu->cycles += 2;
}

void stx(CPU* cpu, unsigned char* memory)
{
	uint16_t address = fetch_data(cpu,memory);
	cpu->memory[address] = cpu->X;
	cpu->PC += address_bytes[memory[cpu->PC]] + 1; 
	cpu->cycles += 2;
	store_extra(address, cpu->X, cpu);
}

void sta(CPU* cpu, unsigned char* memory)
{
	uint16_t address = fetch_data(cpu, memory);
	cpu->memory[address] = cpu->A;
	cpu->PC += address_bytes[memory[cpu->PC]] + 1;
	cpu->cycles += 2;
	store_extra(address, cpu->A, cpu);
}

void sty(CPU* cpu, unsigned char* memory)
{
	uint16_t address = fetch_data(cpu, memory);
	cpu->memory[address] = cpu->Y;
	cpu->PC += address_bytes[memory[cpu->PC]] + 1;
	cpu->cycles += 2;
	store_extra(address, cpu->Y, cpu);
}

void jsr(CPU* cpu, unsigned char* memory)
{
	uint16_t nextPC = cpu->PC + 2;
	uint16_t pc = cpu->PC;
	cpu->memory[0x100 | cpu->S] = (uint8_t)((nextPC & 0xFF00) >> 8);
	cpu->memory[0x100 | cpu->S - 1] = nextPC & 0x00FF;
	cpu->S = cpu->S - 2;
	cpu->PC = ((memory[pc + 2]) << 8) | (memory[pc + 1]);
	cpu->cycles += 6;
}

void nop(CPU* cpu, unsigned char* memory)
{
	switch (memory[cpu->PC]) {
	case 0x04:
		cpu->cycles += 3;
		break;
	case 0x0C:
		cpu->cycles += 4;
		break;
	case 0x14:
		cpu->cycles += 4;
		break;
	case 0x1C:
		if ((memory[cpu->PC + 2] + cpu->X) & 0x100)
			cpu->cycles++;
		cpu->cycles += 4;
		break;
	case 0x34:
		cpu->cycles += 4;
		break;
	case 0x3C:
		if ((memory[cpu->PC + 2] + cpu->X) & 0x100)
			cpu->cycles++;
		cpu->cycles += 4;
		break;
	case 0x44:
		cpu->cycles += 3;
		break;
	case 0x54:
		cpu->cycles += 4;
		break;
	case 0x5C:
		if ((memory[cpu->PC + 2] + cpu->X) & 0x100)
			cpu->cycles++;
		cpu->cycles += 4;
		break;
	case 0x64:
		cpu->cycles += 3;
		break;
	case 0x74:
		cpu->cycles += 4;
		break;
	case 0x7C:
		if ((memory[cpu->PC + 2] + cpu->X) & 0x100)
			cpu->cycles++;
		cpu->cycles += 4;
		break;
	case 0x80:
		cpu->cycles += 2;
		break;
	case 0x82:
		cpu->cycles += 2;
		break;
	case 0x89:
		cpu->cycles += 2;
		break;
	case 0xC2:
		cpu->cycles += 2;
		break;
	case 0xD4:
		cpu->cycles += 4;
		break;
	case 0xDC:
		if ((memory[cpu->PC + 2] + cpu->X) & 0x100)
			cpu->cycles++;
		cpu->cycles += 4;
		break;
	case 0xE2:
		cpu->cycles += 2;
		break;
	case 0xF4:
		cpu->cycles += 4;
		break;
	case 0xFC:
		if ((memory[cpu->PC + 2] + cpu->X) & 0x100)
			cpu->cycles++;
		cpu->cycles += 4;
		break;
	default:
		cpu->cycles += 2;
		break;
	}
	cpu->PC += address_bytes[memory[cpu->PC]] + 1;
}

void sec(CPU* cpu, unsigned char* memory)
{
	cpu->P[0] = 1;
	cpu->PC += address_bytes[memory[cpu->PC]] + 1;
	cpu->cycles += 2;
}

void sei(CPU* cpu, unsigned char* memory)
{
	cpu->P[2] = 1;
	cpu->PC += address_bytes[memory[cpu->PC]] + 1;
	cpu->cycles += 2;
}

void sed(CPU* cpu, unsigned char* memory)
{
	cpu->P[3] = 1;
	cpu->PC += address_bytes[memory[cpu->PC]] + 1;
	cpu->cycles += 2;
}

void clc(CPU* cpu, unsigned char* memory)
{
	cpu->P[0] = 0;
	cpu->PC += address_bytes[memory[cpu->PC]] + 1;
	cpu->cycles += 2;
}

void bcs(CPU* cpu, unsigned char* memory)
{
	int8_t displacement = memory[cpu->PC + 1];
	if (cpu->P[0] == 1)
	{

		if (((displacement +  (0xFF & cpu->PC+2))) & 0x100)
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
		cpu->PC += address_bytes[memory[cpu->PC]] + 1;
		cpu->cycles += 2;
	}
}

void bcc(CPU* cpu, unsigned char* memory)
{
	int8_t displacement = memory[cpu->PC + 1];
	if (cpu->P[0] == 0)
	{

		if (((displacement +  (0xFF & cpu->PC+2))) & 0x100)
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
		cpu->PC += address_bytes[memory[cpu->PC]] + 1;
		cpu->cycles += 2;
	}
}

void beq(CPU* cpu, unsigned char* memory)
{
	int8_t displacement = memory[cpu->PC + 1];
	if (cpu->P[1] == 1)
	{

		if (((displacement +  (0xFF & cpu->PC+2))) & 0x100)
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
		cpu->PC += address_bytes[memory[cpu->PC]] + 1;
		cpu->cycles += 2;
	}
}

void bne(CPU* cpu, unsigned char* memory)
{
	int8_t displacement = memory[cpu->PC + 1];
	if (cpu->P[1] == 0)
	{

		if (((displacement +  (0xFF & cpu->PC+2))) & 0x100)
		{
			cpu->cycles += 5;
		}
		else
		{
			cpu->cycles += 3;
		}
		cpu->PC += displacement+2;
	}
	else
	{
		cpu->PC += address_bytes[memory[cpu->PC]] + 1;
		cpu->cycles += 2;
	}
}


void bvs(CPU* cpu, unsigned char* memory)
{
	int8_t displacement = memory[cpu->PC + 1];
	if (cpu->P[6] == 1)
	{

		if (((displacement +  (0xFF & cpu->PC+2))) & 0x100)
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
		cpu->PC += address_bytes[memory[cpu->PC]] + 1;
		cpu->cycles += 2;
	}
}

void bmi(CPU* cpu, unsigned char* memory)
{
	int8_t displacement = memory[cpu->PC + 1];
	if (cpu->P[7] == 1)
	{

		if (((displacement +  (0xFF & cpu->PC+2))) & 0x100)
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
		cpu->PC += address_bytes[memory[cpu->PC]] + 1;
		cpu->cycles += 2;
	}
}

void bvc(CPU* cpu, unsigned char* memory)
{
	int8_t displacement = memory[cpu->PC + 1];
	if (cpu->P[6] == 0)
	{

		if (((displacement +  (0xFF & cpu->PC+2))) & 0x100)
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
		cpu->PC += address_bytes[memory[cpu->PC]] + 1;
		cpu->cycles += 2;
	}
}

void bpl(CPU* cpu, unsigned char* memory)
{
	int8_t displacement = memory[cpu->PC + 1];
	if (cpu->P[7] == 0)
	{

		if (((displacement +  (0xFF & cpu->PC+2))) & 0x100)
		{
			cpu->cycles += 5;
		}
		else
		{
			cpu->cycles += 3;
		}
	//	uint16_t oldPC = cpu->PC;
		cpu->PC += displacement + 2;
		//cpu->PC = (oldPC & 0xFF00) | (cpu->PC & 0x00FF);
	}
	else
	{
		cpu->PC += address_bytes[memory[cpu->PC]] + 1;
		cpu->cycles += 2;
	}
}

void rts(CPU* cpu, unsigned char* memory)
{
	uint8_t lower = cpu->memory[ 0x100 | cpu->S + 1];
	uint8_t high = cpu->memory[ 0x100 | cpu->S + 2];
	uint16_t pc = high << 8 | lower;
	cpu->PC = pc + 1;
	cpu->cycles += 6;
	cpu->S += 2;
}

void php(CPU* cpu, unsigned char* memory)
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
	cpu->PC += address_bytes[memory[cpu->PC]] + 1;
}

void pha(CPU* cpu, unsigned char* memory)
{
	cpu->memory[0x100|cpu->S] = cpu->A;
	cpu->cycles += 3;
	cpu->S -= 1;
	cpu->PC += address_bytes[memory[cpu->PC]] + 1;
}

void pla(CPU* cpu, unsigned char* memory)
{
	cpu->A = cpu->memory[0x100|cpu->S + 1];
	cpu->P[1] = (cpu->A == 0) ? 1 : 0;
	cpu->P[7] = (cpu->A & 0x80) ? 1 : 0;
	cpu->cycles += 4;
	cpu->S += 1;
	cpu->PC += address_bytes[memory[cpu->PC]] + 1;
}

void plp(CPU* cpu, unsigned char* memory)
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
	cpu->PC += address_bytes[memory[cpu->PC]] + 1;
}

void and(CPU* cpu, unsigned char* memory)
{
	cpu->A &= fetch_data(cpu,memory);
	cpu->P[1] = (cpu->A == 0) ? 1 : 0;
	cpu->P[7] = (cpu->A & 0x80) ? 1 : 0;
	cpu->PC += address_bytes[memory[cpu->PC]] + 1;
	cpu->cycles += 2;
}

void ora(CPU* cpu, unsigned char* memory)
{
	cpu->A |= fetch_data(cpu,memory);
	cpu->P[1] = (cpu->A == 0) ? 1 : 0;
	cpu->P[7] = (cpu->A & 0x80) ? 1 : 0;
	cpu->PC += address_bytes[memory[cpu->PC]] + 1;
	cpu->cycles += 2;
}

void eor(CPU* cpu, unsigned char* memory)
{
	cpu->A ^= fetch_data(cpu,memory);
	cpu->P[1] = (cpu->A == 0) ? 1 : 0;
	cpu->P[7] = (cpu->A & 0x80) ? 1 : 0;
	cpu->PC += address_bytes[memory[cpu->PC]] + 1;
	cpu->cycles += 2;
}

void cmp(CPU* cpu, unsigned char* memory)
{
	uint8_t value = (uint8_t)(fetch_data(cpu, memory) &0xFF);
	uint8_t comp = cpu->A - value;
	cpu->P[0] = (cpu->A >= value) ? 1 : 0;
	cpu->P[1] = (cpu->A == value) ? 1 : 0;
	cpu->P[7] = (comp & 0x80) ? 1 : 0;
	cpu->cycles += 2;
	cpu->PC += address_bytes[memory[cpu->PC]] + 1;
}

void cld(CPU *cpu, unsigned char* memory)
{
	cpu->P[3] = 0;
	cpu->PC += address_bytes[memory[cpu->PC]] + 1;
	cpu->cycles += 2;
}

void clv(CPU *cpu, unsigned char* memory)
{
	cpu->P[6] = 0;
	cpu->PC += address_bytes[memory[cpu->PC]] + 1;
	cpu->cycles += 2;
}

void adc(CPU *cpu, unsigned char* memory)
{
	uint16_t imm = fetch_data(cpu,memory);
	uint16_t temp = imm + cpu->A + cpu->P[0];
	cpu->P[1] = ((temp & 0xFF) == 0) ? 1 : 0;
	cpu->P[7] = (temp & 0x80) ? 1 : 0;
	cpu->P[6] = overflow(cpu->A, (uint8_t)(imm), (uint8_t)(temp));
	cpu->P[0] = (temp & 0x100) ? 1 : 0;
	cpu->A = (uint8_t)(temp & 0x0FF);
	cpu->PC += address_bytes[memory[cpu->PC]] + 1;
	cpu->cycles += 2;
}

void sbc(CPU *cpu, unsigned char* memory)
{
	uint8_t imm = (uint8_t)(fetch_data(cpu, memory) & 0xFF);
	uint16_t temp = cpu->A - imm - (1 - cpu->P[0]);
	cpu->P[1] = ((temp & 0xFF) == 0) ? 1 : 0;
	cpu->P[7] = (temp & 0x80) ? 1 : 0;
	cpu->P[6] = overflowsbc(cpu->A, (uint8_t)(imm), (uint8_t)(temp));
	cpu->P[0] = (temp & 0x100) ? 0 : 1;
	cpu->A = (uint8_t)(temp & 0x0FF);
	cpu->PC += address_bytes[memory[cpu->PC]] + 1;
	cpu->cycles += 2;
}

void ldy(CPU *cpu, unsigned char* memory)
{

	uint8_t data = (uint8_t)(fetch_data(cpu, memory)&0xFF);
	cpu->P[1] = (data == 0) ? 1 : 0;
	cpu->P[7] = (data & 0x80) ? 1 : 0;
	cpu->Y = data;
	cpu->PC += address_bytes[memory[cpu->PC]] + 1;
	cpu->cycles += 2;
}

void cpy(CPU* cpu, unsigned char* memory)
{
	uint8_t value = (uint8_t)(fetch_data(cpu, memory)&0xFF);
	uint8_t comp = cpu->Y - value;
	cpu->P[0] = (cpu->Y >= value) ? 1 : 0;
	cpu->P[1] = (cpu->Y == value) ? 1 : 0;
	cpu->P[7] = (comp & 0x80) ? 1 : 0;
	cpu->cycles += 2;
	cpu->PC += address_bytes[memory[cpu->PC]] + 1;
}

void cpx(CPU* cpu, unsigned char* memory)
{
	uint8_t value = (uint8_t)(fetch_data(cpu, memory)&0xFF);
	uint8_t comp = cpu->X - value;
	cpu->P[0] = (cpu->X >= value) ? 1 : 0;
	cpu->P[1] = (cpu->X == value) ? 1 : 0;
	cpu->P[7] = (comp & 0x80) ? 1 : 0;
	cpu->cycles += 2;
	cpu->PC += address_bytes[memory[cpu->PC]] + 1;
}

void iny(CPU* cpu, unsigned char* memory)
{
	cpu->Y++;
	cpu->P[1] = (cpu->Y == 0) ? 1 : 0;
	cpu->P[7] = (cpu->Y & 0x80) ? 1 : 0;
	cpu->cycles += 2;
	cpu->PC++;
}

void dey(CPU* cpu, unsigned char* memory)
{
	cpu->Y--;
	cpu->P[1] = (cpu->Y == 0) ? 1 : 0;
	cpu->P[7] = (cpu->Y & 0x80) ? 1 : 0;
	cpu->cycles += 2;
	cpu->PC++;
}

void tay(CPU* cpu, unsigned char* memory)
{
	cpu->Y = cpu->A;
	cpu->P[1] = (cpu->Y == 0) ? 1 : 0;
	cpu->P[7] = (cpu->Y & 0x80) ? 1 : 0;
	cpu->cycles += 2;
	cpu->PC++;
}

void tsy(CPU* cpu, unsigned char* memory)
{
	cpu->Y = cpu->A;
	cpu->P[1] = (cpu->Y == 0) ? 1 : 0;
	cpu->P[7] = (cpu->Y & 0x80) ? 1 : 0;
	cpu->cycles += 2;
	cpu->PC++;
}

void inx(CPU* cpu, unsigned char* memory)
{
	cpu->X++;
	cpu->P[1] = (cpu->X == 0) ? 1 : 0;
	cpu->P[7] = (cpu->X & 0x80) ? 1 : 0;
	cpu->cycles += 2;
	cpu->PC++;
}

void dex(CPU* cpu, unsigned char* memory)
{
	cpu->X--;
	cpu->P[1] = (cpu->X == 0) ? 1 : 0;
	cpu->P[7] = (cpu->X & 0x80) ? 1 : 0;
	cpu->cycles += 2;
	cpu->PC++;
}

void tax(CPU* cpu, unsigned char* memory)
{
	cpu->X=cpu->A;
	cpu->P[1] = (cpu->X == 0) ? 1 : 0;
	cpu->P[7] = (cpu->X & 0x80) ? 1 : 0;
	cpu->cycles += 2;
	cpu->PC++;
}

void tsx(CPU* cpu, unsigned char* memory)
{
	cpu->X = cpu->S;
	cpu->P[1] = (cpu->X == 0) ? 1 : 0;
	cpu->P[7] = (cpu->X & 0x80) ? 1 : 0;
	cpu->cycles += 2;
	cpu->PC++;
}
void txs(CPU* cpu, unsigned char* memory)
{
	cpu->S = cpu->X;
	cpu->cycles += 2;
	cpu->PC++;
}
void txa(CPU* cpu, unsigned char* memory)
{
	cpu->A = cpu->X;
	cpu->P[1] = (cpu->A == 0) ? 1 : 0;
	cpu->P[7] = (cpu->A & 0x80) ? 1 : 0;
	cpu->cycles += 2;
	cpu->PC++;
}

void tya(CPU* cpu, unsigned char* memory)
{
	cpu->A = cpu->Y;
	cpu->P[1] = (cpu->A == 0) ? 1 : 0;
	cpu->P[7] = (cpu->A & 0x80) ? 1 : 0;
	cpu->cycles += 2;
	cpu->PC++;
}

void rti(CPU* cpu, unsigned char* memory)
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

void lsr(CPU* cpu, unsigned char* memory)
{
	uint8_t shift_value;
	if (memory[cpu->PC] == 0x4A) {
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
		shift_value = fetch_data(cpu, memory);
		cpu->P[0] = shift_value & 0x1;
		shift_value = shift_value >> 1;
		cpu->P[1] = (shift_value == 0) ? 1 : 0;
		cpu->P[7] = (shift_value & 0x80) ? 1 : 0;
		cpu->memory[shift_address(cpu, memory)] = shift_value;
		cpu->cycles += 4;
		cpu->PC += address_bytes[memory[cpu->PC]] + 1;
	}
}

void asl(CPU* cpu, unsigned char* memory)
{
	uint8_t shift_value;
	if (memory[cpu->PC] == 0x0A) {
		cpu->P[0] = (cpu->A & 0x80) ? 1 : 0;
		cpu->A = cpu->A << 1;
		cpu->cycles += 2;
		cpu->PC += 1;
		cpu->P[1] = (cpu->A == 0) ? 1 : 0;
		cpu->P[7] = (cpu->A & 0x80) ? 1 : 0;
	}
	else
	{
		shift_value = fetch_data(cpu, memory);
		cpu->P[0] = (shift_value & 0x80) ? 1 : 0;
		shift_value = shift_value << 1;
		cpu->P[1] = (shift_value == 0) ? 1 : 0;
		cpu->P[7] = (shift_value & 0x80) ? 1 : 0;
		cpu->memory[shift_address(cpu, memory)] = shift_value;
		cpu->cycles += 4;
		cpu->PC += address_bytes[memory[cpu->PC]] + 1;
	}
}


void ror(CPU* cpu, unsigned char* memory)
{
	uint8_t shift_value;
	if (memory[cpu->PC] == 0x6A) {
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
		shift_value = fetch_data(cpu, memory);
		uint8_t oldP = cpu->P[0];
		cpu->P[0] = shift_value & 0x1;
		shift_value = shift_value >> 1 | (oldP << 7);
		cpu->P[1] = (shift_value == 0) ? 1 : 0;
		cpu->P[7] = (shift_value & 0x80) ? 1 : 0;
		cpu->memory[shift_address(cpu, memory)] = shift_value;
		cpu->cycles += 4;
		cpu->PC += address_bytes[memory[cpu->PC]] + 1;
	}
}

void inc(CPU* cpu, unsigned char* memory)
{
	uint8_t shift_value = fetch_data(cpu, memory) & 0xFF;
	uint8_t oldP = cpu->P[0];
	shift_value++;
	cpu->P[1] = (shift_value == 0) ? 1 : 0;
	cpu->P[7] = (shift_value & 0x80) ? 1 : 0;
	cpu->memory[shift_address(cpu, memory)] = shift_value;
	cpu->cycles += 4;
	cpu->PC += address_bytes[memory[cpu->PC]] + 1;

}

void bit(CPU* cpu, unsigned char* memory)
{
	uint8_t mem_val = fetch_data(cpu, memory) & 0xFF;
	uint8_t anded = mem_val & cpu->A;
	cpu->P[1] = (anded == 0) ? 1 : 0;
	cpu->P[6] = (mem_val & 0x60) ? 1 : 0;
	cpu->P[7] = (mem_val & 0x80) ? 1 : 0;
	cpu->cycles +=2;
	cpu->PC += address_bytes[memory[cpu->PC]] + 1;

}

void dec(CPU* cpu, unsigned char* memory)
{
	uint8_t shift_value = fetch_data(cpu, memory) & 0xFF;
	uint8_t oldP = cpu->P[0];
	shift_value--;
	cpu->P[1] = (shift_value == 0) ? 1 : 0;
	cpu->P[7] = (shift_value & 0x80) ? 1 : 0;
	cpu->memory[shift_address(cpu, memory)] = shift_value;
	cpu->cycles += 4;
	cpu->PC += address_bytes[memory[cpu->PC]] + 1;
}

void rol (CPU* cpu, unsigned char* memory)
{
	if (memory[cpu->PC] == 0x2A) {
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
		uint8_t shift_value = fetch_data(cpu, memory);
		uint8_t oldP = cpu->P[0];
		cpu->P[0] = (shift_value & 0x80) ? 1:0;
		shift_value = ((shift_value << 1 )| (oldP));
		cpu->P[1] = (shift_value == 0) ? 1 : 0;
		cpu->P[7] = (shift_value & 0x80) ? 1 : 0;
		cpu->memory[shift_address(cpu, memory)] = shift_value;
		cpu->cycles += 4;
		cpu->PC += address_bytes[memory[cpu->PC]] + 1;
	}
}


void kil(CPU* cpu, unsigned char* memory) {
	printf("CPU Address %x Cycle %d\n", cpu->PC, cpu->cycles);
	assert(0,"kil not implemented"); };
void slo(CPU* cpu, unsigned char* memory) 
{ 
	uint8_t shift_value = fetch_data(cpu, memory);
	cpu->P[0] = (shift_value & 0x80) ? 1 : 0;
	shift_value = shift_value << 1;
	cpu->A = cpu->A | shift_value;
	cpu->P[1] = (cpu->A == 0) ? 1 : 0;
	cpu->P[7] = (cpu->A & 0x80) ? 1 : 0;
	cpu->memory[shift_address(cpu, memory)] = shift_value;
	cpu->cycles +=4;
	//Because fuck that for now, this is the worst hack ever.
	if (memory[cpu->PC] == 0x1F)
		cpu->cycles--;
	cpu->PC += address_bytes[memory[cpu->PC]] + 1;
}

void brk(CPU* cpu, unsigned char* memory)
{
	cpu->P[2] = 1;

	uint16_t pc = cpu->PC+2;
	cpu->memory[0x100 | cpu->S] = (uint8_t)((pc& 0xFF00) >> 8);
	cpu->memory[0x100 | cpu->S - 1] = pc & 0x00FF;
	cpu->memory[0x100 | cpu->S - 2] = cpu->P;
	cpu->S -= 3;
	
	uint16_t newPC = cpu->memory[0xFFFF] << 8 | cpu->memory[0xFFFE];
	cpu->cycles += 7;
	cpu->PC = newPC;
}
void rra(CPU* cpu, unsigned char* memory)
{
	uint8_t shift_value = fetch_data(cpu, memory);
	uint8_t oldP = cpu->P[0];
	cpu->P[0] = shift_value & 0x1;
	shift_value = shift_value >> 1 | (oldP << 7);
	uint16_t temp = shift_value + cpu->A + cpu->P[0];
	cpu->P[1] = ((temp & 0xFF) == 0) ? 1 : 0;
	cpu->P[7] = (temp & 0x80) ? 1 : 0;
	cpu->P[6] = overflow(cpu->A, (uint8_t)(shift_value), (uint8_t)(temp));
	cpu->P[0] = (temp & 0x100) ? 1 : 0;
	cpu->A = (uint8_t)(temp & 0x0FF);
	cpu->memory[shift_address(cpu, memory)] = shift_value;
	cpu->cycles += 4;
	//Because fuck that for now, this is the worst hack ever.
	if (memory[cpu->PC] == 0x7F)
		cpu->cycles--;
	cpu->PC += address_bytes[memory[cpu->PC]] + 1;
}
void rla(CPU* cpu, unsigned char* memory)
{ 
	uint8_t shift_value = fetch_data(cpu, memory);

	uint8_t oldP = cpu->P[0];
	cpu->P[0] = (shift_value & 0x80) ? 1 : 0;
	shift_value = ((shift_value << 1) | (oldP));
	cpu->A = cpu->A & shift_value;
	cpu->P[1] = (cpu->A == 0) ? 1 : 0;
	cpu->P[7] = (cpu->A & 0x80) ? 1 : 0;
	cpu->memory[shift_address(cpu, memory)] = shift_value;
	cpu->cycles += 4;
	//Because fuck that for now, this is the worst hack ever.
	if (memory[cpu->PC] == 0x3F)
		cpu->cycles--;
	cpu->PC += address_bytes[memory[cpu->PC]] + 1;
}
void sax(CPU* cpu, unsigned char* memory) 
{
	uint16_t address = fetch_data(cpu, memory);
	cpu->memory[address] = cpu->A & cpu->X;
	cpu->PC += address_bytes[memory[cpu->PC]] + 1;
	cpu->cycles += 2;
}
void sre(CPU* cpu, unsigned char* memory) 
{
	uint8_t shift_value = fetch_data(cpu, memory);
	cpu->P[0] = shift_value & 0x1;
	shift_value = shift_value >> 1;
	cpu->A = cpu->A ^ shift_value;
	cpu->P[1] = (cpu->A == 0) ? 1 : 0;
	cpu->P[7] = (cpu->A & 0x80) ? 1 : 0;
	cpu->memory[shift_address(cpu, memory)] = shift_value;
	cpu->cycles += 4;
	//Because fuck that for now, this is the worst hack ever.
	if (memory[cpu->PC] == 0x5F)
		cpu->cycles--;
	cpu->PC += address_bytes[memory[cpu->PC]] + 1;
};
void ahx(CPU* cpu, unsigned char* memory) { assert(0, "ahx not implemented"); };
void lax(CPU* cpu, unsigned char* memory) 
{
	uint16_t pc = cpu->PC;
	uint16_t data = fetch_data(cpu, memory);
	cpu->P[1] = (data == 0) ? 1 : 0;
	cpu->P[7] = (data & 0x80) ? 1 : 0;
	cpu->X = (uint8_t)(data & 0xFF);
	cpu->A = (uint8_t)(data & 0xFF);
	cpu->PC += address_bytes[memory[cpu->PC]] + 1;
	cpu->cycles += 2;
}

void dcp(CPU* cpu, unsigned char* memory)
{
	uint8_t shift_value = fetch_data(cpu, memory) & 0xFF;
	uint8_t oldP = cpu->P[0];
	shift_value--;
	cpu->P[0] = (cpu->A >= shift_value) ? 1 : 0;
	cpu->P[1] = (cpu->A == shift_value) ? 1 : 0;
	cpu->P[7] = ((cpu->A & (cpu->A-shift_value)) & 0x80) ? 1 : 0;
	cpu->memory[shift_address(cpu, memory)] = shift_value;
	cpu->cycles += 4;
	//Because fuck that for now, this is the worst hack ever.
	if (memory[cpu->PC] == 0xDF)
		cpu->cycles--;
	cpu->PC += address_bytes[memory[cpu->PC]] + 1;
}

void isc(CPU* cpu, unsigned char* memory) 
{
	uint8_t shift_value = fetch_data(cpu, memory) & 0xFF;
	uint8_t oldP = cpu->P[0];
	shift_value++;
	uint16_t temp = cpu->A - shift_value - (1 - cpu->P[0]);
	cpu->P[1] = ((temp & 0xFF) == 0) ? 1 : 0;
	cpu->P[7] = (temp & 0x80) ? 1 : 0;
	cpu->P[6] = overflowsbc(cpu->A, (uint8_t)(shift_value), (uint8_t)(temp));
	cpu->P[0] = (temp & 0x100) ? 0 : 1;
	cpu->A = (uint8_t)(temp & 0x0FF);
	cpu->memory[shift_address(cpu, memory)] = shift_value;
	cpu->cycles += 4;
	//Because fuck that for now, this is the worst hack ever.
	if (memory[cpu->PC] == 0xFF)
		cpu->cycles--;
	cpu->PC += address_bytes[memory[cpu->PC]] + 1;
}

void anc(CPU* cpu, unsigned char* memory) { assert(0,"anc not implemented"); };
void cli(CPU* cpu, unsigned char* memory) { assert(0, "cli not implemented"); };
void las(CPU* cpu, unsigned char* memory) { assert(0, "las not implemented"); };
void arr(CPU* cpu, unsigned char* memory) { assert(0, "arr not implemented"); };
void tas(CPU* cpu, unsigned char* memory) { assert(0, "tas not implemented"); };
void alr(CPU* cpu, unsigned char* memory) { assert(0, "alr not implemented"); };
void xaa(CPU* cpu, unsigned char* memory) { assert(0, "xaa not implemented"); };
void axs(CPU* cpu, unsigned char* memory) { assert(0, "axs not implemented"); };
void shy(CPU* cpu, unsigned char* memory) { assert(0, "shy not implemented"); };
void shx(CPU* cpu, unsigned char* memory) { assert(0, "shx not implemented"); };


void(*instruction[256])(CPU*, unsigned char*) = { 
brk ,  ora ,  kil ,  slo ,  nop ,  ora ,  asl ,  slo ,  php ,  ora ,  asl ,  anc ,  nop ,  ora ,  asl ,  slo ,
bpl ,  ora ,  kil ,  slo ,  nop ,  ora ,  asl ,  slo ,  clc ,  ora ,  nop ,  slo ,  nop ,  ora ,  asl ,  slo ,
jsr ,  and ,  kil ,  rla ,  bit ,  and ,  rol ,  rla ,  plp ,  and ,  rol ,  anc ,  bit ,  and ,  rol ,  rla ,
bmi ,  and ,  kil ,  rla ,  nop ,  and ,  rol ,  rla ,  sec ,  and ,  nop ,  rla ,  nop ,  and ,  rol ,  rla ,
rti ,  eor ,  kil ,  sre ,  nop ,  eor ,  lsr ,  sre ,  pha ,  eor ,  lsr ,  alr ,  jmp ,  eor ,  lsr ,  sre ,
bvc ,  eor ,  kil ,  sre ,  nop ,  eor ,  lsr ,  sre ,  cli ,  eor ,  nop ,  sre ,  nop ,  eor ,  lsr ,  sre ,
rts ,  adc ,  kil ,  rra ,  nop ,  adc ,  ror ,  rra ,  pla ,  adc ,  ror ,  arr ,  jmp ,  adc ,  ror ,  rra ,
bvs ,  adc ,  kil ,  rra ,  nop ,  adc ,  ror ,  rra ,  sei ,  adc ,  nop ,  rra ,  nop ,  adc ,  ror ,  rra ,
nop ,  sta ,  nop ,  sax ,  sty ,  sta ,  stx ,  sax ,  dey ,  nop ,  txa ,  xaa ,  sty ,  sta ,  stx ,  sax ,
bcc ,  sta ,  kil ,  ahx ,  sty ,  sta ,  stx ,  sax ,  tya ,  sta ,  txs ,  tas ,  shy ,  sta ,  shx ,  ahx ,
ldy ,  lda ,  ldx ,  lax ,  ldy ,  lda ,  ldx ,  lax ,  tay ,  lda ,  tax ,  lax ,  ldy ,  lda ,  ldx ,  lax ,
bcs ,  lda ,  kil ,  lax ,  ldy ,  lda ,  ldx ,  lax ,  clv ,  lda ,  tsx ,  las ,  ldy ,  lda ,  ldx ,  lax ,
cpy ,  cmp ,  nop ,  dcp ,  cpy ,  cmp ,  dec ,  dcp ,  iny ,  cmp ,  dex ,  axs ,  cpy ,  cmp ,  dec ,  dcp ,
bne ,  cmp ,  kil ,  dcp ,  nop ,  cmp ,  dec ,  dcp ,  cld ,  cmp ,  nop ,  dcp ,  nop ,  cmp ,  dec ,  dcp ,
cpx ,  sbc ,  nop ,  isc ,  cpx ,  sbc ,  inc ,  isc ,  inx ,  sbc ,  nop ,  sbc ,  cpx ,  sbc ,  inc ,  isc ,
beq ,  sbc ,  kil ,  isc ,  nop ,  sbc ,  inc ,  isc ,  sed ,  sbc ,  nop ,  isc ,  nop ,  sbc ,  inc ,  isc };

void initiate_cpu(CPU *cpu,unsigned char* buffer, int size, PPU* ppu)
{
	for (int i = 0; i <= 0xFFFF; i++)
	{
		cpu->memory[i] = 0;
	}
	cpu->ppu = ppu;
#ifdef NESTEST
	cpu->PC=0x0000;
#endif
	cpu->sl = 0;
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
	cpu->dma_wait = 0;
	cpu->dma_wait_cycles = 0;
	cpu->vram_buff = 0;
	printf("SSSSize %d\n", size);
	for (int i = 0x2000; i <= 0x3FFF; i++)
		cpu->memory[i] = 0;
	if (size == 16384) {
		printf("16kb one\n");
		memcpy(&(cpu->memory[0x8000]), buffer, size);
		memcpy(&(cpu->memory[0xC000]), buffer, size);
	}
	else {
		memcpy(&(cpu->memory[0x8000]), buffer, size);
	}
	cpu->PC = cpu->memory[0xFFFC] | cpu->memory[0xFFFD] << 8;
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
	printf("A:%x X:%x Y:%x P:%x SP:%x SL:%d CYC:%d", cpu->A, cpu->X, cpu->Y, get_status_flag(cpu->P), cpu->S, cpu->sl, cpu->cycles);//,((cpu->cycles*3)%341));
}

void debug_inst(CPU *cpu, unsigned char *memory)
{
  char* instruction = inst_name[memory[cpu->PC]];
  printf("%x ",cpu->PC);
  switch(addressing_mode[memory[cpu->PC]])
  {
    case 0:
	printf("%s \t",instruction);
	get_cpu_state(cpu);
	break;
    case 1:
        printf("%s #%x\t",instruction, memory[cpu->PC+1]);
	get_cpu_state(cpu);
	break;
    case 2:
	printf("%s $%x\t",instruction, memory[cpu->PC+1]);
	get_cpu_state(cpu);
	break;
    case 3:
        printf("%s $%x,X\t",instruction,memory[cpu->PC+1]);
	get_cpu_state(cpu);
	break;
    case 4:
        printf("%s $%x,Y\t",instruction,memory[cpu->PC+1]);
	get_cpu_state(cpu);
	break;
    case 5:
        printf("%s ($%x),X\t",instruction,memory[cpu->PC+1]);
	get_cpu_state(cpu);
	break;
    case 6:
        printf("%s ($%x),Y\t",instruction,memory[cpu->PC+1]);
	get_cpu_state(cpu);
	break;
    case 7:
        printf("%s $%2x%2x\t",instruction,memory[cpu->PC+2],memory[cpu->PC+1]);
	get_cpu_state(cpu);
	break;
    case 8:
        printf("%s $%2x%2x,X\t",instruction,memory[cpu->PC+2],memory[cpu->PC+1]);
	get_cpu_state(cpu);
	break;
    case 9:
        printf("%s $%2x%2x,Y\t",instruction,memory[cpu->PC+2],memory[cpu->PC+1]);
	get_cpu_state(cpu);
	break;
    case 10:
        printf("%s $(%x%x)\t",instruction,memory[cpu->PC+2],memory[cpu->PC+1]);
	get_cpu_state(cpu);
	break;
    case 11:
        printf("%s %x\t",instruction,memory[cpu->PC+1]);
	get_cpu_state(cpu);
	break;
	case 12:
		printf("%s $%x\t", instruction, memory[cpu->PC + 1]);
		get_cpu_state(cpu);
		break;
	case 13:
		printf("%s $%x%x\t", instruction, memory[cpu->PC + 2], memory[cpu->PC + 1]);
		get_cpu_state(cpu);
		break;
	case 14:
		printf("%s $%x%x,X\t", instruction, memory[cpu->PC + 2], memory[cpu->PC + 1]);
		get_cpu_state(cpu);
		break;
	case 15:
		printf("%s $%x%x,Y\t", instruction, memory[cpu->PC + 2], memory[cpu->PC + 1]);
		get_cpu_state(cpu);
		break;
	case 16:
		printf("%s $%x,X\t", instruction, memory[cpu->PC + 1]);
		get_cpu_state(cpu);
		break;
	case 17:
		printf("%s $%x,Y\t", instruction, memory[cpu->PC + 1]);
		get_cpu_state(cpu);
		break;
	case 18:
		printf("%s ($%x),X\t", instruction, memory[cpu->PC + 1]);
		get_cpu_state(cpu);
		break;
	case 19:
		printf("%s ($%x),Y\t", instruction, memory[cpu->PC + 1]);
		get_cpu_state(cpu);
		break;
  }
}

void nmi(CPU *cpu)
{
	uint16_t pc = cpu->PC;
	cpu->memory[0x100 | cpu->S] = (uint8_t)((pc & 0xFF00) >> 8);
	cpu->memory[0x100 | cpu->S - 1] = pc & 0x00FF;
	cpu->memory[0x100 | cpu->S - 2] = cpu->P;
	cpu->S -= 3;
	cpu->PC = cpu->memory[0xFFFA] | cpu->memory[0xFFFB] << 8;
	cpu->cycles += 7;
}

void decode_and_execute(CPU *cpu)
{
	int i = 0;
	int prev_sl = cpu->sl;
	//while (prev_sl == cpu->sl) {
//	debug_inst(cpu, cpu->memory);

		uint8_t instOp = cpu->memory[cpu->PC];
		instruction[instOp](cpu, cpu->memory);
		cpu->sl = ((242 + ((cpu->cycles * 3) / 341)) % 262) - 1;
//	printf("\n");
	//}
}