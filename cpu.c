#include "cpu.h"

void cpu_init(i8080* m) {
    memset(m->memory, 0, sizeof m->memory);
    m->reg.A = 0;
    m->reg.B = 0;
    m->reg.C = 0;
    m->reg.D = 0;
    m->reg.E = 0;
    m->reg.H = 0;
    m->reg.L = 0;

    m->flag.S = 0;
    m->flag.Z = 0;
    m->flag.AC = 0;
    m->flag.P = 0;
    m->flag.CY = 0;

    m->pc = 0;
    m->sp = 0;
    m->iff = false;

    m->port1 = 0;
    m->port2 = 0;
    m->cycles_count = 0;
    m->next_interrupt = 0x08;

    m->shift0 = 0;
    m->shift1 = 0;
    m->shift_offset = 0;
}

int cpu_load_file(i8080* m, const char* filename, const u16 start_addr) {
    FILE *f;
    long file_size = 0;

    f = fopen (filename, "rb");
    if (f == NULL) {
        printf("error: can't open ROM file '%s'.", filename);
        return 1;
    }

    // obtain file size
    fseek(f, 0, SEEK_END);
    file_size = ftell(f);
    rewind(f);

    if (file_size > 0xFFFF) {
        printf("error: rom file '%s' too big to fit in memory.", filename);
        return 1;
    }

    // copy file data to buffer
    u8 buffer[file_size];
    size_t result = fread(buffer, 1, file_size, f);
    if (result != file_size) {
        printf("error: while reading ROM file '%s'", filename);
        return 1;
    }

    // copy buffer to memory
    for (int i = 0; i < file_size; i++) {
        m->memory[start_addr + i] = buffer[i];
    }

    return 0;
}

void cpu_set_flags(i8080* m, const u8 flags_mask, const i32 value) {
    if (flags_mask & S) {
        m->flag.S = (value & 0x80) == 0x80;
    }
    if (flags_mask & Z) {
        m->flag.Z = (value & 0xFF) == 0;
    }
    if (flags_mask & AC) {
        m->flag.AC = (m->reg.A & 0xF) > (value & 0xF);
    }
    if (flags_mask & P) {
        m->flag.P = parity(value);
    }
    if (flags_mask & CY) {
        m->flag.CY = value > 0xFF || value < 0x00;
    }
}

u8 cpu_step(i8080* m) {
    u32 start_cycle_count = m->cycles_count; // to count the number of cycles
                                             // used for this opcode
                                             // (some cycle durations
                                             // are handled in opcodes)

    u8 opcode = mmu_next_byte(m);

    switch (opcode) {
        // 8 bit transfer instructions
        case 0x7F: m->reg.A = m->reg.A; break; // MOV A,A
        case 0x78: m->reg.A = m->reg.B; break; // MOV A,B
        case 0x79: m->reg.A = m->reg.C; break; // MOV A,C
        case 0x7A: m->reg.A = m->reg.D; break; // MOV A,D
        case 0x7B: m->reg.A = m->reg.E; break; // MOV A,E
        case 0x7C: m->reg.A = m->reg.H; break; // MOV A,H
        case 0x7D: m->reg.A = m->reg.L; break; // MOV A,L
        case 0x7E: m->reg.A = mmu_rb(m, mmu_get_hl(m)); break; // MOV A,M

        case 0x0A: m->reg.A = mmu_rb(m, mmu_get_bc(m)); break; // LDAX B
        case 0x1A: m->reg.A = mmu_rb(m, mmu_get_de(m)); break; // LDAX D
        case 0x3A: m->reg.A = mmu_rb(m, mmu_next_word(m)); break; // LDA word

        case 0x47: m->reg.B = m->reg.A; break; // MOV B,A
        case 0x40: m->reg.B = m->reg.B; break; // MOV B,B
        case 0x41: m->reg.B = m->reg.C; break; // MOV B,C
        case 0x42: m->reg.B = m->reg.D; break; // MOV B,D
        case 0x43: m->reg.B = m->reg.E; break; // MOV B,E
        case 0x44: m->reg.B = m->reg.H; break; // MOV B,H
        case 0x45: m->reg.B = m->reg.L; break; // MOV B,L
        case 0x46: m->reg.B = mmu_rb(m, mmu_get_hl(m)); break; // MOV B,M

        case 0x4F: m->reg.C = m->reg.A; break; // MOV C,A
        case 0x48: m->reg.C = m->reg.B; break; // MOV C,B
        case 0x49: m->reg.C = m->reg.C; break; // MOV C,C
        case 0x4A: m->reg.C = m->reg.D; break; // MOV C,D
        case 0x4B: m->reg.C = m->reg.E; break; // MOV C,E
        case 0x4C: m->reg.C = m->reg.H; break; // MOV C,H
        case 0x4D: m->reg.C = m->reg.L; break; // MOV C,L
        case 0x4E: m->reg.C = mmu_rb(m, mmu_get_hl(m)); break; // MOV C,M

        case 0x57: m->reg.D = m->reg.A; break; // MOV D,A
        case 0x50: m->reg.D = m->reg.B; break; // MOV D,B
        case 0x51: m->reg.D = m->reg.C; break; // MOV D,C
        case 0x52: m->reg.D = m->reg.D; break; // MOV D,D
        case 0x53: m->reg.D = m->reg.E; break; // MOV D,E
        case 0x54: m->reg.D = m->reg.H; break; // MOV D,H
        case 0x55: m->reg.D = m->reg.L; break; // MOV D,L
        case 0x56: m->reg.D = mmu_rb(m, mmu_get_hl(m)); break; // MOV D,M

        case 0x5F: m->reg.E = m->reg.A; break; // MOV E,A
        case 0x58: m->reg.E = m->reg.B; break; // MOV E,B
        case 0x59: m->reg.E = m->reg.C; break; // MOV E,C
        case 0x5A: m->reg.E = m->reg.D; break; // MOV E,D
        case 0x5B: m->reg.E = m->reg.E; break; // MOV E,E
        case 0x5C: m->reg.E = m->reg.H; break; // MOV E,H
        case 0x5D: m->reg.E = m->reg.L; break; // MOV E,L
        case 0x5E: m->reg.E = mmu_rb(m, mmu_get_hl(m)); break; // MOV E,M

        case 0x67: m->reg.H = m->reg.A; break; // MOV H,A
        case 0x60: m->reg.H = m->reg.B; break; // MOV H,B
        case 0x61: m->reg.H = m->reg.C; break; // MOV H,C
        case 0x62: m->reg.H = m->reg.D; break; // MOV H,D
        case 0x63: m->reg.H = m->reg.E; break; // MOV H,E
        case 0x64: m->reg.H = m->reg.H; break; // MOV H,H
        case 0x65: m->reg.H = m->reg.L; break; // MOV H,L
        case 0x66: m->reg.H = mmu_rb(m, mmu_get_hl(m)); break; // MOV H,M

        case 0x6F: m->reg.L = m->reg.A; break; // MOV L,A
        case 0x68: m->reg.L = m->reg.B; break; // MOV L,B
        case 0x69: m->reg.L = m->reg.C; break; // MOV L,C
        case 0x6A: m->reg.L = m->reg.D; break; // MOV L,D
        case 0x6B: m->reg.L = m->reg.E; break; // MOV L,E
        case 0x6C: m->reg.L = m->reg.H; break; // MOV L,H
        case 0x6D: m->reg.L = m->reg.L; break; // MOV L,L
        case 0x6E: m->reg.L = mmu_rb(m, mmu_get_hl(m)); break; // MOV L,M

        case 0x77: mmu_wb(m, mmu_get_hl(m), m->reg.A); break; // MOV M,A
        case 0x70: mmu_wb(m, mmu_get_hl(m), m->reg.B); break; // MOV M,B
        case 0x71: mmu_wb(m, mmu_get_hl(m), m->reg.C); break; // MOV M,C
        case 0x72: mmu_wb(m, mmu_get_hl(m), m->reg.D); break; // MOV M,D
        case 0x73: mmu_wb(m, mmu_get_hl(m), m->reg.E); break; // MOV M,E
        case 0x74: mmu_wb(m, mmu_get_hl(m), m->reg.H); break; // MOV M,H
        case 0x75: mmu_wb(m, mmu_get_hl(m), m->reg.L); break; // MOV M,L

        case 0x3E: m->reg.A = mmu_next_byte(m); break; // MVI A,byte
        case 0x06: m->reg.B = mmu_next_byte(m); break; // MVI B,byte
        case 0x0E: m->reg.C = mmu_next_byte(m); break; // MVI C,byte
        case 0x16: m->reg.D = mmu_next_byte(m); break; // MVI D,byte
        case 0x1E: m->reg.E = mmu_next_byte(m); break; // MVI E,byte
        case 0x26: m->reg.H = mmu_next_byte(m); break; // MVI H,byte
        case 0x2E: m->reg.L = mmu_next_byte(m); break; // MVI L,byte
        case 0x36: mmu_wb(m, mmu_get_hl(m), mmu_next_byte(m)); break; // MVI M,byte

        case 0x02: mmu_wb(m, mmu_get_bc(m), m->reg.A); break; // STAX B
        case 0x12: mmu_wb(m, mmu_get_de(m), m->reg.A); break; // STAX D
        case 0x32: mmu_wb(m, mmu_next_word(m), m->reg.A); break; // STA word

        // 16 bit transfer instructions
        case 0x01: mmu_set_bc(m, mmu_next_word(m)); break; // LXI B,word
        case 0x11: mmu_set_de(m, mmu_next_word(m)); break; // LXI D,word
        case 0x21: mmu_set_hl(m, mmu_next_word(m)); break; // LXI H,word
        case 0x31: m->sp = mmu_next_word(m); break; // LXI SP,word
        case 0x2A: mmu_set_hl(m, mmu_rw(m, mmu_next_word(m))); break; // LHLD
        case 0x22: mmu_ww(m, mmu_next_word(m), mmu_get_hl(m)); break; // SHLD
        case 0xF9: m->sp = mmu_get_hl(m); break; // SPHL

        // register exchange instructions
        case 0xEB: { // XCHG
            u16 de = mmu_get_de(m);
            mmu_set_de(m, mmu_get_hl(m));
            mmu_set_hl(m, de);
        } break;
        case 0xE3: { // XTHL
            u16 value = mmu_rw(m, m->sp);
            mmu_ww(m, m->sp, mmu_get_hl(m));
            mmu_set_hl(m, value);
        } break;

        // add byte instructions
        case 0x87: cpu_add(m, &m->reg.A, m->reg.A, 0); break; // ADD A
        case 0x80: cpu_add(m, &m->reg.A, m->reg.B, 0); break; // ADD B
        case 0x81: cpu_add(m, &m->reg.A, m->reg.C, 0); break; // ADD C
        case 0x82: cpu_add(m, &m->reg.A, m->reg.D, 0); break; // ADD D
        case 0x83: cpu_add(m, &m->reg.A, m->reg.E, 0); break; // ADD E
        case 0x84: cpu_add(m, &m->reg.A, m->reg.H, 0); break; // ADD H
        case 0x85: cpu_add(m, &m->reg.A, m->reg.L, 0); break; // ADD L
        case 0x86: cpu_add(m, &m->reg.A, mmu_rb(m, mmu_get_hl(m)), 0); break; // ADD M
        case 0xC6: cpu_add(m, &m->reg.A, mmu_next_byte(m), 0); break; // ADI byte

        // add byte with carry-in instructions
        case 0x8F: cpu_add(m, &m->reg.A, m->reg.A, m->flag.CY); break; // ADC A
        case 0x88: cpu_add(m, &m->reg.A, m->reg.B, m->flag.CY); break; // ADC B
        case 0x89: cpu_add(m, &m->reg.A, m->reg.C, m->flag.CY); break; // ADC C
        case 0x8A: cpu_add(m, &m->reg.A, m->reg.D, m->flag.CY); break; // ADC D
        case 0x8B: cpu_add(m, &m->reg.A, m->reg.E, m->flag.CY); break; // ADC E
        case 0x8C: cpu_add(m, &m->reg.A, m->reg.H, m->flag.CY); break; // ADC H
        case 0x8D: cpu_add(m, &m->reg.A, m->reg.L, m->flag.CY); break; // ADC L
        case 0x8E: cpu_add(m, &m->reg.A, mmu_rb(m, mmu_get_hl(m)), m->flag.CY); break; // ADC M
        case 0xCE: cpu_add(m, &m->reg.A, mmu_next_byte(m), m->flag.CY); break; // ACI byte

        // substract byte instructions
        case 0x97: cpu_sub(m, &m->reg.A, m->reg.A, 0); break; // SUB A
        case 0x90: cpu_sub(m, &m->reg.A, m->reg.B, 0); break; // SUB B
        case 0x91: cpu_sub(m, &m->reg.A, m->reg.C, 0); break; // SUB C
        case 0x92: cpu_sub(m, &m->reg.A, m->reg.D, 0); break; // SUB D
        case 0x93: cpu_sub(m, &m->reg.A, m->reg.E, 0); break; // SUB E
        case 0x94: cpu_sub(m, &m->reg.A, m->reg.H, 0); break; // SUB H
        case 0x95: cpu_sub(m, &m->reg.A, m->reg.L, 0); break; // SUB L
        case 0x96: cpu_sub(m, &m->reg.A, mmu_rb(m, mmu_get_hl(m)), 0); break; // SUB M
        case 0xD6: cpu_sub(m, &m->reg.A, mmu_next_byte(m), 0); break; // SUI byte

        // substract byte with borrow-in instructions
        case 0x9F: cpu_sub(m, &m->reg.A, m->reg.A, m->flag.CY); break; // SBB A
        case 0x98: cpu_sub(m, &m->reg.A, m->reg.B, m->flag.CY); break; // SBB B
        case 0x99: cpu_sub(m, &m->reg.A, m->reg.C, m->flag.CY); break; // SBB C
        case 0x9A: cpu_sub(m, &m->reg.A, m->reg.D, m->flag.CY); break; // SBB D
        case 0x9B: cpu_sub(m, &m->reg.A, m->reg.E, m->flag.CY); break; // SBB E
        case 0x9C: cpu_sub(m, &m->reg.A, m->reg.H, m->flag.CY); break; // SBB H
        case 0x9D: cpu_sub(m, &m->reg.A, m->reg.L, m->flag.CY); break; // SBB L
        case 0x9E: cpu_sub(m, &m->reg.A, mmu_rb(m, mmu_get_hl(m)), m->flag.CY); break; // SBB M
        case 0xDE: cpu_sub(m, &m->reg.A, mmu_next_byte(m), m->flag.CY); break; // SBI byte

        // double byte add instructions
        case 0x09: cpu_dad(m, mmu_get_bc(m)); break; // DAD B
        case 0x19: cpu_dad(m, mmu_get_de(m)); break; // DAD D
        case 0x29: cpu_dad(m, mmu_get_hl(m)); break; // DAD H
        case 0x39: cpu_dad(m, m->sp); break; // DAD SP

        // control instructions
        case 0xF3: m->iff = 0; break; // DI
        case 0xFB: m->iff = 1; break; // EI
        case 0x00: break; // NOP
        case 0x76: m->pc = (m->pc - 1) & 0xFFFF; break; // HLT

        // increment byte instructions
        case 0x3C: cpu_inr(m, &m->reg.A); break; // INR A
        case 0x04: cpu_inr(m, &m->reg.B); break; // INR B
        case 0x0C: cpu_inr(m, &m->reg.C); break; // INR C
        case 0x14: cpu_inr(m, &m->reg.D); break; // INR D
        case 0x1C: cpu_inr(m, &m->reg.E); break; // INR E
        case 0x24: cpu_inr(m, &m->reg.H); break; // INR H
        case 0x2C: cpu_inr(m, &m->reg.L); break; // INR L
        case 0x34: { // INR M
            u8 value = (mmu_rb(m, mmu_get_hl(m)) + 1) & 0xFF;
            mmu_wb(m, mmu_get_hl(m), value);
            cpu_set_flags(m, S|Z|AC|P, value);
        } break;

        // decrement byte instructions
        case 0x3D: cpu_dcr(m, &m->reg.A); break; // DCR A
        case 0x05: cpu_dcr(m, &m->reg.B); break; // DCR B
        case 0x0D: cpu_dcr(m, &m->reg.C); break; // DCR C
        case 0x15: cpu_dcr(m, &m->reg.D); break; // DCR D
        case 0x1D: cpu_dcr(m, &m->reg.E); break; // DCR E
        case 0x25: cpu_dcr(m, &m->reg.H); break; // DCR H
        case 0x2D: cpu_dcr(m, &m->reg.L); break; // DCR L
        case 0x35: { // DCR M
            u8 value = (mmu_rb(m, mmu_get_hl(m)) - 1) & 0xFF;
            mmu_wb(m, mmu_get_hl(m), value);
            cpu_set_flags(m, S|Z|AC|P, value);
        } break;

        // increment register pair instructions
        case 0x03: mmu_set_bc(m, (mmu_get_bc(m) + 1) & 0xFFFF); break; // INX B
        case 0x13: mmu_set_de(m, (mmu_get_de(m) + 1) & 0xFFFF); break; // INX D
        case 0x23: mmu_set_hl(m, (mmu_get_hl(m) + 1) & 0xFFFF); break; // INX H
        case 0x33: m->sp = (m->sp + 1) & 0xFFFF; break; // INX SP

        // decrement register pair instructions
        case 0x0B: mmu_set_bc(m, (mmu_get_bc(m) - 1) & 0xFFFF); break; // DCX B
        case 0x1B: mmu_set_de(m, (mmu_get_de(m) - 1) & 0xFFFF); break; // DCX D
        case 0x2B: mmu_set_hl(m, (mmu_get_hl(m) - 1) & 0xFFFF); break; // DCX H
        case 0x3B: m->sp = (m->sp - 1) & 0xFFFF; break; // DCX SP

        // special accumulator and flag instructions
        case 0x27: { // DAA
            u8 lsb = m->reg.A & 0x0F;
            if (lsb > 9 || m->flag.AC) {
                m->flag.AC = (lsb + 6) > 0xF;
                m->reg.A = (m->reg.A + 6) & 0xFF;
            }

            u8 msb = m->reg.A >> 4;
            if (msb > 9 || m->flag.CY) {
                m->flag.CY = (msb + 6) > 0xF ? 1 : m->flag.CY;
                m->reg.A = (m->reg.A + 0b01100000) & 0xFF;
            }

            cpu_set_flags(m, Z|S|P, m->reg.A);
        } break;
        case 0x2F: m->reg.A = ~m->reg.A; break; // CMA
        case 0x37: m->flag.CY = 1; break; // STC
        case 0x3F: m->flag.CY = !m->flag.CY; break; // CMC

        // rotate instructions
        case 0x07: { // RLC (rotate left)
            u8 a = m->reg.A;
            m->flag.CY = a >> 7;
            m->reg.A = (a << 1) | m->flag.CY;
        } break;

        case 0x0F: { // RRC (rotate right)
            u8 a = m->reg.A;
            m->flag.CY = a & 1;
            m->reg.A = (m->flag.CY << 7) | (a >> 1);
        } break;

        case 0x17: { // RAL
            u8 a = m->reg.A;
            m->reg.A = (a << 1) | m->flag.CY;
            m->flag.CY = a >> 7;
        } break;

        case 0x1F: { // RAR
            u8 a = m->reg.A;
            m->reg.A = (m->flag.CY << 7) | (a >> 1);
            m->flag.CY = a & 1;
        } break;

        // logical byte instructions
        case 0xA7: cpu_ana(m, m->reg.A); break; // ANA A
        case 0xA0: cpu_ana(m, m->reg.B); break; // ANA B
        case 0xA1: cpu_ana(m, m->reg.C); break; // ANA C
        case 0xA2: cpu_ana(m, m->reg.D); break; // ANA D
        case 0xA3: cpu_ana(m, m->reg.E); break; // ANA E
        case 0xA4: cpu_ana(m, m->reg.H); break; // ANA H
        case 0xA5: cpu_ana(m, m->reg.L); break; // ANA L
        case 0xA6: cpu_ana(m, mmu_rb(m, mmu_get_hl(m))); break; // ANA M
        case 0xE6: cpu_ana(m, mmu_next_byte(m)); break; // ANI byte

        case 0xAF: cpu_xra(m, m->reg.A); break; // XRA A
        case 0xA8: cpu_xra(m, m->reg.B); break; // XRA B
        case 0xA9: cpu_xra(m, m->reg.C); break; // XRA C
        case 0xAA: cpu_xra(m, m->reg.D); break; // XRA D
        case 0xAB: cpu_xra(m, m->reg.E); break; // XRA E
        case 0xAC: cpu_xra(m, m->reg.H); break; // XRA H
        case 0xAD: cpu_xra(m, m->reg.L); break; // XRA L
        case 0xAE: cpu_xra(m, mmu_rb(m, mmu_get_hl(m))); break; // XRA M
        case 0xEE: { // XRI byte
            // according to docs, AC must not be affected by this opcode ?
            // bool ac = m->flag.AC;
            cpu_xra(m, mmu_next_byte(m));
            // m->flag.AC = ac;
        } break; // XRI byte

        case 0xB7: cpu_ora(m, m->reg.A); break; // ORA A
        case 0xB0: cpu_ora(m, m->reg.B); break; // ORA B
        case 0xB1: cpu_ora(m, m->reg.C); break; // ORA C
        case 0xB2: cpu_ora(m, m->reg.D); break; // ORA D
        case 0xB3: cpu_ora(m, m->reg.E); break; // ORA E
        case 0xB4: cpu_ora(m, m->reg.H); break; // ORA H
        case 0xB5: cpu_ora(m, m->reg.L); break; // ORA L
        case 0xB6: cpu_ora(m, mmu_rb(m, mmu_get_hl(m))); break; // ORA M
        case 0xF6: cpu_ora(m, mmu_next_byte(m)); break; // ORI byte

        case 0xBF: cpu_cmp(m, m->reg.A); break; // CMP A
        case 0xB8: cpu_cmp(m, m->reg.B); break; // CMP B
        case 0xB9: cpu_cmp(m, m->reg.C); break; // CMP C
        case 0xBA: cpu_cmp(m, m->reg.D); break; // CMP D
        case 0xBB: cpu_cmp(m, m->reg.E); break; // CMP E
        case 0xBC: cpu_cmp(m, m->reg.H); break; // CMP H
        case 0xBD: cpu_cmp(m, m->reg.L); break; // CMP L
        case 0xBE: cpu_cmp(m, mmu_rb(m, mmu_get_hl(m))); break; // CMP M
        case 0xFE: cpu_cmp(m, mmu_next_byte(m)); break; // CPI byte

        // branch control/program counter load instructions
        case 0xC3: cpu_jump(m, mmu_next_word(m)); break; // JMP
        case 0xC2: cpu_cond_jump(m, m->flag.Z == 0); break; // JNZ
        case 0xCA: cpu_cond_jump(m, m->flag.Z == 1); break; // JZ
        case 0xD2: cpu_cond_jump(m, m->flag.CY == 0); break; // JNC
        case 0xDA: cpu_cond_jump(m, m->flag.CY == 1); break; // JC
        case 0xE2: cpu_cond_jump(m, m->flag.P == 0); break; // JPO
        case 0xEA: cpu_cond_jump(m, m->flag.P == 1); break; // JPE
        case 0xF2: cpu_cond_jump(m, m->flag.S == 0); break; // JP
        case 0xFA: cpu_cond_jump(m, m->flag.S == 1); break; // JM

        case 0xE9: m->pc = mmu_get_hl(m); break; // PCHL
        case 0xCD: cpu_call(m, mmu_next_word(m)); break; // CALL

        case 0xC4: cpu_cond_call(m, m->flag.Z == 0); break; // CNZ
        case 0xCC: cpu_cond_call(m, m->flag.Z == 1); break; // CZ
        case 0xD4: cpu_cond_call(m, m->flag.CY == 0); break; // CNC
        case 0xDC: cpu_cond_call(m, m->flag.CY == 1); break; // CC
        case 0xE4: cpu_cond_call(m, m->flag.P == 0); break; // CPO
        case 0xEC: cpu_cond_call(m, m->flag.P == 1); break; // CPE
        case 0xF4: cpu_cond_call(m, m->flag.S == 0); break; // CP
        case 0xFC: cpu_cond_call(m, m->flag.S == 1); break; // CM

        case 0xC9: cpu_ret(m); break; // RET
        case 0xC0: cpu_cond_ret(m, m->flag.Z == 0); break; // RNZ
        case 0xC8: cpu_cond_ret(m, m->flag.Z == 1); break; // RZ
        case 0xD0: cpu_cond_ret(m, m->flag.CY == 0); break; // RNC
        case 0xD8: cpu_cond_ret(m, m->flag.CY == 1); break; // RC
        case 0xE0: cpu_cond_ret(m, m->flag.P == 0); break; // RPO
        case 0xE8: cpu_cond_ret(m, m->flag.P == 1); break; // RPE
        case 0xF0: cpu_cond_ret(m, m->flag.S == 0); break; // RP
        case 0xF8: cpu_cond_ret(m, m->flag.S == 1); break; // RM

        case 0xC7: cpu_call(m, 0x0); break; // RST 0
        case 0xCF: cpu_call(m, 0x8); break; // RST 1
        case 0xD7: cpu_call(m, 0x10); break; // RST 2
        case 0xDF: cpu_call(m, 0x18); break; // RST 3
        case 0xE7: cpu_call(m, 0x20); break; // RST 4
        case 0xEF: cpu_call(m, 0x28); break; // RST 5
        case 0xF7: cpu_call(m, 0x30); break; // RST 6
        case 0xFF: cpu_call(m, 0x38); break; // RST 7

        // stack operation instructions
        case 0xC5: mmu_push_stack(m, mmu_get_bc(m)); break; // PUSH B
        case 0xD5: mmu_push_stack(m, mmu_get_de(m)); break; // PUSH D
        case 0xE5: mmu_push_stack(m, mmu_get_hl(m)); break; // PUSH H
        case 0xF5: { // PUSH PSW
            u8 psw = 0;
            if (m->flag.S) psw |= S;
            if (m->flag.Z) psw |= Z;
            if (m->flag.AC) psw |= AC;
            if (m->flag.P) psw |= P;
            if (m->flag.CY) psw |= CY;

            psw |= 0b00000010; // bit 1 is always 1.
            psw &= ~0b00001000; // bit 3 is always 0.
            psw &= ~0b00100000; // bit 5 is always 0.

            mmu_push_stack(m, (m->reg.A << 8 | psw));
        } break;
        case 0xC1: mmu_set_bc(m, mmu_pop_stack(m)); break; // POP B
        case 0xD1: mmu_set_de(m, mmu_pop_stack(m)); break; // POP D
        case 0xE1: mmu_set_hl(m, mmu_pop_stack(m)); break; // POP H
        case 0xF1: { // POP PSW
            u16 af = mmu_pop_stack(m);
            m->reg.A = af >> 8;
            u8 psw = af & 0xFF;

            m->flag.S = psw & S ? 1 : 0;
            m->flag.Z = psw & Z ? 1 : 0;
            m->flag.AC = psw & AC ? 1 : 0;
            m->flag.P = psw & P ? 1 : 0;
            m->flag.CY = psw & CY ? 1 : 0;
        } break;

        // input/output instructions
        case 0xDB: { // IN
            // space invaders specific implementation
            const u8 port = mmu_next_byte(m);
            u8 value = 0;

            if (port == 1) {
                value = m->port1;
            }
            else if (port == 2) {
                value = m->port2;
            }
            else if (port == 3) {
                u16 v = (m->shift1 << 8) | m->shift0;
                value = v >> (8 - m->shift_offset);
            }
            else {
                printf("error: unknown port %i", port);
            }
            m->reg.A = value;
        } break;

        case 0xD3: { // OUT
            // space invaders specific implementation
            u8 port = mmu_next_byte(m);
            u8 value = m->reg.A;

            if (port == 2) {
                m->shift_offset = value & 0x7;
            }
            else if (port == 3) {
                // play sound
            }
            else if (port == 4) {
                m->shift0 = m->shift1;
                m->shift1 = value;
            }
            else if (port == 5) {
                // play sound
            }
            else if (port == 6) {
                // unused port (debug port?)
            }
            else {
                printf("error: unknown port %i", port);
            }
        } break;

        // UNIMPLEMENTED OPCODES (NOP?)
        case 0x08: break;
        case 0x10: break;
        case 0x18: break;
        case 0x20: break;
        case 0x28: break;
        case 0x30: break;
        case 0x38: break;
        case 0xD9: cpu_ret(m); break; // undocumented RET

        default:
            printf("error: unknown opcode %i", opcode);
            break;
    }

    // cycle management
    m->cycles_count = m->cycles_count + OPCODES_CYCLES[opcode];
    const u8 cycles_in_opcode = m->cycles_count - start_cycle_count;

    // interrupts handling
    if (m->cycles_count >= HALF_CYCLES_PER_FRAME) {
        if (m->iff) {
            // generate interrupt
            m->iff = 0;
            cpu_call(m, m->next_interrupt);
        }
        m->cycles_count = m->cycles_count - HALF_CYCLES_PER_FRAME;
        m->next_interrupt = m->next_interrupt == 0x08 ? 0x10 : 0x08;
    }

    return cycles_in_opcode;
}

void cpu_update(i8080* m) {
    // emulates the correct number of cycles for one frame
    // function to execute every 1/60s
    u16 count = 0;
    while (count <= CYCLES_PER_FRAME) {
        count += cpu_step(m);
    }
}

void cpu_run_tests(i8080* m, const char* filename) {
    cpu_init(m);
    cpu_load_file(m, filename, 0x100);

    m->pc = 0x100;
    m->memory[5] = 0xC9; // inject RET at 0x0005 to handle "CALL 5"
                         // useful for "CPUTEST.ROM"

    printf("***********************\n");
    while (1) {
        u16 cur_pc = m->pc;

        if (m->memory[m->pc] == 0x76) { // RET
            printf("HLT at %04X\n", m->pc);
        }

        // writing debug info to console
        if (m->pc == 5) {
            if (m->reg.C == 9) {
                u16 i = mmu_get_de(m);
                do {
                    printf("%c", m->memory[i]);
                    i += 1;
                } while (m->memory[i] != 0x24); // == '$'
            }
            if (m->reg.C == 2) {
                printf("%c", m->reg.E);
            }
        }

        // uncomment following line to have a debug output of machine state
        // cpu_debug_output(m); sleep(1);
        cpu_step(m);

        if (m->pc == 0) {
            printf("\nJumped to 0x000 from 0x%04X\n\n", cur_pc);
            break;
        }
    }
}

void cpu_debug_output(i8080* m) {
    char flags[] = "......";

    if (m->flag.Z) flags[0] = 'z';
    if (m->flag.S) flags[1] = 's';
    if (m->flag.P) flags[2] = 'p';
    if (m->iff) flags[3] = 'i';
    if (m->flag.CY) flags[4] = 'c';
    if (m->flag.AC) flags[5] = 'a';

    // registers + flag states
    printf("af\tbc\tde\thl\tpc\tsp\tflags\tcycles\n");
    printf("%02X__\t%04X\t%04X\t%04X\t%04X\t%04X\t%s\t%i\n",
           m->reg.A, mmu_get_bc(m), mmu_get_de(m), mmu_get_hl(m), m->pc,
           m->sp, flags, m->cycles_count);

    // ouput current address in memory
    printf("%04X: ", m->pc);

    // output current opcode + next two
    printf("%02X %02X %02X", m->memory[m->pc], m->memory[m->pc + 1],
           m->memory[m->pc + 2]);

    // output disassembly of the current opcode
    printf(" - %s", DISASSEMBLE_TABLE[m->memory[m->pc]]);

    printf("\n================================");
    printf("==============================\n");
}

// helper functions
bool parity(const u8 value) {
    // returns: false if number of 1 bits in `value` is odd,
    //          true if number of 1 bits in `value` is even
    u8 nb_one_bits = 0;
    for (int i = 0; i < 8; i++) {
        nb_one_bits += ((value >> i) & 1);
    }

    return (nb_one_bits & 1) == 0;
}

void cpu_jump(i8080* m, const u16 addr) {
    m->pc = addr;
}

void cpu_call(i8080* m, const u16 addr) {
    mmu_push_stack(m, m->pc);
    cpu_jump(m, addr);
}

void cpu_ret(i8080* m) {
    m->pc = mmu_pop_stack(m);
}

void cpu_add(i8080* m, u8* const reg, const u8 value, const bool carry) {
    const u32 result = *reg + value + carry;
    cpu_set_flags(m, S|Z|AC|P|CY, result);
    *reg = result & 0xFF;
}

void cpu_sub(i8080* m, u8* const reg, const u8 value, const bool carry) {
    const i32 result = *reg - value - carry;
    cpu_set_flags(m, S|Z|AC|P|CY, result);
    *reg = result & 0xFF;
}

void cpu_inr(i8080* m, u8* const reg) {
    i16 value = (*reg + 1) & 0xFF;
    cpu_set_flags(m, S|Z|AC|P, value);
    *reg = value;
}

void cpu_dcr(i8080* m, u8* const reg) {
    i16 value = (*reg - 1) & 0xFF;
    cpu_set_flags(m, S|Z|AC|P, value);
    *reg = value;
}

void cpu_ana(i8080* m, const u8 value) {
    // executes a logic AND between reg A and a 8bit value, then stores the
    // result in reg A.
    u8 result = m->reg.A & value;
    cpu_set_flags(m, S|Z|P, result);
    m->flag.CY = 0;
    m->reg.A = result;
}

void cpu_xra(i8080* m, const u8 value) {
    // executes a logic XOR between reg A and a 8bit value, then stores the
    // result in reg A.
    u8 result = m->reg.A ^ value;
    cpu_set_flags(m, S|Z|P, result);
    m->flag.CY = 0;
    m->flag.AC = 0;
    m->reg.A = result;
}

void cpu_ora(i8080* m, const u8 value) {
    // executes a logic OR between reg A and a 8bit value, then stores the
    // result in reg A.
    u8 result = m->reg.A | value;
    cpu_set_flags(m, S|Z|P, result);
    m->flag.CY = 0;
    m->flag.AC = 0;
    m->reg.A = result;
}

void cpu_cmp(i8080* m, const u8 value) {
    // compares the reg A to another 8bit value.
    i16 result = m->reg.A - value;
    cpu_set_flags(m, S|Z|AC|P|CY, result);
}

void cpu_cond_jump(i8080* m, const bool condition) {
    // jumps to next word in memory if condition == true
    u16 addr = mmu_next_word(m);
    if (condition) {
        cpu_jump(m, addr);
        m->cycles_count += 15;
    }
    else {
        m->cycles_count += 10;
    }
}

void cpu_cond_call(i8080* m, const bool condition) {
    // calls to next word in memory if condition == true
    u16 addr = mmu_next_word(m);
    if (condition) {
        cpu_call(m, addr);
        m->cycles_count += 18;
    }
    else {
        m->cycles_count += 11;
    }
}

void cpu_cond_ret(i8080* m, const bool condition) {
    // return if condition == true
    if (condition) {
        cpu_ret(m);
        m->cycles_count += 11;
    }
    else {
        m->cycles_count += 5;
    }
}

void cpu_dad(i8080* m, const u16 value) {
    // adds 16bit integer to HL
    u32 result = mmu_get_hl(m) + value;
    mmu_set_hl(m, result & 0xFFFF);
    m->flag.CY = result > 0xFFFF;
}
