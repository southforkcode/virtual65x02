#ifndef __CPU65XOPS_H
#define __CPU65XOPS_H

#include <stdint.h>

enum eOpcodes : uint16_t {
    ADC_Immediate = 0x69,
    ADC_ZeroPage = 0x65,
    ADC_ZeroPageX = 0x75,
    ADC_Absolute = 0x6d,
    ADC_AbsoluteX = 0x7d,
    ADC_AbsoluteY = 0x79,
    ADC_IndirectX = 0x61,
    ADC_IndirectY = 0x71,

    AND_Immediate = 0x29,
    AND_ZeroPage = 0x25,
    AND_ZeroPageX = 0x35,
    AND_Absolute = 0x2d,
    AND_AbsoluteX = 0x3d,
    AND_AbsoluteY = 0x39,
    AND_IndirectX = 0x21,
    AND_IndirectY = 0x31,

    ASL_Implied = 0x0a,
    ASL_ZeroPage = 0x06,
    ASL_ZeroPageX = 0x16,
    ASL_Absolute = 0x0e,
    ASL_AbsoluteX = 0x1e,

    BCC = 0x90,
    BCS = 0xb0,
    BEQ = 0xf0,
    BMI = 0x30,
    BNE = 0xd0,
    BPL = 0x10,
    BVC = 0x50,
    BVS = 0x70,

    BIT_ZeroPage = 0x24,
    BIT_Absolute = 0x2c,

    BRK = 0x00,

    CLC = 0x18,
    CLD = 0xd8,
    CLI = 0x58,
    CLV = 0xb8,

    CMP_Immediate = 0xc9,
    CMP_ZeroPage = 0xc5,
    CMP_ZeroPageX = 0xd5,
    CMP_Absolute = 0xcd,
    CMP_AbsoluteX = 0xdd,
    CMP_AbsoluteY = 0xd9,
    CMP_IndirectX = 0xc1,
    CMP_IndirectY = 0xd1,

    CPX_Immediate = 0xe0,
    CPX_ZeroPage = 0xe4,
    CPX_Absolute = 0xec,

    CPY_Immediate = 0xc0,
    CPY_ZeroPage = 0xc4,
    CPY_Absolute = 0xcc,

    DEC_ZeroPage = 0xc6,
    DEC_ZeroPageX = 0xd6,
    DEC_Absolute = 0xce,
    DEC_AbsoluteX = 0xde,

    DEX = 0xca,
    DEY = 0x88,

    EOR_Immediate = 0x49,
    EOR_ZeroPage = 0x45,
    EOR_ZeroPageX = 0x55,
    EOR_Absolute = 0x4d,
    EOR_AbsoluteX = 0x5d,
    EOR_AbsoluteY = 0x59,
    EOR_IndirectX = 0x41,
    EOR_IndirectY = 0x51,

    INC_ZeroPage = 0xe6,
    INC_ZeroPageX = 0xf6,
    INC_Absolute = 0xee,
    INC_AbsoluteX = 0xfe,

    INX = 0xe8,
    INY = 0xc8,

    JMP_Absolute = 0x4c,
    JMP_Indirect = 0x6c,
    
    JSR_Absolute = 0x20,

    LDA_Immediate = 0xa9,
    LDA_ZeroPage = 0xa5,
    LDA_ZeroPageX = 0xb5,
    LDA_Absolute = 0xad,
    LDA_AbsoluteX = 0xbd,
    LDA_AbsoluteY = 0xb9,
    LDA_IndirectX = 0xa1,
    LDA_IndirectY = 0xb1,

    LDX_Immediate = 0xa2,
    LDX_ZeroPage = 0xa6,
    LDX_ZeroPageY = 0xb6,
    LDX_Absolute = 0xae,
    LDX_AbsoluteY = 0xbe,

    LDY_Immediate = 0xa0,
    LDY_ZeroPage = 0xa4,
    LDY_ZeroPageX = 0xb4,
    LDY_Absolute = 0xac,
    LDY_AbsoluteX = 0xbc,

    LSR_Implied = 0x4a,
    LSR_ZeroPage = 0x46,
    LSR_ZeroPageX = 0x56,
    LSR_Absolute = 0x4e,
    LSR_AbsoluteX = 0x5e,

    NOP = 0xea,

    ORA_Immediate = 0x09,
    ORA_ZeroPage = 0x05,
    ORA_ZeroPageX = 0x15,
    ORA_Absolute = 0x0d,
    ORA_AbsoluteX = 0x1d,
    ORA_AbsoluteY = 0x19,
    ORA_IndirectX = 0x01,
    ORA_IndirectY = 0x11,

    PHA = 0x48,
    PHP = 0x08,
    PLA = 0x68,
    PLP = 0x28,

    ROL_Implied = 0x2a,
    ROL_ZeroPage = 0x26,
    ROL_ZeroPageX = 0x36,
    ROL_Absolute = 0x2e,
    ROL_AbsoluteX = 0x3e,

    ROR_Implied = 0x6a,
    ROR_ZeroPage = 0x66,
    ROR_ZeroPageX = 0x76,
    ROR_Absolute = 0x6e,
    ROR_AbsoluteX = 0x7e,

    RTI = 0x40,
    RTS = 0x60,

    SBC_Immediate = 0xe9,
    SBC_ZeroPage = 0xe5,
    SBC_ZeroPageX = 0xf5,
    SBC_Absolute = 0xed,
    SBC_AbsoluteX = 0xfd,
    SBC_AbsoluteY = 0xf9,
    SBC_IndirectX = 0xe1,
    SBC_IndirectY = 0xf1,

    SEC = 0x38,
    SED = 0xf8,
    SEI = 0x78,

    STA_ZeroPage = 0x85,
    STA_ZeroPageX = 0x95,
    STA_Absolute = 0x8d,
    STA_AbsoluteX = 0x9d,
    STA_AbsoluteY = 0x99,
    STA_IndirectX = 0x81,
    STA_IndirectY = 0x91,
    
    STX_ZeroPage = 0x86,
    STX_ZeroPageY = 0x96,
    STX_Absolute = 0x8e,

    STY_ZeroPage = 0x84,
    STY_ZeroPageX = 0x94,
    STY_Absolute = 0x8c,

    TAX = 0xaa,
    TAY = 0xa8,
    TSX = 0xba,
    TXA = 0x8a,
    TXS = 0x9a,
    TYA = 0x98,

    /* 65C02 Opcodes */
    BRA = 0x80,

    STZ_ZeroPage = 0x64,
    STZ_ZeroPageX = 0x74,
    STZ_Absolute = 0x9c,
    STZ_AbsoluteX = 0x9e,

    PHX = 0xda,
    PHY = 0x5a,
    PLX = 0xfa,
    PLY = 0x7a,

    TRB_ZeroPage = 0x14,
    TRB_Absolute = 0x1c,

    TSB_ZeroPage = 0x04,
    TSB_Absolute = 0x0c,

    /* 65X02 Opcodes */

    /*
        0xf2 00 bbb aaa:        transfer byte register b->a, negate byte register when b==a
        0xf2 01 rrr Dss:        transfer byte data register into segment register
                                00 - DS     D
                                01 - PS     0 = from register to segment register
                                10 - XS     1 = from segment register to register
                                11 - SS        
    */
    XTOP1 = 0xf2,
    XTOP2 = 0xf3,
    XTOP3 = 0xf4,

    XTOP1_TRX = 0xf7,

    /*
        0xd2 ff bbb aaa:        register-to-register math
        0xd3                    16-bit r-to-r math
        0xd4                    32-bit r-to-r math
    */
    XTOP1_MATH = 0xd2,
    XTOP2_MATH = 0xd3,
    XTOP3_MATH = 0xd4,


    XTOP1_STOR = 0xfa,
    XTOP2_STOR = 0xfb,
    XTOP3_STOR = 0xfc,

};

#endif
