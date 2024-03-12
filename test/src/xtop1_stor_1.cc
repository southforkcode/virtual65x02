#include <cstdint>
#include <catch2/catch_test_macros.hpp>

#include "memory.h"
#include "cpu65x.h"
#include "cpu65xops.h"

#include "xtop1imp.h"

#include "test_utils.h"

TEST_CASE("xtop1_stor", "[xtop1_stor]") {
    Memory ram;
    CPU cpu;

    ram.init();

    cpu.tracing = true;
    cpu.allow65c02 = false;
    cpu.allow65x02 = true;

    init_segment_with_program(ram, {0}, 0, 0x300, {
        LDA_Immediate, 0x40,
        XTOP1, xtop1(0, D7, D1), // %d7 <- %d1
        XTOP1_STOR, xtop1_stor(0, D7, D7), 0x01
    });
    cpu.reset(ram);
    cpu.execute_next_instruction(ram);
    cpu.execute_next_instruction(ram);
    cpu.execute_next_instruction(ram);
    REQUIRE(ram.read(0, 0x01) == 0x40);

    // LDA #$EA
    // LDX #$02
    // TRB %D6, %D1
    // STB [$04,%D3],%D6
    // $00:0006 = $EA
    init_segment_with_program(ram, {0}, 0, 0x300, {
        LDA_Immediate, 0xea,
        LDX_Immediate, 0x02,
        XTOP1, xtop1(0, D6, D1), // trb %d6, %d1 (d1->d6)
        XTOP1_STOR, xtop1_stor(0, D3, D6), 0x04, // stb %d6,$04,%d3
    });
    cpu.reset(ram);
    cpu.execute_next_instruction(ram);
    cpu.execute_next_instruction(ram);
    cpu.execute_next_instruction(ram);
    cpu.execute_next_instruction(ram);
    REQUIRE( ram.read(0,0x06) == 0xea);

    // LDA #$08
    // TRB %D5, %A
    // STB [$1000],%D5
    init_segment_with_program(ram, {0}, 0, 0x300, {
        LDA_Immediate, 0x08,
        XTOP1, xtop1(0, D5, D1),
        XTOP1_STOR, xtop1_stor(1, D5, D5), 0x00, 0x10
    });
    cpu.reset(ram);
    cpu.execute_next_instruction(ram);
    cpu.execute_next_instruction(ram);
    cpu.execute_next_instruction(ram);
    REQUIRE( ram.read(0,0x1000) == 0x08);

    // LDX #$dd
    // LDY #$04
    // STB [$2000,%Y], %X
    // $00:2004=$dd
    init_segment_with_program(ram, {0}, 0, 0x300, {
        LDX_Immediate, 0xdd,
        LDY_Immediate, 0x04,
        XTOP1_STOR, xtop1_stor(1, D5, D3), 0x00, 0x20
    });
    cpu.reset(ram);
    cpu.execute_next_instruction(ram);
    cpu.execute_next_instruction(ram);
    cpu.execute_next_instruction(ram);
    REQUIRE( ram.read(0,0x2004) == 0xdd);

    // LDA #$30
    // STB [$105000], %A
    // 0x105000 = $30
    init_segment_with_program(ram, {0}, 0, 0x300, {
        LDA_Immediate, 0x30,
        XTOP1_STOR, xtop1_stor(2, D1, D1), 0x00, 0x50, 0x10
    });
    cpu.reset(ram);
    cpu.execute_next_instruction(ram);
    cpu.execute_next_instruction(ram);
    REQUIRE( ram.read(0x10,0x5000) == 0x30);
    
    // LDA #$55
    // LDX #$FF ; -1
    // STB [$f04001,%X], %A
    init_segment_with_program(ram, {0}, 0, 0x300, {
        LDA_Immediate, 0x55,
        LDX_Immediate, 0xff,
        XTOP1_STOR, xtop1_stor(2, D3, D1), 0x01, 0x40, 0xf0
    });
    cpu.reset(ram);
    cpu.execute_next_instruction(ram);
    cpu.execute_next_instruction(ram);
    cpu.execute_next_instruction(ram);
    REQUIRE( ram.read(0xf0,0x4000) == 0x55);

    // LDA #$a0
    // STB [SS:SP-4], %A
    // SS:SP-4 = $a0
    init_segment_with_program(ram, {0}, 0, 0x300, {
        LDA_Immediate, 0xa0,
        XTOP1_STOR, xtop1_stor(3, D1, D1), 0xfc
    });
    cpu.reset(ram);
    cpu.execute_next_instruction(ram);
    cpu.execute_next_instruction(ram);
    REQUIRE( ram.read(cpu.SS,cpu.SP-4) == 0xa0);

}
