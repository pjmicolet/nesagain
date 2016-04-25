#include "SDL.h"
#ifndef __PPU_INCLUDED_H__ 
#define __PPU_INCLUDED_H__

typedef struct PPU
{
	uint8_t* PPUCTRL;
	uint8_t* PPUMASK;
	uint8_t* PPUSTATUS;
	uint8_t* OAMADDR;
	uint8_t* OAMDATA;
	uint8_t* PPUSCROLL;
	uint8_t* PPUADDR;
	uint8_t* PPUDATA;
	uint8_t* OAMDMA;
	uint8_t nmiOcc;
	uint8_t nmiOutput;
	uint8_t memory[0x10000];
	uint16_t vram_addr;
	uint8_t write_flip;
	uint16_t regT;
	uint16_t regV;
	uint8_t regFX;
	uint64_t ppu_cycle;
	SDL_Renderer* renderer;
	SDL_Window* window;
	SDL_Surface* screen;
	SDL_Texture* texture;
	uint32_t pixel_colour[61440];
	uint8_t evenOdd;
	uint8_t ppuNMI;
	int scanline;
	int ppuFrames;
	uint8_t OAM[2][256];
	uint8_t n;
	uint8_t spriteindex;
	uint8_t spritesdrawn;
	uint8_t oamindex;
	uint8_t oambuffer[4];
	uint8_t keep_reading;
	uint8_t vertical_mirroring;
	uint8_t horizontal_mirroring;
	uint8_t buffer;
	uint8_t oldBufer;
} PPU;

typedef struct colour_t
{
	uint8_t r;
	uint8_t g;
	uint8_t b;
} colour_t;


SDL_Renderer* make_screen(PPU *ppu);
colour_t getColour(PPU *ppu, int x, int scanline);
colour_t getSpriteColour(PPU* ppu, int x, uint16_t tile_addr, uint8_t attr_tile);
uint8_t is_opaque(PPU *ppu, int x, int scanline);
void read_vram_data(PPU *ppu);
void initialize_ppu(PPU *ppu, unsigned char *memory, unsigned char *chrbuffer, int size, int mirroring);
void print_status(PPU *ppu);
void render(PPU *ppu, int16_t scanline,int cycles);
void write_vram_addr(PPU* ppu, uint8_t addr);
void write_vram_data(PPU* ppu, uint8_t data);
void write_controll_reg(PPU* ppu, uint8_t data);
void write_scroll_reg(PPU* ppu, uint16_t data);
void read_controller(PPU* ppu);
void status_read(PPU* ppu);
void sprite_data(PPU* ppu);
void closeWindow(PPU *ppu);
#endif