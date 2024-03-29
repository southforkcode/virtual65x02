#include <cstdlib>

#include "cpu65x.h"
#include "xutils.h"

int main(int argc, const char** argv) {
    struct CPU cpu;
    struct Memory ram;

    ram.init();
    // set NMI=0x0300, RESET=0x0300, INT=0x0300
    ram.program(0x00, 0xfffa, {0x00, 0x03, 0x00, 0x03, 0x00, 0x03});
    //ram.program(0x00, 0x0300, {0xa9, 0x01, 0xa2, 0x02, 0xa0, 0x03});
    //ram.program(0x00, 0x0300, {0xa9, 0x80, 0x85, 0x21, 0xa9, 0x85, 0xa2, 0x01, 0x79, 0x20, 0x00, 0x29, 0x01, 0xc6, 0x00, 0x45, 0x00 });
    //ram.program(0x00, 0x0300, {0xa9, 0x80, 0x85, 0x01, 0x9c, 0x01, 0x00, 0xf2, 0x12, 0xf4, 0x38, 0xf4, 0x40});
    ram.program(0x00, 0x0300, {
        0xf4, 0x40, // xor.l %x0,%x0 (clr x0)
        0xd4, 0x80, 0xef, 0xbe, 0xad, 0xde, // add.l %x0, #$dead_beef
        0xf4, 0x08, // tr.l %x1, %x0
        0x00, // brk
    });
    cpu.tracing = true;
    cpu.allow65c02 = true;
    cpu.allow65x02 = true;

    cpu.reset(ram);
    cpu.execute_until_break(ram);
    ram.dump_memory(std::cout, 0, 0, 16, 8);
    ram.dump_memory(std::cout, cpu.SS, cpu.SP+1, 1, (0x1ff - cpu.SP) );
    x_dump_regs_info(std::cout, ram, cpu);
    std::cout << std::endl;
    return EXIT_SUCCESS;
}
