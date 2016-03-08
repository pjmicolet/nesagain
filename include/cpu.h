#include <stdint.h>
typedef struct CPU
{
  uint8_t A;
  uint8_t X,Y;
  uint16_t PC;
  uint8_t S;
  uint8_t P;
} CPU;

int enter_cpu(unsigned char *buffer);
