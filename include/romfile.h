#include <stdint.h>


struct romFile
{
  uint8_t prgRomSize;
  uint8_t chrRomSize;
  uint8_t prgRamSize;
};
  


int loadRom(unsigned char ** buffer);
