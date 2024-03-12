#ifndef __CPU65X_H
#define __CPU65X_H

#include <cstdint>
#include <cstdbool>

#include <iostream>
#include <iomanip>
#include <bitset>
#include <exception>

#include "memory.h"
#include "utils.h"

enum ProcessorState {
    Reset,
    Halt,
    Normal
};

enum FlagMasks: uint8_t {
    NF_Mask = 1 << 7,
    OF_Mask = 1 << 6,
    BF_Mask = 1 << 4,
    DF_Mask = 1 << 3,
    IF_Mask = 1 << 2,
    ZF_Mask = 1 << 1,
    CF_Mask = 1
};

struct ProcessorStatus {
    uint8_t CF : 1;
    uint8_t ZF : 1;
    uint8_t IF : 1;
    uint8_t DF : 1;
    uint8_t BF : 1;
    uint8_t OF : 1;
    uint8_t NF : 1;

    uint8_t asByte() {
        return (NF << 7) | (OF << 6) | (1 << 5) | (BF << 4) | (DF << 3) | (IF << 2) | (ZF << 1) | (CF);
    }

    void setByte(uint8_t byte) {
        NF = (byte & 0x80) >> 7;
        OF = (byte & 0x40) >> 6;
        BF = (byte & 0x10) >> 4;
        DF = (byte & 0x08) >> 3;
        IF = (byte & 0x04) >> 2;
        ZF = (byte & 0x02) >> 1;
        CF = (byte & 0x01);
    }
};

struct CPU {
    ProcessorState state = Reset;

    uint16_t PC;
    uint16_t SP;
    struct ProcessorStatus P;

    uint32_t reg32[8];

    uint8_t opSeg; // used for reporting illegal instructions
    uint16_t opPC;

    uint8_t PS; // X program segment
    uint8_t DS; // X data segment
    uint8_t SS; // X stack segment
    
    uint16_t OP;

    bool tracing = false;
    bool ignoreIllegalInstructions = true;
    bool allowHalting = false;

    bool haltOnBRK = false;

    bool allow65c02 = true;
    bool allow65x02 = true;

    uint64_t cycles;
    unsigned opCC;

    void init() {
        state = Reset;
        SP = 0x1ff;
        for(int i=0; i<8; i++) reg32[i] = 0;
        P.CF = P.ZF = P.IF = P.DF = P.BF = P.OF = P.NF = 0;
        // Xtra
        PS = 0; // we boot at 00:fffc
        DS = 0; // we load/store data at 00:xxxx
        SS = 0; // we push/pop from 00:xxxx
        // other
        cycles = 0;
    }

    // performs a full reset of the cpu
    // and returns with the PC at the specified reset vector
    void reset(Memory& ram);

    void execute_next_instruction(Memory& ram);
    void execute_until_break(Memory& ram);
    void decodeAndExecute(Memory& ram, uint16_t opcode);
    
    void illegalInstruction();
    void illegalInstruction(uint8_t inst);
    void illegalInstruction(uint8_t inst0, uint8_t inst1);

    inline uint8_t A() const { return register8(1); }
    inline uint8_t X() const { return register8(3); }
    inline uint8_t Y() const { return register8(5); }
    inline void setA(uint8_t v) { set_register8(1, v); }
    inline void setX(uint8_t v) { set_register8(3, v); }
    inline void setY(uint8_t v) { set_register8(5, v); }

    uint8_t register8(uint8_t sel) const;
    void set_register8(uint8_t sel, uint8_t val);
    uint16_t register16(uint8_t sel) const;
    void set_register16(uint8_t sel, uint16_t val);
    uint32_t register32(uint8_t sel) const;
    void set_register32(uint8_t sel, uint32_t val);

    uint8_t registerPS() const;
    void set_registerPS(uint8_t val);
    uint8_t registerDS() const;
    void set_registerDS(uint8_t val);
    uint8_t registerSS() const;
    void set_registerSS(uint8_t val);
    
    void branch_relative8_if(int8_t rel, bool cond);
    void pushByte(Memory& ram, uint8_t byte);
    uint8_t popByte(Memory& ram);
    uint8_t fetchByte(Memory& ram);
    uint16_t fetchWord(Memory& ram);
    uint32_t fetchLongWord(Memory& ram);
    uint8_t readByte(Memory& ram, uint8_t seg, uint16_t addr);
    uint16_t readWord(Memory& ram, uint8_t seg, uint16_t addr);
    uint32_t readLongWord(Memory& ram, uint8_t seg, uint16_t addr);

    void writeByte(Memory& ram, uint8_t seg, uint16_t addr, uint8_t byte);
    void writeWord(Memory& ram, uint8_t seg, uint16_t addr, uint16_t word);
    void writeLongWord(Memory& ram, uint8_t seg, uint16_t addr, uint32_t word);
    void cycle();

    // flags is a mask matching the flags in cpu.P
    // only CF, ZF, NF, and VF are settable with this function
    void set_flags(uint8_t flags, size_t size, unsigned v1, unsigned v2);

    void dump_regs_info(std::ostream& ostr) {
        ostr << format("PC=%04X SP=%04X A=%02X X=%02X Y=%02X P=%02X", PC, SP, A(), X(), Y(), P.asByte());
        ostr << " (" << std::bitset<8>(P.asByte()) << ")";
    }

};

#endif