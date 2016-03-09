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

uint8_t address_bytes[256] = {0, 1, 0, 0, 1, 1, 1, 0, 0, 1, 0, 0, 2, 2, 2, 0, 1, 1, 0, 0, 1, 1, 1, 0, 0, 2, 0, 0, 2, 2, 2, 0, 2, 1, 0, 0, 1, 1, 1, 0, 0, 1, 0, 0, 2, 2, 2, 0, 1, 1, 0, 0, 1, 1, 1, 0, 0, 2, 0, 0, 2, 2, 2, 0, 0, 1, 0, 0, 1, 1, 1, 0, 0, 1, 0, 0, 2, 2, 2, 0, 1, 1, 0, 0, 1, 1, 1, 0, 0, 2, 0, 0, 2, 2, 2, 0, 0, 1, 0, 0, 1, 1, 1, 0, 0, 1, 0, 0, 2, 2, 2, 0, 1, 1, 0, 0, 1, 1, 1, 0, 0, 2, 0, 0, 2, 2, 2, 0, 1, 1, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 2, 2, 2, 2, 1, 1, 0, 0, 1, 1, 1, 1, 0, 2, 0, 0, 0, 2, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 0, 2, 2, 2, 2, 1, 1, 0, 1, 1, 1, 1, 1, 0, 2, 0, 0, 2, 2, 2, 2, 1, 1, 0, 0, 1, 1, 1, 0, 0, 1, 0, 0, 2, 2, 2, 0, 1, 1, 0, 0, 1, 1, 1, 0, 0, 2, 0, 0, 2, 2, 2, 0, 1, 1, 0, 0, 1, 1, 1, 0, 0, 1, 0, 0, 2, 2, 2, 0, 1, 1, 0, 0, 1, 1, 1, 0, 0, 2, 0, 0, 2, 2, 2, 0};

//Handler jump_table[256] = {brk, ora, kil, slo, nop, ora, asl, slo, php, ora, asl, anc, nop, ora, asl, slo, bpl, ora, kil, slo, nop, ora, asl, slo, clc, ora, nop, slo, nop, ora, asl, slo, jsr, and, kil, rla, bit, and, rol, rla, plp, and, rol, anc, bit, and, rol, rla, bmi, and, kil, rla, nop, and, rol, rla, sec, and, nop, rla, nop, and, rol, rla, rti, eor, kil, sre, nop, eor, lsr, sre, pha, eor, lsr, alr, jmp, eor, lsr, sre, bvc, eor, kil, sre, nop, eor, lsr, sre, cli, eor, nop, sre, nop, eor, lsr, sre, rts, adc, kil, rra, nop, adc, ror, rra, pla, adc, ror, arr, jmp, adc, ror, rra, bvs, adc, kil, rra, nop, adc, ror, rra, sei, adc, nop, rra, nop, adc, ror, rra, nop, sta, nop, sax, sty, sta, stx, sax, dey, nop, txa, xaa, sty, sta, stx, sax, bcc, sta, kil, ahx, sty, sta, stx, sax, tya, sta, txs, tas, shy, sta, shx, ahx, ldy, lda, ldx, lax, ldy, lda, ldx, lax, tay, lda, tax, lax, ldy, lda, ldx, lax, bcs, lda, kil, lax, ldy, lda, ldx, lax, clv, lda, tsx, las, ldy, lda, ldx, lax, cpy, cmp, nop, dcp, cpy, cmp, dec, dcp, iny, cmp, dex, axs, cpy, cmp, dec, dcp, bne, cmp, kil, dcp, nop, cmp, dec, dcp, cld, cmp, nop, dcp, nop, cmp, dec, dcp, cpx, sbc, nop, isc, cpx, sbc, inc, isc, inx, sbc, nop, sbc, cpx, sbc, inc, isc, beq, sbc, kil, isc, nop, sbc, inc, isc, sed, sbc, nop, isc, nop, sbc, inc, isc};

void (*instruction[])(CPU*,unsigned char*) = {jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,
											  jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,
											  jsr,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,
											  jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,sec,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,
											  jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,
											  jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,
											  jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,
											  jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,
											  jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,storex_zp,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,
											  jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,
											  jmp_abs,jmp_abs,loadx_imm,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,
											  bcs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,
											  jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,
											  jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,
											  jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,nop,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,
											  jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs,jmp_abs};


