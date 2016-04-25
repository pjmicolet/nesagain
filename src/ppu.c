#include "ppu.h"
#include <assert.h>
#include<stdio.h>
#include <windows.h>

const int SCREEN_WIDTH = 256;
const int SCREEN_HEIGHT = 240;

//Screen dimension constants

colour_t palette[64] = {
{ 0x80, 0x80, 0x80}, { 0x00,0x00,0xBB }, { 0x37,0x00,0xBF }, { 0x84,0x00,0xA6 },
{ 0xBB,0x00,0x6A }, { 0xB7,0x00,0x1E }, { 0xB3,0x00,0x00 }, { 0x91,0x26,0x00 },
{ 0x7B,0x2B,0x00 }, { 0x00,0x3E,0x00 }, { 0x00,0x48,0x0D }, { 0x00,0x3C,0x22 },
{ 0x00,0x2F,0x66 }, { 0x00,0x00,0x00 }, { 0x05,0x05,0x05 }, { 0x05,0x05,0x05 },

{ 0xC8,0xC8,0xC8 }, { 0x00,0x59,0xFF }, { 0x44,0x3C,0xFF }, { 0xB7,0x33,0xCC },
{ 0xFF,0x33,0xAA }, { 0xFF,0x37,0x5E }, { 0xFF,0x37,0x1A }, { 0xD5,0x4B,0x00 },
{ 0xC4,0x62,0x00 }, { 0x3C,0x7B,0x00 }, { 0x1E,0x84,0x15 }, { 0x00,0x95,0x66 },
{ 0x00,0x84,0xC4 }, { 0x11,0x11,0x11 }, { 0x09,0x09,0x09 }, { 0x09,0x09,0x09 },

{ 0xFF,0xFF,0xFF }, { 0x00,0x95,0xFF }, { 0x6F,0x84,0xFF }, { 0xD5,0x6F,0xFF },
{ 0xFF,0x77,0xCC }, { 0xFF,0x6F,0x99 }, { 0xFF,0x7B,0x59 }, { 0xFF,0x91,0x5F },
{ 0xFF,0xA2,0x33 }, { 0xA6,0xBF,0x00 }, { 0x51,0xD9,0x6A }, { 0x4D,0xD5,0xAE },
{ 0x00,0xD9,0xFF }, { 0x66,0x66,0x66 }, { 0x0D,0x0D,0x0D }, { 0x0D,0x0D,0x0D },

{ 0xFF,0xFF,0xFF }, { 0x84,0xBF,0xFF }, { 0xBB,0xBB,0xFF }, { 0xD0,0xBB,0xFF },
{ 0xFF,0xBF,0xEA }, { 0xFF,0xBF,0xCC }, { 0xFF,0xC4,0xB7 }, { 0xFF,0xCC,0xAE },
{ 0xFF,0xD9,0xA2 }, { 0xCC,0xE1,0x99 }, { 0xAE,0xEE,0xB7 }, { 0xAA,0xF7,0xEE },
{ 0xB3,0xEE,0xFF }, { 0xDD,0xDD,0xDD }, { 0x11,0x11,0x11 }, { 0x11,0x11,0x11 }
};

void initialize_ppu(PPU *ppu, unsigned char *memory, unsigned char *chrbuffer, int size, int mirroring)
{
	ppu->PPUCTRL = &(memory[0x2000]);
	ppu->PPUMASK = &(memory[0x2001]);
	ppu->PPUSTATUS = &(memory[0x2002]);
	ppu->OAMADDR = &(memory[0x2003]);
	ppu->OAMDATA = &(memory[0x2004]);
	ppu->PPUSCROLL = &(memory[0x2005]);
	ppu->PPUADDR = &(memory[0x2006]);
	ppu->PPUDATA = &(memory[0x2007]);
	ppu->OAMDMA = &(memory[0x4014]);
	for (int i = 0; i <= 0xFFFF; i++)
		ppu->memory[i] = 0;
	for (int i = 0; i < 256; i++)
		ppu->OAM[0][i] = 0;
	if (size > 0)
		memcpy(&(ppu->memory[0]), chrbuffer, size);
	ppu->vram_addr = 0;
	ppu->write_flip = 0x00;
	ppu->regV = 0;
	ppu->regT = 0;
	ppu->regFX = 0;
	ppu->ppu_cycle = 0;
	ppu->renderer = make_screen(ppu);
	ppu->scanline = 0;
	ppu->nmiOutput = 0;
	ppu->nmiOcc = 0;
	ppu->evenOdd = 0;
	ppu->ppuNMI = 0;
	ppu->ppuFrames= 0;
	ppu->oamindex = 0;
	ppu->n = 0;
	ppu->spriteindex = 0;
	ppu->spritesdrawn = 0;
	ppu->keep_reading = 1;
	if (mirroring == 1)
	{
		ppu->vertical_mirroring = 1;
		ppu->horizontal_mirroring = 0;
	}
	else if (mirroring == 2)
	{
		ppu->vertical_mirroring = 0;
		ppu->horizontal_mirroring = 1;
	}
	ppu->buffer = 0;
}

void print_status(PPU *ppu)
{
	printf("PPUCTRL %x PPUMASK %x PPUSTATUS %x OAMADDR %x OAMDATA %x PPUSCROLL %x PPUADDR %x PPUDATA %x OAMDMA %x\n",
		ppu->PPUCTRL, ppu->PPUMASK, ppu->PPUSTATUS, ppu->OAMADDR, ppu->OAMDATA, ppu->PPUSCROLL, ppu->PPUADDR, ppu->PPUDATA, ppu->OAMDMA);
}

void update_coarse_x(PPU* ppu)
{
	if ((ppu->regV & 0x001F) == 31)
	{
		ppu->regV &= ~0x001F;
		ppu->regV ^= 0x0400;
	}
	else {
		ppu->regV += 1;
	}
}

void update_fine_y(PPU* ppu)
{
	if ((ppu->regV & 0x7000) != 0x7000)
		ppu->regV += 0x1000;
	else
	{
		ppu->regV &= ~0x7000;
		int y = (ppu->regV & 0x03E0) >> 5;
		if (y == 29)
		{
			y = 0;
			ppu->regV ^= 0x0800;
		}
		else if (y == 31)
		{
			y = 0;
		}
		else
		{
			y += 1;
		}
		ppu->regV = (ppu->regV & ~0x03E0) | (y << 5);
	}
}

void drawPixel(PPU *ppu, int scanline)
{
	colour_t colour = getColour(ppu, ((((ppu->ppu_cycle) % 341)) % 8 ), scanline);
	int x = (ppu->ppu_cycle)% 341;
	ppu->pixel_colour[(x-1)+(scanline*256)] = 0xFF000000 |(int)(colour.r) | (int)(colour.g << 8 ) |(int) (colour.b)<<16;
}

void drawSprite(PPU *ppu, int scanline, uint16_t tile_addr, uint8_t attr_tile,uint8_t xpos)
{
	colour_t colour = getSpriteColour(ppu, ((((ppu->ppu_cycle) % 341) - 1) % 8), tile_addr, attr_tile);
	int x = ((((ppu->ppu_cycle) % 341) - 1) % 8) + xpos;
	ppu->pixel_colour[(x) + ((scanline*256))] = 0xFF000000 | (int)(colour.r) | (int)(colour.g << 8) | (int)(colour.b) << 16;
}

uint16_t flip_byte(uint16_t byte)
{
	uint8_t b1 = (byte & 0xFF00) >> 8;
	uint8_t b2 = (byte & 0x00FF);
	b1 = (b1 & 0xF0) >> 4 | (b1 & 0x0F) << 4;
	b1 = (b1 & 0xCC) >> 2 | (b1 & 0x33) << 2;
	b1 = (b1 & 0xAA) >> 1 | (b1 & 0x55) << 1;
	b2 = (b2 & 0xF0) >> 4 | (b2 & 0x0F) << 4;
	b2 = (b2 & 0xCC) >> 2 | (b2 & 0x33) << 2;
	b2 = (b2 & 0xAA) >> 1 | (b2 & 0x55) << 1;
	return (b1 << 8) | b2;
}

void render(PPU *ppu, int16_t scanline, int cycle)
{
	ppu->scanline = scanline;
	int renderNow = 0;
	while (ppu->ppu_cycle  <= ((cycle)*3))	
	{
		if (((ppu->ppu_cycle % 341) == 0) && (scanline == 0) && (ppu->evenOdd))
		{
			ppu->ppu_cycle++;
			continue;
		}
		if (ppu->ppu_cycle % 341 == 0)
		{
			ppu->ppu_cycle++;
			continue;
		}
		if ((*(ppu->PPUMASK) & 0x8))
		{
			if ((scanline > -1) && (scanline < 240))
			{	
				if ((ppu->ppu_cycle % 341) >= 1 && (ppu->ppu_cycle % 341) <= 256)
				{

					if ((ppu->ppu_cycle % 341) % 8 == 0)
					{
						update_coarse_x(ppu);
					}
					drawPixel(ppu, scanline);
					if ((ppu->ppu_cycle % 341) % 256 == 0)
					{
						update_fine_y(ppu);
					}
				}
				if ((ppu->ppu_cycle % 341) % 257 == 0)
				{
					ppu->regV = (ppu->regT & 0x41F) | (ppu->regV & ~(0x41f));
				}
			    if (((ppu->ppu_cycle % 341) == 328) || ((ppu->ppu_cycle % 341) == 336))
				{
					//Prefetch needs to be implemented
					//update_coarse_x(ppu);
				}
			}
			else if ((scanline >= 240) && (scanline <= 260))
			{
				if ((scanline == 241) && ((ppu->ppu_cycle % 341) == 1))
				{
					renderNow = 1;
					*(ppu->PPUSTATUS) |= 0x80;
					ppu->nmiOcc = 1;
					ppu->ppuNMI = 1;
				}
			}
			else if ((scanline == -1))
			{
				if ((ppu->ppu_cycle % 341) == 1)
				{
					*(ppu->PPUSTATUS) &= 0x7F;
					ppu->nmiOcc = 0;
				}

				if ((ppu->ppu_cycle % 341) >= 1 && (ppu->ppu_cycle % 341) <= 256)
				{
					if ((ppu->ppu_cycle % 341) % 256 == 0)
					{
						update_fine_y(ppu);
					}
					if ((ppu->ppu_cycle % 341) % 8 == 0)
					{
						update_coarse_x(ppu);
					}
				}
				if ((ppu->ppu_cycle % 341) % 257 == 0)
				{
					ppu->regV = (ppu->regT & 0x41F)| (ppu->regV & ~(0x41f));
				}
				if (((ppu->ppu_cycle % 341) >= 280) && ((ppu->ppu_cycle % 341) <= 304))
				{
					ppu->regV = (ppu->regT & 0x7BE0) | (ppu->regV & ~(0x7BE0));
				}
				if (((ppu->ppu_cycle % 341) == 328 )|| ((ppu->ppu_cycle % 341) == 336))
				{
					//prefetch needs to be implemented;
				    //update_coarse_x(ppu);
				}
			}
		}
		if (*(ppu->PPUMASK) & 0x10)
		{
			if (scanline == -1)
			{
				if (((ppu->ppu_cycle) % 341) == 1)
				{
					*(ppu->PPUSTATUS) &= 0xDF;
				}
			}
			if (scanline == 241)
			{
				if (((ppu->ppu_cycle) % 341) == 1)
				{
					*(ppu->PPUSTATUS) &= 0x9F;
					ppu->spriteindex = 0;
					ppu->n = 0;
					ppu->oamindex = 0;
					ppu->spritesdrawn = 0;
					ppu->keep_reading = 1;
				}
			}
			if ((scanline > 0) && (scanline < 240))
			{
				if (((ppu->ppu_cycle % 341) > 0) && ((ppu->ppu_cycle % 341) <= 64))
				{
					ppu->OAM[1][((ppu->ppu_cycle % 341) - 1)*4] = 0xFF;
					ppu->OAM[1][((ppu->ppu_cycle % 341) - 1) * 4+1] = 0xFF;
					ppu->OAM[1][((ppu->ppu_cycle % 341) - 1) * 4+2] = 0xFF;
					ppu->OAM[1][((ppu->ppu_cycle % 341) - 1) * 4+3] = 0xFF;
				}
				else if (((ppu->ppu_cycle % 341) > 64) && ((ppu->ppu_cycle % 341) <= 256))
				{
					if (((ppu->ppu_cycle % 341) % 2) == 1)
					{
						uint8_t y = ppu->OAM[0][ppu->n * 4];
						if (ppu->spritesdrawn <9 && ppu->keep_reading)
						{
							ppu->OAM[1][ppu->oamindex * 4] = ppu->OAM[0][ppu->n * 4]+1;
							if ((scanline > (y)) && (scanline <= (y + 8)))
							{
								ppu->OAM[1][ppu->oamindex * 4 + 1] = ppu->OAM[0][(ppu->n ) * 4 + 1];
								ppu->OAM[1][ppu->oamindex * 4 + 2] = ppu->OAM[0][(ppu->n ) * 4 + 2];
								ppu->OAM[1][ppu->oamindex * 4 + 3] = ppu->OAM[0][(ppu->n ) * 4 + 3];
								ppu->oamindex++;
								ppu->spritesdrawn++;
								if (ppu->spritesdrawn == 9)
								{
									*(ppu->PPUSTATUS) |= 0x20;
								}
							}
						}
						ppu->n = (ppu->n + 1) % 64;
						if (ppu->n == 0)
						{
							ppu->keep_reading = 0;
						}
					}
				}
				else if (((ppu->ppu_cycle % 341) > 256) && ((ppu->ppu_cycle % 341) <= 320))
				{
					if ((ppu->ppu_cycle % 341) == 257)
					{
						ppu->spriteindex = 0;
						ppu->n = 1;
					}
					if ((ppu->n % 9) == 0)
					{
						ppu->n = 1;
						ppu->spriteindex++;
					}	
					uint8_t y_pos = ppu->OAM[1][ppu->spriteindex * 4];
					uint8_t tile_num = ppu->OAM[1][ppu->spriteindex * 4 + 1];
					uint8_t attribute = ppu->OAM[1][ppu->spriteindex * 4 + 2];
					uint8_t x_pos = ppu->OAM[1][ppu->spriteindex * 4 + 3];
					uint16_t which_bank = 0;
					if (*(ppu->PPUCTRL) & 0x20)
					{
						//16 bit sprite
						which_bank = (tile_num & 0x1) << 13;
						tile_num >>= 1;
					}
					else
					{
						which_bank = ((*(ppu->PPUCTRL) & 0x8)) << 9;
					}
					uint16_t tile_addr = which_bank | tile_num << 4 | ((((scanline) - (y_pos))) & 0x7);

					uint16_t pattern_d1 = ppu->memory[tile_addr];
					uint16_t pattern_d2 = ppu->memory[tile_addr + 8];
					int x = (((ppu->ppu_cycle) % 341) - 1) % 8;
					if (attribute & 0x40)
					{
						pattern_d1 = flip_byte(pattern_d1);
						pattern_d2 = flip_byte(pattern_d2);
					}
					pattern_d1 >>= 7 - x;
					pattern_d1 &= 1;
					pattern_d2 >>= 7 - x;
					pattern_d2 &= 1;
					uint8_t pix_index = (pattern_d1 | (pattern_d2 << 1));
					if ((ppu->spriteindex == 0) && pix_index && (*(ppu->PPUMASK)&0x10) && (*(ppu->PPUMASK) & 0x08) && is_opaque(ppu, ((x%356)%8 -1), scanline) && (x != 255))
					{
						*(ppu->PPUSTATUS) |= 0x40;
					}
					if (!(attribute & 0x20) && pix_index) {
						drawSprite(ppu, scanline, tile_addr, attribute, x_pos);
					}
					ppu->n++;
				}
				else
				{
					ppu->spriteindex = 0;
					ppu->n = 0; 
					ppu->oamindex = 0;
					ppu->spritesdrawn = 0;
					ppu->keep_reading = 1;
				}
			}

		}
		else if(!(*(ppu->PPUMASK) & 0x18))
		{
			if ((scanline > -1) && (scanline < 240))
			{
				if (ppu->ppu_cycle % 341 == 0)
				{
					ppu->ppu_cycle++;
					continue;
				}
			}
			else if (((scanline >= 240) && (scanline <= 260)))
			{
				if ((scanline == 241) && ((ppu->ppu_cycle % 341) == 1)){
					*(ppu->PPUSTATUS) |= 0x80; 
					*(ppu->PPUSTATUS) &= 0xBF;
					ppu->nmiOcc = 1;
					ppu->ppuNMI = 1;
				}

			}
			else if( ((scanline == -1)))
			{
				if ((ppu->ppu_cycle % 341) == 1)
				{
					*(ppu->PPUSTATUS) &= 0x7F;
					ppu->nmiOcc = 0;
				}
			}
		}
		if (scanline == 241 && (ppu->ppu_cycle %341 == 0))
		{
			ppu->evenOdd = ~ppu->evenOdd;
		}
		ppu->ppu_cycle++;
	}
	if ((*(ppu->PPUMASK) & 0x18) && (scanline == 241) && renderNow)
	{
		SDL_UpdateTexture(ppu->texture, NULL, ppu->pixel_colour, 256*4);
		SDL_RenderCopy(ppu->renderer, ppu->texture, NULL, NULL);
		SDL_RenderPresent(ppu->renderer);
		SDL_PumpEvents();
		ppu->ppuFrames++;
		renderNow = 0;
	}
}

uint8_t is_opaque(PPU *ppu, int x, int scanline)
{
	uint8_t pattern_table = ppu->memory[0x2000 | (ppu->regV & 0x0FFF)];
	uint8_t attribute = ppu->memory[0x23C0 | (ppu->regV & 0x0C00) | ((ppu->regV >> 4) & 0x38) | (ppu->regV >> 2) & 0x07];
	uint8_t offset_y = (ppu->regV >> 12) & 0x7;
	uint16_t tile_addr = ((*(ppu->PPUCTRL) & 0x10) >> 4) << 12;
	tile_addr |= (uint16_t)(pattern_table) << 4;
	tile_addr |= offset_y;
	uint16_t pattern_d1 = ppu->memory[tile_addr];
	uint16_t pattern_d2 = ppu->memory[tile_addr + 8];
	pattern_d1 >>= 7 - x;
	pattern_d1 &= 1;
	pattern_d2 >>= 7 - x;
	pattern_d2 &= 1;
	return (pattern_d1 | pattern_d2 << 1);
}

void debug_tile(PPU *ppu,uint8_t offset_y,  uint8_t pix_index)
{
	uint16_t full_tile[16];
	uint16_t tile_addr = 0;
	for (int pattern_table = 0; pattern_table < 0xFF; pattern_table++)
	{
		tile_addr = ((*(ppu->PPUCTRL) & 0x10) >> 4) << 12;
		tile_addr |= pattern_table << 4;
		tile_addr += offset_y;
		for (int i = 0; i < 16; i++)
		{
			full_tile[i] = ppu->memory[tile_addr + i];
		}
		if (ppu->ppuFrames == 1) {
			printf("WHAT %x\n", ((tile_addr & 0xF00) >> 4) | ((tile_addr & 0xF0) >> 4));
			printf("well here I am.. %x %x %x %d %d %d\n", pix_index, (*(ppu->PPUCTRL) & 0x10) >> 4, tile_addr, ppu->regV & 0x0001F, (ppu->regV & 0x01e0) >> 5, offset_y);
			for (int i = 0; i < 8; i++)
			{
				for (int c = 0; c < 8; c++)
				{
					uint8_t a = full_tile[i];
					uint8_t b = full_tile[i + 8];
					a >>= (7 - c);
					a &= 1;
					b >>= (7 - c);
					b &= 1;
					uint8_t thing = a | b << 1;
					char s = '.';
					if (thing != 0)
					{
						s = thing + '0';
					}
					else
					{
						s = '.';
					}
					printf("%c", s);
				}
				printf("\n");
			}
		}
	}

}



/*
 Now this is what I call bad coding!
*/
colour_t getSpriteColour(PPU* ppu, int x, uint16_t tile_addr, uint8_t attr_tile)
{
	
	uint16_t pattern_d1 = ppu->memory[tile_addr];
	uint16_t pattern_d2 = ppu->memory[tile_addr + 8];
	if (attr_tile & 0x40)
	{
		pattern_d1 = flip_byte(pattern_d1);
		pattern_d2 = flip_byte(pattern_d2);
	}
		pattern_d1 >>= 7 - x;
		pattern_d1 &= 1;
		pattern_d2 >>= 7 - x;
		pattern_d2 &= 1;
	
	uint8_t pix_index = (pattern_d1 | (pattern_d2 << 1));

	if (pix_index != 0)
		pix_index |= ((attr_tile&0x3) << 2);
	return palette[ppu->memory[0x3F10 | pix_index]];
}

uint16_t getBase(PPU *ppu)
{
	uint16_t base = 0;
	switch (*(ppu->PPUCTRL) & 0x3)
	{
		case 0:
			base = 0x2000;
			break;
		case 1:
			base = 0x2400;
			break;
		case 2:
			base = 0x2800;
			break;
		case 3:
			base = 0x2C00;
			break;
	}
	return base;
}

colour_t getColour(PPU *ppu, int x, int scanline)
{
	uint8_t pattern_table = ppu->memory[getBase(ppu) | (ppu->regV & 0x0FFF)];
	uint8_t attribute = ppu->memory[0x23C0 | (ppu->regV & 0x0C00) | ((ppu->regV >> 4) & 0x38) | (ppu->regV >> 2) & 0x07];
	uint8_t offset_y =  (ppu->regV >> 12) & 0x7;
	uint16_t tile_addr = ((*(ppu->PPUCTRL)&0x10) >> 4) << 12;
	tile_addr |= (uint16_t)(pattern_table) << 4;
	tile_addr |= offset_y;
	uint16_t pattern_d1 = ppu->memory[tile_addr];
	uint16_t pattern_d2 = ppu->memory[tile_addr+8];
	pattern_d1 >>= 7-x;
	pattern_d1 &= 1;
	pattern_d2 >>= 7-x;
	pattern_d2 &= 1;
	uint8_t shift = (((ppu->regV) >> 4) & 0x4)| ((ppu->regV) & 0x2);
	attribute = (attribute >> shift) & 0x3;
	uint8_t pix_index = (pattern_d1 | (pattern_d2 << 1));
	if(pix_index!=0)
		pix_index |= (attribute << 2);
	return palette[ppu->memory[0x3F00 | pix_index]];
}

void write_vram_addr(PPU* ppu,uint8_t addr)
{
	if (ppu->write_flip)
	{
		ppu->regT |= addr;
		ppu->regV = ppu->regT;
	}
	else
	{
		ppu->regT = (addr & 0x3F) << 8;
	}
	ppu->write_flip = ~ppu->write_flip;
}

void write_controll_reg(PPU* ppu, uint8_t data)
{
	ppu->nmiOutput = (*(ppu->PPUCTRL) & 0x80) >> 7;
	ppu->regT = (ppu->regT & ~(0xC00)) | ((data &0x3)<< 10);
}

void write_scroll_reg(PPU* ppu, uint16_t data)
{
	if (ppu->write_flip)
	{
		ppu->regT = (ppu->regT & ~(0x7000)) | ((data & 0x03) << 12);
		ppu->regT = (ppu->regT & ~(0x00E0)) |  ((data & 0x38) << 2);
		ppu->regT = (ppu->regT & ~(0x0300)) | ((data & 0xC0) << 2);
	}
	else
	{
		ppu->regT = (ppu->regT & ~(0x1F)) | ((data & 0xF8) >> 3);
		ppu->regFX = (data & 0x3);
	}
	ppu->write_flip = ~ppu->write_flip;
}

void status_read(PPU* ppu)
{
	*(ppu->PPUSTATUS) = (*(ppu->PPUSTATUS ) & 0x7F) | (ppu->nmiOcc << 7);
	ppu->nmiOcc = 0;
	ppu->write_flip = 0x00;
}

void sprite_data(PPU* ppu)
{
	if(!((ppu->scanline >= 240) && (ppu->scanline <= 260)))
		*(ppu->OAMADDR)++;
}

void mirroring(PPU* ppu, uint8_t data, uint16_t addr)
{
	if (ppu->horizontal_mirroring)
	{
		if ((addr >= 0x2000) && (addr <= 0x23FF))
		{
			ppu->memory[addr + 0x0400] = data;
		}
		else if ((addr >= 0x2400) && (addr <= 0x27FF))
		{
			ppu->memory[addr - 0x0400] = data;
		}
		else if ((addr >= 0x2800) && (addr <= 0x2BFF))
		{
			ppu->memory[addr + 0x0400] = data;
		}
		else if ((addr >= 0x2C00) && (addr <= 0x2FFF))
		{
			ppu->memory[addr - 0x0400] = data;
		}
	}
	else if (ppu->vertical_mirroring)
	{
		if ((addr >= 0x2000) && (addr <= 0x23FF))
		{
			ppu->memory[addr + 0x0800] = data;
		}
		else if ((addr >= 0x2400) && (addr <= 0x27FF))
		{
			ppu->memory[addr + 0x0800] = data;
		}
		// Not sure if should be added
		//else if ((addr >= 0x2800) && (addr <= 0x2BFF))
		//{
		//	ppu->memory[addr - 0x0800] = data;
		//}
		//else if ((addr >= 0x2C00) && (addr <= 0x2FFF))
		//{
		//	ppu->memory[addr - 0x0800] = data;
		//}
	}

}

void write_vram_data(PPU* ppu, uint8_t data)
{
	ppu->memory[ppu->regV] = data;
	
	if ((ppu->regV == 0x3F10) || (ppu->regV == 0x3F14) || (ppu->regV == 0x3F18) || (ppu->regV == 0x3F1C))
	{
		ppu->memory[ppu->regV - 0x10] = data;
	}

	else if ((ppu->regV == 0x3F00) || (ppu->regV == 0x3F04) || (ppu->regV == 0x3F08) || (ppu->regV == 0x3F0C))
	{
		ppu->memory[ppu->regV + 0x10] = data;
	}

	else if (ppu->regV >= 0x3F20)
		ppu->memory[ppu->regV - 0x20] = data;
	else if ((ppu->regV < 0x3F00) && (ppu->regV >= 0x3000))
	{
		ppu->memory[ppu->regV - 0x1000] = data;
		mirroring(ppu, data, ppu->regV - 0x1000);
	}
	else if ((ppu->regV <= 0x2FFF) && (ppu->regV >= 0x2000)) {
		ppu->memory[ppu->regV + 0x1000] = data;
		mirroring(ppu, data, ppu->regV);
	}
	if ((*(ppu->PPUMASK) & 0x18) && (ppu->scanline >= -1 && ppu->scanline <= 239))
	{
		update_coarse_x(ppu);
		update_fine_y(ppu);
	}
	//not sure if it's an else if or just an if
	else if (*(ppu->PPUCTRL) & 0x4) {
		ppu->regV += 32;
	}
	else {
		ppu->regV += 1;
	}
}

void read_controller(PPU *ppu)
{
	printf("read controller...?\n");
	
}

void read_vram_data(PPU *ppu)
{
	if ((*(ppu->PPUMASK) & 0x18) && (ppu->scanline >= -1 && ppu->scanline <= 239))
	{
		printf("LOL");
		update_coarse_x(ppu);
		update_fine_y(ppu);
	}
	if (*(ppu->PPUCTRL) & 0x4) {
		printf("Uhhh");
		ppu->regV += 32;
	}
	else {
		printf("ok.");
		ppu->regV += 1;
	}
}

uint16_t get_vram_addr(PPU* ppu)
{
	return ppu->vram_addr;
}


SDL_Renderer* make_screen(PPU* ppu)
{
	//The window we'll be rendering to
	SDL_Window* window = NULL;

	//The surface contained by the window
	SDL_Surface* screenSurface = NULL;
	SDL_Renderer *renderer = NULL;
	SDL_Event event;

	//Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
	}
	else
	{
		//Create window
		SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, 0, &window, &renderer);
		if (window == NULL)
		{
			printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
		}
		else
		{
			//Get window surface
			screenSurface = SDL_GetWindowSurface(window);
			ppu->texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STATIC, SCREEN_WIDTH, SCREEN_HEIGHT);
		}  
	}
	ppu->screen = screenSurface;
	ppu->window = window;
	return renderer;
}
void closeWindow(PPU *ppu)
{
	SDL_DestroyWindow(ppu->window);
	SDL_Quit();
}