#include <cstdint>
#include <catch2/catch_test_macros.hpp>

#include "memory.h"
#include "cpu65x.h"
#include "cpu65xops.h"

#include "test_utils.h"

TEST_CASE( "ADC", "[6502]" ) {
    Memory ram;
    ram.init();

    CPU cpu;
    cpu.tracing = false;
    cpu.allow65c02 = false;
    cpu.allow65x02 = false;

    init_segment_with_program(ram, {0}, 0, 0x300, {
        LDA_Immediate, 0x40,
        ADC_Immediate, 0x20
    });

    cpu.reset(ram);
    cpu.execute_next_instruction(ram);
    cpu.execute_next_instruction(ram);
    
    REQUIRE( cpu.A() == 0x60 );
    REQUIRE( cpu.P.CF == 0);
    REQUIRE( cpu.P.ZF == 0);
    REQUIRE( cpu.P.NF == 0);
    REQUIRE( cpu.opCC == 2);

    init_segment_with_program(ram, {0}, 0, 0x300, {
        LDA_Immediate, 0x40,
        ADC_ZeroPage, 0x01
    });
    ram.write(0, 1, 0x60); // zp:01=60

    cpu.reset(ram);
    cpu.execute_next_instruction(ram);
    cpu.execute_next_instruction(ram);

    REQUIRE( cpu.A() == 0xa0 );
    REQUIRE( cpu.P.NF == 1);
    REQUIRE( cpu.opCC == 3);

    init_segment_with_program(ram, {0}, 0, 0x300, {
        LDA_Immediate, 0x40,
        LDX_Immediate, 0x01,
        ADC_ZeroPageX, 0x00
    });
    ram.write(0, 1, 0x60); // zp:01=60

    cpu.reset(ram);
    cpu.execute_next_instruction(ram);
    cpu.execute_next_instruction(ram);
    cpu.execute_next_instruction(ram);

    REQUIRE( cpu.A() == 0xa0 );
    REQUIRE( cpu.P.NF == 1);
    REQUIRE( cpu.opCC == 4 );

    init_segment_with_program(ram, {0}, 0, 0x300, {
        LDA_Immediate, 0x40,
        ADC_Absolute, 0x00, 0x20
    });
    ram.write(0, 0x2000, 0x60); // $2000=60

    cpu.reset(ram);
    cpu.execute_next_instruction(ram);
    cpu.execute_next_instruction(ram);

    REQUIRE( cpu.A() == 0xa0 );
    REQUIRE( cpu.P.NF == 1);
    REQUIRE( cpu.opCC == 4);

    init_segment_with_program(ram, {0}, 0, 0x300, {
        LDA_Immediate, 0x40,
        LDX_Immediate, 0x01,
        ADC_AbsoluteX, 0x00, 0x20
    });
    ram.write(0, 0x2001, 0x60); // $2000=60

    cpu.reset(ram);
    cpu.execute_next_instruction(ram);
    cpu.execute_next_instruction(ram);
    cpu.execute_next_instruction(ram);

    REQUIRE( cpu.A() == 0xa0 );
    REQUIRE( cpu.P.NF == 1);
    REQUIRE( cpu.opCC == 4 );

    init_segment_with_program(ram, {0}, 0, 0x300, {
        LDA_Immediate, 0x40,
        LDY_Immediate, 0x01,
        ADC_AbsoluteY, 0x00, 0x20
    });
    ram.write(0, 0x2001, 0x60); // $2000=60

    cpu.reset(ram);
    cpu.execute_next_instruction(ram);
    cpu.execute_next_instruction(ram);
    cpu.execute_next_instruction(ram);

    REQUIRE( cpu.A() == 0xa0 );
    REQUIRE( cpu.P.NF == 1);
    REQUIRE( cpu.opCC == 4 );

    init_segment_with_program(ram, {0}, 0, 0x300, {
        LDA_Immediate, 0x40,
        LDX_Immediate, 0x01,
        ADC_IndirectX, 0x3F
    });
    ram.write(0, 0x40, 0x00);
    ram.write(0, 0x41, 0x20);
    ram.write(0, 0x2000, 0x60); // $2000=60

    cpu.reset(ram);
    cpu.execute_next_instruction(ram);
    cpu.execute_next_instruction(ram);
    cpu.execute_next_instruction(ram);
    
    REQUIRE( cpu.A() == 0xa0 );
    REQUIRE( cpu.P.NF == 1);
    REQUIRE( cpu.opCC == 6 );

    init_segment_with_program(ram, {0}, 0, 0x300, {
        LDA_Immediate, 0x40,
        LDY_Immediate, 0x02,
        ADC_IndirectY, 0x40
    });
    ram.write(0, 0x40, 0x00);
    ram.write(0, 0x41, 0x20);
    ram.write(0, 0x2002, 0x60); // $2000=60

    cpu.reset(ram);
    cpu.execute_next_instruction(ram);
    cpu.execute_next_instruction(ram);
    cpu.execute_next_instruction(ram);
    
    REQUIRE( cpu.A() == 0xa0 );
    REQUIRE( cpu.P.NF == 1 );
    REQUIRE( cpu.opCC == 5 );
}
