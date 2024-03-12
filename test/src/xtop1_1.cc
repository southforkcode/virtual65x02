#include <cstdint>
#include <catch2/catch_test_macros.hpp>

#include "memory.h"
#include "cpu65x.h"
#include "cpu65xops.h"

#include "xtop1imp.h"

#include "test_utils.h"

TEST_CASE("xtop1 d8 r/r trx", "[xtop1_0]") {
    Memory ram;
    CPU cpu;

    ram.init();

    cpu.tracing = false;
    cpu.allow65c02 = false;
    cpu.allow65x02 = true;

    init_segment_with_program(ram, {0}, 0, 0x300, {
        LDA_Immediate, 0x40,
        XTOP1, 0x39 // %d1 <- ~%d1
    });

    cpu.reset(ram);
    cpu.execute_next_instruction(ram);
    cpu.execute_next_instruction(ram);

    REQUIRE( cpu.register8(1) == 0x40 );
    REQUIRE( cpu.register8(7) == 0x40 );

    init_segment_with_program(ram, {0}, 0, 0x300, {
        LDA_Immediate, 0x40,
        XTOP1, 0x09
    });
    
    cpu.reset(ram);
    cpu.execute_next_instruction(ram);
    cpu.execute_next_instruction(ram);

    REQUIRE( cpu.register8(1) == 0xbf );
}