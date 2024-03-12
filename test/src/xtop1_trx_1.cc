#include <catch2/catch_test_macros.hpp>

#include "memory.h"
#include "cpu65x.h"
#include "cpu65xops.h"

#include "xtop1imp.h"

#include "test_utils.h"

TEST_CASE("xtop1_trx", "[xtop1_trx]") {
    Memory ram;
    CPU cpu;

    ram.init();

    cpu.tracing = false;
    cpu.allow65c02 = false;
    cpu.allow65x02 = true;

    init_segment_with_program(ram, {0}, 0, 0x300, {
        LDA_Immediate, 0x40,
        XTOP1_TRX, xtop1_trx(0, D1, PS), // %ps <- %d1
    });

    cpu.reset(ram);
    cpu.execute_next_instruction(ram);
    cpu.execute_next_instruction(ram);
    
    REQUIRE( cpu.registerPS() == 0x40 );

    init_segment_with_program(ram, {0}, 0, 0x300, {
        LDA_Immediate, 0x40,
        XTOP1, xtop1(0, D2, D1),
        XTOP1_TRX, xtop1_trx(0, D2, DS), // %ds <- %d2
        XTOP1_TRX, xtop1_trx(1, D3, DS), // %d3 <- %ds
    });

    cpu.reset(ram);
    cpu.execute_next_instruction(ram);
    cpu.execute_next_instruction(ram);
    REQUIRE( cpu.register8(D2) == 0x40);
    cpu.execute_next_instruction(ram);
    REQUIRE( cpu.registerDS() == 0x40 );
    cpu.execute_next_instruction(ram);
    REQUIRE( cpu.register8(D3) == 0x040);
}