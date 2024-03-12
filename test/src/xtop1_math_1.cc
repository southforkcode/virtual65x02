#include <cstdint>
#include <catch2/catch_test_macros.hpp>

#include "memory.h"
#include "cpu65x.h"
#include "cpu65xops.h"

#include "xtop1imp.h"

#include "test_utils.h"

TEST_CASE("xtop1_math", "[xtop1_math]") {
    Memory ram;
    CPU cpu;

    ram.init();

    cpu.tracing = false;
    cpu.allow65c02 = false;
    cpu.allow65x02 = true;

    init_segment_with_program(ram, {0}, 0, 0x300, {
        LDA_Immediate, 0x40,
        LDX_Immediate, 0x60,
        XTOP1_MATH, xtop1_math(0, 0, D1, D3) // %d1 <- %d1 + %d3
    });

    cpu.reset(ram);
    cpu.execute_next_instruction(ram);
    cpu.execute_next_instruction(ram);
    cpu.execute_next_instruction(ram);
    
    REQUIRE( cpu.register8(D1) == 0xa0 );
    REQUIRE( cpu.P.NF == 1 );

    init_segment_with_program(ram, {0}, 0, 0x300, {
        LDA_Immediate, 0x60,
        LDX_Immediate, 0x40,
        XTOP1_MATH, xtop1_math(0, 1, D1, D3) // %d1 <- %d1 + %d3
    });

    cpu.reset(ram);
    cpu.execute_next_instruction(ram);
    cpu.execute_next_instruction(ram);
    cpu.execute_next_instruction(ram);
    
    REQUIRE( cpu.register8(D1) == 0x20 );
    REQUIRE( cpu.P.NF == 0 );

    init_segment_with_program(ram, {0}, 0, 0x300, {
        LDA_Immediate, 0x60,
        LDX_Immediate, 0x40,
        
        XTOP1_MATH, xtop1_math(1, 0, D2, D2), 0xa1 // %d2 <- $a1
    });

    cpu.reset(ram);
    cpu.execute_next_instruction(ram);
    cpu.execute_next_instruction(ram);
    cpu.execute_next_instruction(ram);
    
    REQUIRE( cpu.register8(D2) == 0xa1 );

    init_segment_with_program(ram, {0}, 0, 0x300, {
        LDX_Immediate, 0x40,
        XTOP1_MATH, xtop1_math(1, 1, D3, D3), 0x01 // %d3 <- %d3 - 1
    });

    cpu.reset(ram);
    cpu.execute_next_instruction(ram);
    cpu.execute_next_instruction(ram);
    
    REQUIRE( cpu.register8(D3) == 0x3f );
    REQUIRE( cpu.P.NF == 0 );

    init_segment_with_program(ram, {0}, 0, 0x300, {
        LDY_Immediate, 0x7f,
        XTOP1_MATH, xtop1_math(1, 0, D5, D5), 0x01 // %d5 = %d5 + 1
    });

    cpu.reset(ram);
    cpu.execute_next_instruction(ram);
    cpu.execute_next_instruction(ram);
    
    REQUIRE( cpu.register8(D5) == 0x80 );
    REQUIRE( cpu.P.NF == 1 );
}
