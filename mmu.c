#include "mmu.h"

u8 mmu_rb(i8080* m, u16 addr) {
    // reads a byte from memory
    if (addr > 0xFFFF) {
        printf("error: accessing outside memory");
        return 0;
    }
    return m->memory[addr];
}

u16 mmu_rw(i8080* m, u16 addr) {
    // reads a word from memory
    if (addr > 0xFFFF) {
        printf("error: accessing outside memory");
        return 0;
    }
    return (m->memory[addr + 1] << 8) | m->memory[addr];
}

u8 mmu_next_byte(i8080* m) {
    // returns next byte in memory at pc, then increments pc
    u8 val = mmu_rb(m, m->pc);
    m->pc = (m->pc + 1) & 0xFFFF;
    return val;
}

u16 mmu_next_word(i8080* m) {
    // returns next byte in memory at pc, then increments pc
    u16 val = mmu_rw(m, m->pc);
    m->pc = (m->pc + 2) & 0xFFFF;
    return val;
}

void mmu_wb(i8080* m, u16 addr, const u8 val) {
    // writes a byte to memory
    if (addr > 0xFFFF) {
        printf("error: writing outside memory");
        return;
    }
    m->memory[addr] = val;
}

void mmu_ww(i8080* m, u16 addr, const u16 val) {
    // writes a word to memory
    if (addr > 0xFFFF) {
        printf("error: writing outside memory");
        return;
    }
    m->memory[addr] = val & 0xFF;
    m->memory[addr + 1] = val >> 8;
}

void mmu_push_stack(i8080* m, const u16 val) {
    // pushes a value into the stack and updates the stack pointer accordingly
    m->sp = (m->sp - 2) & 0xFFFF;
    mmu_ww(m, m->sp, val);
}

u16 mmu_pop_stack(i8080* m) {
    // pops a value from the stack and updates the stack pointer accordingly
    u16 val = mmu_rw(m, m->sp);
    m->sp = (m->sp + 2) & 0xFFFF;
    return val;
}

u16 mmu_get_bc(i8080* m) {
    return (m->reg.B << 8) | m->reg.C;
}

u16 mmu_get_de(i8080* m) {
    return (m->reg.D << 8) | m->reg.E;
}

u16 mmu_get_hl(i8080* m) {
    return (m->reg.H << 8) | m->reg.L;
}

void mmu_set_bc(i8080* m, const u16 val) {
    m->reg.B = val >> 8;
    m->reg.C = val & 0xFF;
}

void mmu_set_de(i8080* m, const u16 val) {
    m->reg.D = val >> 8;
    m->reg.E = val & 0xFF;
}

void mmu_set_hl(i8080* m, const u16 val) {
    m->reg.H = val >> 8;
    m->reg.L = val & 0xFF;
}
