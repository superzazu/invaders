#pragma once

#include <stdio.h>

#include "types.h"

u8 mmu_rb(i8080* m, u16 addr);
u16 mmu_rw(i8080* m, u16 addr);
u8 mmu_next_byte(i8080* m);
u16 mmu_next_word(i8080* m);

void mmu_wb(i8080* m, u16 addr, const u8 val);
void mmu_ww(i8080* m, u16 addr, const u16 val);

void mmu_push_stack(i8080* m, const u16 val);
u16 mmu_pop_stack(i8080* m);

u16 mmu_get_bc(i8080* m);
u16 mmu_get_de(i8080* m);
u16 mmu_get_hl(i8080* m);
void mmu_set_bc(i8080* m, const u16 val);
void mmu_set_de(i8080* m, const u16 val);
void mmu_set_hl(i8080* m, const u16 val);
