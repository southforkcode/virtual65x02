#include <cstdint>
#include <catch2/catch_test_macros.hpp>
#include "memory.h"

TEST_CASE( "read/write memory", "[memory]" ) {
    Memory ram;
    ram.init();
    ram.write(10, 1000, 0xea);
    REQUIRE( ram.read(10, 1000) == 0xea );
}    

