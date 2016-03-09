#include <stdint.h>
typedef struct CPU
{
  uint8_t A;
  uint8_t X,Y;
  uint16_t PC;
  uint8_t S;
  // Little endianess, so P[7] will be bit 0
  uint8_t P[8];
  uint8_t memory[0x10000];
  // We dont need to keep accumulating really.
  uint64_t cycles;
} CPU;

int enter_cpu(unsigned char *buffer);
