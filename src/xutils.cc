#include "xutils.h"

uint8_t getSegmentByIndex(const CPU& cpu, SegmentRegisterIndex index) {
    switch(index) {
        case PS: return cpu.PS;
        case SS: return cpu.SS;
        default: return cpu.DS;
    }
}

uint8_t getImpliedIndex(uint8_t rs) {
    switch(rs) {
        case 0: return 4;
        case 1: return 5;
        case 2: return 6;
        case 3: return 7;
        case 4: return 0;
        case 5: return 1;
        case 6: return 2;
        default /* 7 */: return 3;
    }
}

void x_dump_regs_info(std::ostream& ostr, Memory& ram, CPU& cpu) {
    ostr << format("PC=%02X:%04X SP=%02X:%04X P=%02X", cpu.PS, cpu.PC, cpu.SS, cpu.SP, cpu.P.asByte());
    ostr << " (" << std::bitset<8>(cpu.P.asByte()) << ")" << std::endl;
    for(int i=0; i<8; i++) {
        if(i) ostr << " ";
        ostr << format("d%d=%02X", i, cpu.register8(i));
    }
    ostr << std::endl;
    for(int i=0; i<8; i++) {
        if(i) ostr << " ";
        ostr << format("w%d=%04X", i, cpu.register16(i));
    }
    ostr << std::endl;
    for(int i=0; i<8; i++) {
        if(i) ostr << " ";
        ostr << format("x%d=%08X", i, cpu.reg32[i]);
    }
    ostr << std::endl;
}
