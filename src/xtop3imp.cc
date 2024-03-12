#include "xtop3imp.h"
#include "cpu65xops.h"

#include "utils.h"
#include "xutils.h"

void xtop3_decodeAndExecute(Memory& ram, CPU& cpu) {
    uint8_t xop = cpu.fetchByte(ram);
    auto subop = bitsv(xop, 7, 6);
    auto rd = bitsv(xop, 5, 3);
    auto rs = bitsv(xop, 2, 0);
    auto vs = cpu.register32(rs);
    switch(subop) {
        case 0: { // 32-bit register transfer + not
            auto res = (rs == rd) ? ~vs : vs;
            cpu.set_register32(rd, res);
            cpu.set_flags(NF_Mask | ZF_Mask, 32, res, 0);
            cpu.cycle();
            break;
        }
        case 1: { // 32-bit register, register xor
            auto vd = cpu.register32(rd);
            uint32_t res = vs ^ vd;
            cpu.set_register32(rd, res);
            cpu.set_flags(NF_Mask | ZF_Mask, 32, res, 0);
            cpu.cycle();
            break;
        }
        case 2: { // 32-bit register, register and
            auto vd = cpu.register32(rd);
            uint32_t res = vs & vd;
            cpu.set_register32(rd, res);
            cpu.set_flags(NF_Mask | ZF_Mask, 32, res, 0);
            cpu.cycle();
            break;
        }
        case 3: { // 32-bit register, register or
            auto vd = cpu.register32(rd);
            uint32_t res = vs | vd;
            cpu.set_register32(rd, res);
            cpu.set_flags(NF_Mask | ZF_Mask, 32, res, 0);
            cpu.cycle();
            break;
        }
        default: { cpu.illegalInstruction(XTOP3, xop); break; }
    }
}

void xtop3_math_decodeAndExecute(Memory& ram, CPU& cpu) {
    // 0xd4 cf ddd sss
    uint8_t xop = cpu.fetchByte(ram);
    auto const_flag = bitsv(xop, 7, 7);
    auto f = bitsv(xop, 6, 6);
    auto rd = bitsv(xop, 5, 3);
    auto rs = bitsv(xop, 2, 0);
    auto vd = cpu.register32(rd);
    auto vs = cpu.register32(rs);
    uint32_t con = 0;
    if(const_flag) {
        con = cpu.fetchLongWord(ram);
    }
    uint32_t res = 0;
    if(f == 0) {
        res = (rs == rd) ? vd + con : vd + vs + con;
    } else {
        res = (rs == rd) ? vd - con : vd - vs - con; 
    }
    cpu.set_register32(rd, res);
    cpu.set_flags(NF_Mask | ZF_Mask | CF_Mask | OF_Mask, 32, res, vd);
    cpu.cycle();  
}

void xtop3_stor_decodeAndExecute(Memory& ram, CPU& cpu) {
    uint8_t xop = cpu.fetchByte(ram);
    // xop: 00 iii rrr +1 : store r to zp address within DS with index register i
    //      01 iii rrr +2 : store r to word address within DS with index register i
    //      10 iii rrr +3 : store r to 24-bit address with index register i
    //      11 iii rrr +1 : store r to register indirect
    //                      00 lll hhh -> st %d0, (%d1,%d2), %d3 // st %d0, (%d1), %d2
    //                      01 mmS www -> st %d0, (DS:%w1), %d3
    //                                    st %d0, (DS:%w1), %d3++
    //                                    st %d0, (DS:%w1), --%d3
    //                                    st %d0, (PS:%w1), etc.
    //                      01 11S www -> undefined
    //                      10 
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
        addr += static_cast<int8_t>(cpu.register32(ri));
        cpu.cycle();
    }
    cpu.writeByte(ram, seg, addr, cpu.register32(rs));
}

void xtop3_regind_decodeAndExecute(Memory& ram, CPU& cpu) {
    uint8_t xop = cpu.fetchByte(ram);
    // store r to register indirect
    // 0 mm SS rrr (implied)
    //                       : st (SS:Wr)
    //                       : st (SS:Wr,Wi)
    //                       : st (SS:Wr,Wi++)
    //                       : st (SS:Wr,--Wi)
    // 1 mm SS rrr iii ccccc :
    //                       : st (SS:Wr + C)
    //                       : st (SS:Wr,Wi + C)
    //                       : st (SS:Wr,Wi++ + C)
    //                       : st (SS:Wr,--Wi + C)
    auto f  = bitsv(xop, 7, 7);
    auto mm = bitsv(xop, 6, 5);
    auto SS = bitsv(xop, 4, 3);
    auto rs = bitsv(xop, 2, 0);
    auto seg = getSegmentByIndex(cpu, static_cast<SegmentRegisterIndex>(SS));
    auto adr = cpu.register16(rs);
    auto ri = getImpliedIndex(rs);
    auto idx = cpu.register16(rs);
    int8_t con = 0;
    bool writeIndex = (mm == 2 || mm == 3);
    
    if(f == 1) {
        uint8_t xop2 = cpu.fetchByte(ram);
        auto ri = bitsv(xop2,7,5);
        idx = cpu.register16(ri);
        con = static_cast<int8_t>(static_cast<uint8_t>(bitsv(xop2,4,0)));
    }

    switch(mm) {
        case 1: { adr += idx; break; }
        case 2: { adr += idx; idx += 4; break; }
        case 3: { idx -= 4; adr += idx; break; }
        default: break;
    }

    cpu.writeLongWord(ram, seg, adr, cpu.register32(rs));

    if(writeIndex) {
        // write index value back
        cpu.set_register16(ri, idx);
        cpu.cycle();
    }
}
