#include "romfile.h"
#include "cpu.h"
#include "ppu.h"

#include<stdio.h>
#include<stdlib.h>
int main(int argc, char **argv)
{
	unsigned char* buffer;
	unsigned char* chrbuffer;
	int mirroring = 0;
	size_t size[2];
	loadRom(&buffer, &chrbuffer, size, &mirroring);
	//Make this into ifdef debug statements
	printf("BL %x CF %x\n", buffer[size[0] - 1], chrbuffer[0]);
	printf("Size %d %d\n", size[0], size[1]);
	CPU cpu;
	PPU ppu;
	initiate_cpu(&cpu, buffer, size[0], &ppu);
	initialize_ppu(&ppu, cpu.memory, chrbuffer, size[1],mirroring);

/* 
	Stuff for FPS calculation
	Uint32 prev_frame = SDL_GetTicks();
	Uint32 frame = 0;
	Uint32 times[3000];
	int old_frame = 0;

*/
	while (1)
	{
		if (cpu.dma_wait_cycles > 0)
		{
			cpu.dma_wait_cycles--;
			cpu.cycles++;
			cpu.sl = ((242 + ((cpu.cycles * 3) / 341)) % 262) - 1;
		}
		else
		{
			decode_and_execute(&cpu);
		}
		render(&ppu, cpu.sl, cpu.cycles);
		if ((ppu.nmiOcc) && (ppu.nmiOutput) && ppu.ppuNMI)
		{
			nmi(&cpu);
			ppu.ppuNMI = 0;
		}
	}
  closeWindow(&ppu);
  return 0;
}
