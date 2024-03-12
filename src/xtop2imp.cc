#include "xtop2imp.h"
#include "cpu65xops.h"
#include "utils.h"

void xtop2_decodeAndExecute(Memory& ram, CPU& cpu) {
   uint8_t xop = cpu.fetchByte(ram);
    auto subop = bitsv(xop, 7, 6);
    auto rd = bitsv(xop, 5, 3);
    auto rs = bitsv(xop, 2, 0);
    auto vs = cpu.register16(rs);
    switch(subop) {
        case 0: { // 32-bit register transfer
            auto res = (rs == rd) ? ~vs : vs;
            cpu.set_register16(rd, res);
            cpu.set_flags(NF_Mask | ZF_Mask, 16, res, 0);
            cpu.cycle();
            break;
        }
        case 1: { // 32-bit register, register xor
            auto vd = cpu.register16(rd);
            uint16_t res = vs ^ vd;
            cpu.set_register16(rd, res);
            cpu.set_flags(NF_Mask | ZF_Mask, 16, res, 0);
            cpu.cycle();
            break;
        }
        case 2: { // 32-bit register, register and
            auto vd = cpu.register16(rd);
            uint16_t res = vs & vd;
            cpu.set_register16(rd, res);
            cpu.set_flags(NF_Mask | ZF_Mask, 16, res, 0);
            cpu.cycle();
            break;
        }
        case 3: { // 32-bit register, register or
            auto vd = cpu.register16(rd);
            uint16_t res = vs | vd;
            cpu.set_register16(rd, res);
            cpu.set_flags(NF_Mask | ZF_Mask, 16, res, 0);
            cpu.cycle();
            break;
        }
        default: { cpu.illegalInstruction(XTOP2, xop); break; }
    }
}

void xtop2_math_decodeAndExecute(Memory& ram, CPU& cpu) {
    // 0xd2 cf ddd sss
    uint8_t xop = cpu.fetchByte(ram);
    auto const_flag = bitsv(xop, 7, 7);
    auto f = bitsv(xop, 6, 6);
    auto rd = bitsv(xop, 5, 3);
    auto rs = bitsv(xop, 2, 0);
    auto vd = cpu.register16(rd);
    auto vs = cpu.register16(rs);
    uint16_t con = 0;
    if(const_flag) con = cpu.fetchWord(ram); // read constant
    uint16_t res = 0;
    if(f == 0) {
        res = (rs == rd) ? vd + con : vd + vs + con;
    } else {
        res = (rs == rd) ? vd - con : vd - vs - con; 
    }
    cpu.set_register16(rd, res);
    cpu.set_flags(NF_Mask | ZF_Mask | CF_Mask | OF_Mask, 16, res, vd);
    cpu.cycle();  
}

void xtop2_stor_decodeAndExecute(Memory& ram, CPU& cpu) {
    uint8_t xop = cpu.fetchByte(ram);
    // xop: 00 iii rrr +1 : store r to zp address within DS with index register i
    //      01 iii rrr +2 : store r to word address within DS with index register i
    //      10 iii rrr +3 : store r to 24-bit address with index register i
    //      11 iii rrr +4 : store r to word address within SS with index register i
    auto addr_mode = bitsv(xop,7,6);
    auto ri = bitsv(xop,5,3);
    auto rs = bitsv(xop,2,0);
    auto seg = cpu.DS;
    uint16_t addr = 0;
    switch(addr_mode) {
        case 0: { // DS:ZP,rI
            addr = cpu.fetchByte(ram);
            break;
        }
        case 1: { // DS:ABS.W,rI
            addr = cpu.fetchWord(ram);
            break;
        }
        case 2: {// SEG:ABS.w,rI
            seg = cpu.fetchByte(ram);
            addr = cpu.fetchWord(ram);
            break;
        }
        case 3:
        default: {
            seg = cpu.SS;
            addr = cpu.fetchWord(ram);
            break;
        }
    }
    if (ri != rs) {
        addr += static_cast<int8_t>(cpu.register16(ri));
        cpu.cycle();
    }
    cpu.writeByte(ram, seg, addr, cpu.register16(rs));
}

