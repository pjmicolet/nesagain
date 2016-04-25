#include <stdint.h>
#include "ppu.h"

#ifndef CPU_H
#define CPU_H

typedef struct CPU
{
  uint8_t A;
  uint8_t X,Y;
  uint16_t PC;
  uint8_t S;
  uint8_t P[8];
  unsigned char memory[0x10000];
  // We dont need to keep accumulating really.
  uint64_t cycles;
  int16_t sl;
  PPU* ppu;
  uint8_t dma_wait;
  uint8_t dma_wait_cycles;
  uint8_t vram_buff;
} CPU;

uint8_t get_status_flag(uint8_t pflag[]);
void initiate_cpu(CPU *cpu, unsigned char* buffer, int size, PPU* ppu);
void decode_and_execute(CPU *cpu);
void nmi(CPU *cpu);

#endif