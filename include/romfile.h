#include <stdint.h>
#ifdef __linux__
#include <stdlib.h>
#endif

/* To be developed */
struct romFile
{
  uint8_t prgRomSize;
  uint8_t chrRomSize;
  uint8_t prgRamSize;
};
 
void loadRom(unsigned char ** memory, unsigned char ** chrmem, size_t results[], int* mirroring);
