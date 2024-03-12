#include <cstdint>
#include <catch2/catch_test_macros.hpp>
#include "memory.h"
#include "cpu65x.h"

void test_value_8(CPU& cpu, uint8_t v, int zf, int nf) {
    cpu.set_flags(NF_Mask | ZF_Mask, 8, v, 0);
    REQUIRE( ((cpu.P.ZF == zf) && (cpu.P.NF == nf)) == true );
}
void test_value_16(CPU& cpu, uint16_t v, int zf, int nf) {
    cpu.set_flags(NF_Mask | ZF_Mask, 16, v, 0);
    REQUIRE( ((cpu.P.ZF == zf) && (cpu.P.NF == nf)) == true );
}
void test_value_32(CPU& cpu, uint32_t v, int zf, int nf) {
    cpu.set_flags(NF_Mask | ZF_Mask, 32, v, 0);
    REQUIRE( ((cpu.P.ZF == zf) && (cpu.P.NF == nf)) == true );
}

TEST_CASE( "set N and Z flags", "[flags]" ) {
    CPU cpu;
    cpu.init();
    // 8-bit tests
    test_value_8(cpu, 0x00, 1, 0);
    test_value_8(cpu, 0xf0, 0, 1);
    test_value_8(cpu, 0x7f, 0, 0);
    // 16-bit tests
    test_value_16(cpu, 0x0000, 1, 0);
    test_value_16(cpu, 0xf000, 0, 1);
    test_value_16(cpu, 0x7fff, 0, 0);
    // 32-bit tests
    test_value_32(cpu, 0x00000000, 1, 0);
    test_value_32(cpu, 0xf0000000, 0, 1);
    test_value_32(cpu, 0x7fffffff, 0, 0);
}

