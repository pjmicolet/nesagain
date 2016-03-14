#include <stdint.h>

#ifndef CPU_H
#define CPU_H

typedef struct CPU
{
  uint8_t A;
  uint8_t X,Y;
  uint16_t PC;
  uint8_t S;
  uint8_t P[8];
  uint8_t memory[0x10000];
  // We dont need to keep accumulating really.
  uint64_t cycles;
} CPU;


uint8_t get_status_flag(uint8_t pflag[]);
// Define addressing type of each instructio
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
// 12 zp - addr
// 13 abs - addr
// 14 absx - addr
// 15 absy - addr
// 16 zpx - addr
// 17 zpy - addr
// 18 indirect index, X address
// 19 indirect index, Y address
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


void jmp(CPU* cpu, unsigned char* buffer);
void ldx(CPU* cpu, unsigned char* buffer);
void stx(CPU* cpu, unsigned char* buffer);
void jsr(CPU* cpu, unsigned char* buffer);
void nop(CPU* cpu, unsigned char* buffer);
void sec(CPU* cpu, unsigned char* buffer);
void bcs(CPU* cpu, unsigned char* buffer);
void clc(CPU* cpu, unsigned char* buffer);
void bcc(CPU* cpu, unsigned char* buffer);
void lda(CPU* cpu, unsigned char* buffer);
void beq(CPU* cpu, unsigned char* buffer);
void bne(CPU* cpu, unsigned char* buffer);
void sta(CPU* cpu, unsigned char* buffer);
void sty(CPU* cpu, unsigned char* buffer);
void bit_zp(CPU* cpu, unsigned char* buffer);
void bvs(CPU* cpu, unsigned char* buffer);
void bvc(CPU* cpu, unsigned char* buffer);
void bpl(CPU* cpu, unsigned char* buffer);
void rts(CPU* cpu, unsigned char* buffer);
void sei(CPU* cpu, unsigned char* buffer);
void sed(CPU* cpu, unsigned char* buffer);
void php(CPU* cpu, unsigned char* buffer);
void pla(CPU* cpu, unsigned char* buffer);
void and(CPU* cpu, unsigned char* buffer);
void cmp(CPU* cpu, unsigned char* buffer);
void cld(CPU* cpu, unsigned char* buffer);
void pha(CPU* cpu, unsigned char* buffer);
void plp(CPU* cpu, unsigned char* buffer);
void bmi(CPU* cpu, unsigned char* buffer);
void ora(CPU* cpu, unsigned char* buffer);
void clv(CPU* cpu, unsigned char* buffer);
void eor(CPU* cpu, unsigned char* buffer);
void adc(CPU* cpu, unsigned char* buffer);
void ldy(CPU* cpu, unsigned char* buffer);
void cpy(CPU* cpu, unsigned char* buffer);
void cpx(CPU* cpu, unsigned char* buffer);
void sbc(CPU* cpu, unsigned char* buffer);
void iny(CPU* cpu, unsigned char* buffer);
void inx(CPU* cpu, unsigned char* buffer);
void dex(CPU* cpu, unsigned char* buffer);
void dey(CPU* cpu, unsigned char* buffer);
void tay(CPU* cpu, unsigned char* buffer);
void tax(CPU* cpu, unsigned char* buffer);
void txa(CPU* cpu, unsigned char* buffer);
void tya(CPU* cpu, unsigned char* buffer);
void tsx(CPU* cpu, unsigned char* buffer);
void txs(CPU* cpu, unsigned char* buffer);
void rti(CPU* cpu, unsigned char* buffer);
void lsr(CPU* cpu, unsigned char* buffer);
void asl(CPU* cpu, unsigned char* buffer);
void ror(CPU* cpu, unsigned char* buffer);
void rol(CPU* cpu, unsigned char* buffer);
void inc(CPU* cpu, unsigned char* buffer);
void dec(CPU* cpu, unsigned char* buffer);


uint8_t address_bytes[256] = { 0, 1, 0, 0, 1, 1, 1, 0, 0, 1, 0, 0, 2, 2, 2, 0, 1, 1, 0, 0, 1, 1, 1, 0, 0, 2, 0, 0, 2, 2, 2, 0, 2, 1, 0, 0, 1, 1, 1, 0, 0, 1, 0, 0, 2, 2, 2, 0, 1, 1, 0, 0, 1, 1, 1, 0, 0, 2, 0, 0, 2, 2, 2, 0, 0, 1, 0, 0, 1, 1, 1, 0, 0, 1, 0, 0, 2, 2, 2, 0, 1, 1, 0, 0, 1, 1, 1, 0, 0, 2, 0, 0, 2, 2, 2, 0, 0, 1, 0, 0, 1, 1, 1, 0, 0, 1, 0, 0, 2, 2, 2, 0, 1, 1, 0, 0, 1, 1, 1, 0, 0, 2, 0, 0, 2, 2, 2, 0, 1, 1, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 2, 2, 2, 2, 1, 1, 0, 0, 1, 1, 1, 1, 0, 2, 0, 0, 0, 2, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 0, 2, 2, 2, 2, 1, 1, 0, 1, 1, 1, 1, 1, 0, 2, 0, 0, 2, 2, 2, 2, 1, 1, 0, 0, 1, 1, 1, 0, 0, 1, 0, 0, 2, 2, 2, 0, 1, 1, 0, 0, 1, 1, 1, 0, 0, 2, 0, 0, 2, 2, 2, 0, 1, 1, 0, 0, 1, 1, 1, 0, 0, 1, 0, 0, 2, 2, 2, 0, 1, 1, 0, 0, 1, 1, 1, 0, 0, 2, 0, 0, 2, 2, 2, 0 };


void(*instruction[256])(CPU*, unsigned char*) = { jmp,ora,jmp,jmp,jmp,ora,asl,jmp,php,ora,asl,jmp,jmp,jmp,jmp,jmp,
bpl,jmp,jmp,jmp,jmp,jmp,jmp,jmp,clc,jmp,jmp,jmp,jmp,jmp,jmp,jmp,
jsr,and,jmp,jmp,bit_zp,and,rol,jmp,plp,and,rol,jmp,jmp,jmp,jmp,jmp,
bmi,jmp,jmp,jmp,jmp,jmp,jmp,jmp,sec,jmp,jmp,jmp,jmp,jmp,jmp,jmp,
rti,eor,jmp,jmp,jmp,eor,lsr,jmp,pha,eor,lsr,jmp,jmp,jmp,jmp,jmp,
bvc,jmp,jmp,jmp,jmp,jmp,jmp,jmp,jmp,jmp,jmp,jmp,jmp,jmp,jmp,jmp,
rts,adc,jmp,jmp,jmp,adc,ror,jmp,pla,adc,ror,jmp,jmp,adc,jmp,jmp,
bvs,adc,jmp,jmp,jmp,jmp,jmp,jmp,sei,adc,jmp,jmp,jmp,adc,jmp,jmp,
jmp,sta,jmp,jmp,sty,sta,stx,jmp,dey,jmp,txa,jmp,jmp,sta,stx,jmp,
bcc,jmp,jmp,jmp,jmp,jmp,jmp,jmp,tya,jmp,txs,jmp,jmp,jmp,jmp,jmp,
ldy,lda,ldx,jmp,ldy,lda,ldx,jmp,tay,lda,tax,jmp,ldy,lda,ldx,jmp,
bcs,jmp,jmp,jmp,jmp,jmp,jmp,jmp,clv,jmp,tsx,jmp,jmp,jmp,jmp,jmp,
cpy,cmp,jmp,jmp,cpy,cmp,dec,jmp,iny,cmp,dex,jmp,jmp,jmp,jmp,jmp,
bne,jmp,jmp,jmp,jmp,jmp,jmp,jmp,cld,jmp,jmp,jmp,jmp,jmp,jmp,jmp,
cpx,sbc,jmp,jmp,cpx,sbc,inc,jmp,inx,sbc,nop,jmp,jmp,jmp,jmp,jmp,
beq,jmp,jmp,jmp,jmp,jmp,jmp,jmp,sed,jmp,jmp,jmp,jmp,jmp,jmp,jmp };


#endif