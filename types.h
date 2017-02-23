#pragma once

#include <stdint.h>
#include <stdbool.h>

typedef uint8_t u8;
typedef int8_t i8;
typedef uint16_t u16;
typedef int16_t i16;
typedef uint32_t u32;
typedef int32_t i32;
typedef uint64_t u64;
typedef int64_t i64;

enum flag_masks {
    S = 0b10000000, // sign
    Z = 0b01000000, // zero
    AC= 0b00010000, // auxilary carry (half carry)
    P = 0b00000100, // parity
    CY= 0b00000001, // carry
};

struct i8080_flags {
    bool S, Z, AC, P, CY;
};

struct i8080_registers {
    u8 A, B, C, D, E, H, L;
};

typedef struct i8080 i8080;
struct i8080 {
    u8 memory[0x10000];

    struct i8080_registers reg;
    struct i8080_flags flag;
    u16 pc;
    u16 sp;
    u8 port1, port2;

    bool iff; // INTE flip-flop
    u8 shift0, shift1, shift_offset;

    u32 cycles_count;
    u8 next_interrupt;
};
