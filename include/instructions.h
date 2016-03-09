#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

void jmp_abs(CPU* cpu,unsigned char* buffer);
void loadx_imm(CPU* cpu, unsigned char* buffer);
void storex_zp(CPU* cpu, unsigned char* buffer);
void jsr(CPU* cpu, unsigned char* buffer);
void nop(CPU* cpu, unsigned char* buffer);
void sec(CPU* cpu, unsigned char* buffer);
void bcs(CPU* cpu, unsigned char* buffer);
void clc(CPU* cpu, unsigned char* buffer);
void bcc(CPU* cpu, unsigned char* buffer);
void lda_imm(CPU* cpu, unsigned char* buffer);
void beq(CPU* cpu, unsigned char* buffer);
void bne(CPU* cpu, unsigned char* buffer);
void sta_zp(CPU* cpu, unsigned char* buffer);
void bit_zp(CPU* cpu, unsigned char* buffer);
void bvs(CPU* cpu, unsigned char* buffer);
void bvc(CPU* cpu, unsigned char* buffer);
void bpl(CPU* cpu, unsigned char* buffer);
void rts(CPU* cpu, unsigned char* buffer);
void sei(CPU* cpu, unsigned char* buffer);
void sed(CPU* cpu, unsigned char* buffer);
void php(CPU* cpu, unsigned char* buffer);
void pla(CPU* cpu, unsigned char* buffer);
void and_imm(CPU* cpu, unsigned char* buffer);
void cmp_imm(CPU* cpu, unsigned char* buffer);
void cld(CPU* cpu, unsigned char* buffer);
void pha(CPU* cpu, unsigned char* buffer);
void plp(CPU* cpu, unsigned char* buffer);
void bmi(CPU* cpu, unsigned char* buffer);
void ora_imm(CPU* cpu, unsigned char* buffer);
void clv(CPU* cpu, unsigned char* buffer);
void eor_imm(CPU* cpu, unsigned char* buffer);
void adc_imm(CPU* cpu, unsigned char* buffer);

uint8_t address_bytes[256] = {0, 1, 0, 0, 1, 1, 1, 0, 0, 1, 0, 0, 2, 2, 2, 0, 1, 1, 0, 0, 1, 1, 1, 0, 0, 2, 0, 0, 2, 2, 2, 0, 2, 1, 0, 0, 1, 1, 1, 0, 0, 1, 0, 0, 2, 2, 2, 0, 1, 1, 0, 0, 1, 1, 1, 0, 0, 2, 0, 0, 2, 2, 2, 0, 0, 1, 0, 0, 1, 1, 1, 0, 0, 1, 0, 0, 2, 2, 2, 0, 1, 1, 0, 0, 1, 1, 1, 0, 0, 2, 0, 0, 2, 2, 2, 0, 0, 1, 0, 0, 1, 1, 1, 0, 0, 1, 0, 0, 2, 2, 2, 0, 1, 1, 0, 0, 1, 1, 1, 0, 0, 2, 0, 0, 2, 2, 2, 0, 1, 1, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 2, 2, 2, 2, 1, 1, 0, 0, 1, 1, 1, 1, 0, 2, 0, 0, 0, 2, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 0, 2, 2, 2, 2, 1, 1, 0, 1, 1, 1, 1, 1, 0, 2, 0, 0, 2, 2, 2, 2, 1, 1, 0, 0, 1, 1, 1, 0, 0, 1, 0, 0, 2, 2, 2, 0, 1, 1, 0, 0, 1, 1, 1, 0, 0, 2, 0, 0, 2, 2, 2, 0, 1, 1, 0, 0, 1, 1, 1, 0, 0, 1, 0, 0, 2, 2, 2, 0, 1, 1, 0, 0, 1, 1, 1, 0, 0, 2, 0, 0, 2, 2, 2, 0};

// 0 implied
// 1 immediate
// 2 zero page
// 3 zero page, X
// 4 zero page, Y
// 5 indirect index, X
// 6 indirect index, Y
// 7 absolute
// 8 absolute, X
// 9 absolute, Y
// 10 indirect
// 11 relative
// 2 zp-addr
// 7 abs-addr
// 8 absx-addr
// 9 absy-addr
// 3 zpx-addr
// 4 zpy-addr 
// 6 indirect index, X address
uint8_t addressing_mode[256] = {0, 5, 0, 5, 2, 2, 2, 2, 0, 1, 0, 1, 7, 7, 7, 7, 
				11, 6, 0, 6, 3, 3, 3, 3, 0, 9, 0, 9, 8, 8, 8, 8, 
				7, 5, 0, 5, 2, 2, 2, 2, 0, 1, 0, 1, 7, 7, 7, 7, 
				11, 6, 0, 6, 3, 3, 3, 3, 0, 9, 0, 9, 8, 8, 8, 8, 
				0, 5, 0, 5, 2, 2, 2, 2, 0, 1, 0, 1, 7, 7, 7, 7, 
				11, 6, 0, 6, 3, 3, 3, 3, 0, 9, 0, 9, 8, 8, 8, 8, 
				0, 5, 0, 5, 2, 2, 2, 2, 0, 1, 0, 1, 10, 7, 7, 7, 
				11, 6, 0, 6, 3, 3, 3, 3, 0, 9, 0, 9, 8, 8, 8, 8, 
				1, 5, 1, 6, 2, 2, 2, 2, 0, 1, 0, 1, 7, 7, 7, 7, 
				11, 6, 0, 6, 3, 3, 4, 3, 0, 9, 0, 9, 8, 8, 8, 8, 
				1, 5, 1, 5, 2, 2, 2, 2, 0, 1, 0, 1, 7, 7, 7, 7, 
				11, 6, 0, 6, 3, 3, 4, 4, 0, 9, 0, 9, 8, 8, 9, 9, 
				1, 5, 0, 5, 2, 2, 2, 2, 0, 1, 0, 1, 7, 7, 7, 7, 
				11, 6, 0, 6, 3, 3, 3, 3, 0, 9, 0, 9, 8, 8, 8, 8, 
				1, 6, 1, 5, 2, 2, 2, 2, 0, 1, 0, 1, 7, 7, 7, 7, 
				11, 6, 0, 6, 3, 3, 3, 3, 0, 9, 0, 9, 8, 8, 8, 8 };

char* inst_name[256] =  { "brk", "ora", "kil", "slo", "nop", "ora", "asl", "slo", "php", "ora", "asl", "anc", "nop", "ora", "asl", "slo",
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
			  "beq", "sbc", "kil", "isc", "nop", "sbc", "inc", "isc", "sed", "sbc", "nop", "isc", "nop", "sbc", "inc", "isc"};


void (*instruction[])(CPU*,unsigned char*) = {jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,php,ora_imm,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,
											  bpl,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,clc,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,
											  jsr,jmp_abs,jmp_abs,jmp_abs,bit_zp,jmp_abs,jmp_abs,jmp_abs,plp,and_imm,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,
											  bmi,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,sec,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,
											  jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,pha,eor_imm,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,
											  bvc,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,
											  rts,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,pla,adc_imm,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,
											  bvs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,sei,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,
											  jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,sta_zp,storex_zp,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,
											  bcc,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,
											  jmp_abs,jmp_abs,loadx_imm,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,lda_imm,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,
											  bcs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,clv,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,
											  jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,cmp_imm,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,
											  bne,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,cld,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,
											  jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,nop,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,
											  beq,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,sed,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs};


