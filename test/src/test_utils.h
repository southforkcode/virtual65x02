#ifndef __TEST_UTILS_H
#define __TEST_UTILS_H

#include <stdint.h>
#include <vector>

#include "memory.h"

enum REG8 : uint8_t {
    D0 = 0, D1, D2, D3, D4, D5, D6, D7
};

enum SEGREG : uint8_t {
    PS = 0, DS, SS
};

void init_segment_with_program(Memory& ram, std::vector<uint8_t> segments, uint8_t seg, uint16_t addr, const std::vector<uint8_t>& bytes) {    
    for(auto iseg : segments) {
        ram.segments[iseg].init();
    }
    ram.program(0x00, 0xfffa, {0x00, 0x03, 0x00, 0x03, 0x00, 0x03});
    ram.program(seg, addr, bytes);
}

uint8_t xtop1(uint8_t ff, uint8_t ddd, uint8_t sss) {
    assert (ff < 4);
    assert (ddd < 8);
    assert (sss < 8);
    return (ff << 6) | (ddd << 3) | sss;
}

// 0xd2 cf ddd sss
uint8_t xtop1_math(uint8_t c, uint8_t f, uint8_t rd, uint8_t rs) {
    return (c << 7) | (f << 6) | (rd << 3) | rs;
}

// 0xf7 0 D ddd sss
uint8_t xtop1_trx(uint8_t D, uint8_t ddd, uint8_t sss) {
    assert (D == 0 || D == 1);
    assert (ddd < 8);
    assert (sss < 8);
    return (D << 6) | (ddd << 3) | sss;
}

uint8_t xtop1_stor(uint8_t ff, uint8_t iii, uint8_t rrr) {
    assert (ff < 4);
    assert (iii < 8);
    assert (rrr < 8);
    return (ff << 6) | (iii << 3) | rrr;
}

#endif
