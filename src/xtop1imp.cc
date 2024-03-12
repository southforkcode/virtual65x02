#include "xtop1imp.h"
#include "cpu65xops.h"

#include "utils.h"

#include <iostream>
#include <bitset>

/*
    8 8-bit registers, d0..d7, where d0=A, d2=X, d3=Y
    8 16-bit registers, w0..w7, overlapping d0..d7, overlapping A, X, Y, w0= (A, B), w1=(X, Y)
    8 32-bit registers, x0..x7, overlapping w0..w3, overlapping d0, d2, d3
*/

void xtop1_decodeAndExecute(Memory& ram, CPU& cpu) {
    uint8_t opcode = cpu.fetchByte(ram);
    auto subop = bitsv(opcode, 7, 6);
    auto params = bitsv(opcode, 5, 0);
    auto rd = bitsv(opcode, 5, 3);
    auto rs = bitsv(opcode, 2, 0);
    auto vs = cpu.register8(rs);
    switch(subop) {
        case 0: {
            auto res = (rs == rd) ? ~vs : vs;
            cpu.set_register8(rd, res);
            cpu.cycle(); // one cycle for execute
            break;
        }
        case 1: { // bidirectional register/segment register transfer
            auto D = bitsv(opcode, 2, 2); // direction bit
            auto SR = bitsv(opcode, 1, 0);
            break;
        }
        default: { cpu.illegalInstruction(XTOP1, opcode); break; }
    }
}

void xtop1_trx_decodeAndExecute(Memory& ram, CPU& cpu) {
    uint8_t opcode = cpu.fetchByte(ram);
    auto subop = bitsv(opcode, 7, 7);
    auto dir = bitsv(opcode, 6, 6);
    auto d8r = bitsv(opcode, 5, 3);
    auto segr = bitsv(opcode, 2, 0);
    if(subop == 0) {
        switch(segr) {
            case 0: {
                if(dir) cpu.set_register8(d8r, cpu.registerPS());
                else cpu.set_registerPS(cpu.register8(d8r));
                break;
            }
            case 1: {
                if(dir) cpu.set_register8(d8r, cpu.registerDS());
                else cpu.set_registerDS(cpu.register8(d8r));
                break;
            }
            case 2: {
                if(dir) cpu.set_register8(d8r, cpu.registerSS());
                else cpu.set_registerSS(cpu.register8(d8r));
                break;
            }
            default: { cpu.illegalInstruction(XTOP1_TRX, opcode); break; }
        }
    } else {
        cpu.illegalInstruction(XTOP1_TRX, opcode);
    }
}

void xtop1_math_decodeAndExecute(Memory& ram, CPU& cpu) {
    // 0xd2 cf ddd sss
    uint8_t xop = cpu.fetchByte(ram);
    auto const_flag = bitsv(xop, 7, 7);
    auto f = bitsv(xop, 6, 6);
    auto rd = bitsv(xop, 5, 3);
    auto rs = bitsv(xop, 2, 0);
    auto vd = cpu.register8(rd);
    auto vs = cpu.register8(rs);
    uint8_t con = 0;
    if(const_flag) con = cpu.fetchByte(ram); // read constant
    uint8_t res = 0;
    if(f == 0) {
        res = (rs == rd) ? vd + con : vd + vs + con;
    } else {
        res = (rs == rd) ? vd - con : vd - vs - con; 
    }
    cpu.set_register8(rd, res);
    cpu.set_flags(NF_Mask | ZF_Mask | CF_Mask | OF_Mask, 8, res, vd);
    cpu.cycle();  
}

void xtop1_stor_decodeAndExecute(Memory& ram, CPU& cpu) {
    uint8_t xop = cpu.fetchByte(ram);
    // xop: 00 iii rrr +1 : store r to zp address within DS with index register i
    //      01 iii rrr +2 : store r to word address within DS with index register i
    //      10 iii rrr +3 : store r to 24-bit address with index register i
    //      11 iii rrr +1 : store r to SS:SP+CON,%i
    auto addr_mode = bitsv(xop,7,6);
    auto ri = bitsv(xop,5,3);
    auto rs = bitsv(xop,2,0);
    auto seg = cpu.DS;
    uint16_t addr = 0;
    switch(addr_mode) {
        case 0: { // DS:ZP,rI
            seg = cpu.DS;
            addr = cpu.fetchByte(ram);            
            break;
        }
        case 1: { // DS:ABS.W,rI
            addr = cpu.fetchWord(ram);
            break;
        }
        case 2: {// SEG:ABS.w,rI
            addr = cpu.fetchWord(ram);
            seg = cpu.fetchByte(ram);
            break;
        }
        case 3:
        default: {
            seg = cpu.SS;
            addr = cpu.SP;
            auto off = cpu.fetchByte(ram);
            addr += static_cast<int8_t>(off);
            cpu.cycle();
            break;
        }
    }
    if (ri != rs) {
        addr += static_cast<int8_t>(cpu.register8(ri));
        cpu.cycle();
    }
    cpu.writeByte(ram, seg, addr, cpu.register8(rs));
}
