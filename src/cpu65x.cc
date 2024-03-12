#include "cpu65x.h"
#include "cpu65xops.h"
#include "xtop1imp.h"
#include "xtop2imp.h"
#include "xtop3imp.h"

#define CHECK_CPU_MODE(mode, op) do { if(!mode) { illegalInstruction(op); } } while(0)

void CPU::reset(Memory& ram) {
    init();
    auto resetv = readWord(ram, 0, 0xfffc);
    if(tracing) std::cout << format("Reset vector: 00:%04X\n", resetv);
    PC = resetv;
    PS = 0;
    DS = 0;
    SS = 0;
    state = Normal;
    if(tracing) std::cout << format("CPU: NORMAL @ %02X:%04X\n", PS, PC);
}

void CPU::execute_until_break(Memory& ram) {
    auto oldHaltOnBRK = haltOnBRK;
    if(state == Halt) state = Normal; // unhalt the CPU
    haltOnBRK = true;
    do {
        execute_next_instruction(ram);
    } while (state == Normal);
    if (tracing && OP == BRK) {
        std::cout << "Stopping on BRK" << std::endl;
    } else if (tracing && state == Halt) {
        std::cout << "CPU is halted\n";
    } else if (tracing && state == Reset) {
        std::cout << "CPU was reset\n";
    } else if (tracing) {
        std::cout << "Stopped for unknown reason.\n";
    }
    haltOnBRK = oldHaltOnBRK;
}

void CPU::execute_next_instruction(Memory& ram) {
    switch(state) {
        case Reset: {
            reset(ram);
            return;
        }
        case Halt: {
            if(tracing) std::cout << format("CPU:HALT\n");
            return;
        }
        default: {
            // load operation aka "fetch"
            opSeg = PS;
            opPC = PC;
            auto start = cycles;
            OP = fetchByte(ram);
            if (tracing) {
                std::cout << format("OP=%02X", OP) << std::endl;
            }
            decodeAndExecute(ram, OP);
            auto end = cycles;
            // capture how many cycles this instruction took
            opCC = end - start;
            if (tracing) {
                dump_regs_info(std::cout);
                std::cout << std::endl;
            }
            break;
        }
    }
}

void CPU::illegalInstruction() {
    if(ignoreIllegalInstructions) {
        return;
    }
    else if(allowHalting) {
        state = Halt;
    } else {
        state = Reset;
    }
}

void CPU::illegalInstruction(uint8_t inst) {
    if (tracing) {
        std::cout << format("%02X:%04X=%02X ILLEGAL INSTRUCTION", opSeg, opPC, inst) << std::endl;
    }
    illegalInstruction();
}
void CPU::illegalInstruction(uint8_t inst0, uint8_t inst1) {
    if(tracing) {
        std::cout << format("%02X:%04X=%02X %02X ILLEGAL INSTRUCTION", opSeg, opPC, inst0, inst1) << std::endl;
    }
    illegalInstruction();
}

void CPU::decodeAndExecute(Memory& ram, uint16_t opcode) {
    switch(opcode) {
        case ADC_Immediate: {
            auto imm = fetchByte(ram);
            auto a = A();
            auto v = a + imm + P.CF;
            setA(v);
            set_flags(NF_Mask | ZF_Mask | CF_Mask | OF_Mask, 8, v, a);
            break;
        }
        case ADC_ZeroPage:
        case ADC_ZeroPageX: {
            auto zp = fetchByte(ram);
            if (opcode == ADC_ZeroPageX) {
                zp += X();
                cycle();
            }
            auto v = readByte(ram, DS, zp);
            auto a = A();
            auto v2 = a + v + P.CF;
            setA(v2);
            set_flags(NF_Mask | ZF_Mask | CF_Mask | OF_Mask, 8, v2, a);
            break;
        }
        case ADC_Absolute:
        case ADC_AbsoluteX:
        case ADC_AbsoluteY: {
            uint16_t adr = fetchByte(ram) | fetchByte(ram) << 8;
            adr += opcode == ADC_AbsoluteX ? X() : opcode == ADC_AbsoluteY ? Y() : 0;
            auto v = readByte(ram, DS, adr);
            auto a = A();
            auto v2 = a + v + P.CF;
            setA(v2);
            set_flags(NF_Mask | ZF_Mask | CF_Mask | OF_Mask, 8, v2, a);
            break;
        }
        case ADC_IndirectX: {
            uint8_t zp = fetchByte(ram) + X();
            cycle();
            uint16_t adr = readByte(ram, DS, zp) | readByte(ram, DS, zp+1) << 8;
            auto v = readByte(ram, DS, adr);
            auto a = A();
            auto v2 = a + v + P.CF;
            setA(v2);
            set_flags(NF_Mask | ZF_Mask | CF_Mask | OF_Mask, 8, v2, a);
            break;
        }
        case ADC_IndirectY: {
            uint8_t zp = fetchByte(ram);
            uint16_t adr = readByte(ram, DS, zp) | readByte(ram, DS, zp+1) << 8;
            auto v = readByte(ram, DS, adr + Y());
            auto a = A();
            auto v2 = a + v + P.CF;
            setA(v2);
            set_flags(NF_Mask | ZF_Mask | CF_Mask | OF_Mask, 8, v2, a);
            break;
        }

        case AND_Immediate: {
            auto imm = fetchByte(ram);
            auto v = A() & imm;
            setA(v);
            set_flags(NF_Mask | ZF_Mask, 8, v, 0);
            break;
        }
        case AND_ZeroPage:
        case AND_ZeroPageX: {
            auto zp = (uint8_t)(fetchByte(ram) + (opcode == AND_ZeroPageX ? X() : 0));
            auto v = readByte(ram, DS, zp);
            auto v2 = A() & v;
            setA(v2);
            set_flags(NF_Mask | ZF_Mask, 8, v2, 0);
            break;
        }
        case AND_Absolute:
        case AND_AbsoluteX:
        case AND_AbsoluteY: {
            uint16_t adr = fetchByte(ram) | fetchByte(ram) << 8;
            adr += opcode == AND_AbsoluteX ? X() : opcode == AND_AbsoluteY ? Y() : 0;
            auto v = readByte(ram, DS, adr);
            auto v2 = A() & v;
            setA(v2);
            set_flags(NF_Mask | ZF_Mask, 8, v2, 0);
            break;
        }
        case AND_IndirectX: {
            uint8_t zp = fetchByte(ram) + X();
            cycle();
            uint16_t adr = readByte(ram, DS, zp) | readByte(ram, DS, zp+1) << 8;
            auto v = readByte(ram, DS, adr);
            auto v2 = A() & v;
            setA(v2);
            set_flags(NF_Mask | ZF_Mask, 8, v2, 0);
            break;
        }
        case AND_IndirectY: {
            uint8_t zp = fetchByte(ram);
            uint16_t adr = readByte(ram, DS, zp) | readByte(ram, DS, zp+1) << 8;
            auto v = readByte(ram, DS, adr + Y());
            auto v2 = A() & v;
            setA(v2);
            set_flags(NF_Mask | ZF_Mask, 8, v2, 0);
            break;
        }

        case ASL_Implied: {
            cycle();
            P.CF = A() & 0x80;
            auto v = (A() << 1);
            setA(v);
            set_flags(NF_Mask | ZF_Mask, 8, v, 0);
            break;
        }
        case ASL_ZeroPage: { 
            auto zp = fetchByte(ram);
            auto res = readByte(ram, DS, zp);
            P.CF = res & 0x80;
            res = (res << 1);
            cycle();
            writeByte(ram, DS, zp, res);
            set_flags(NF_Mask | ZF_Mask, 8, res, 0);
            break;
        }
        case ASL_ZeroPageX: {
            auto zp = fetchByte(ram);
            zp = (zp + X()) & 0xff;
            cycle();
            auto res = readByte(ram, DS, zp);
            P.CF = res & 0x80;
            res = (res << 1);
            cycle();
            writeByte(ram, DS, zp, res);
            set_flags(NF_Mask | ZF_Mask, 8, res, 0);
            break;
        }
        case ASL_Absolute: {
            uint16_t adr = fetchByte(ram) | fetchByte(ram) << 8;
            auto res = readByte(ram, DS, adr);
            P.CF = res & 0x80;
            res = (res << 1);
            cycle();
            writeByte(ram, DS, adr, res);
            set_flags(NF_Mask | ZF_Mask, 8, res, 0);
            break;
        }
        case ASL_AbsoluteX: {
            uint16_t adr = fetchByte(ram) | fetchByte(ram) << 8;
            adr = (adr + X()) & 0xffff;
            cycle();
            auto res = readByte(ram, DS, adr);
            P.CF = res & 0x80;
            res = (res << 1);
            cycle();
            writeByte(ram, DS, adr, res);
            set_flags(NF_Mask | ZF_Mask, 8, res, 0);
            break;
        }

        case BCC: {
            auto rel = (int8_t)fetchByte(ram);
            branch_relative8_if(rel, P.CF == 1);
            break;
        }
        case BCS: {
            auto rel = (int8_t)fetchByte(ram);
            branch_relative8_if(rel, P.CF == 0);
            break;
        }
        case BEQ: {
            auto rel = (int8_t)fetchByte(ram);
            branch_relative8_if(rel, P.ZF == 1);
            break;
        }
        case BMI: {
            auto rel = (int8_t)fetchByte(ram);
            branch_relative8_if(rel, P.NF == 1);
            break;
        }
        case BNE: {
            auto rel = (int8_t)fetchByte(ram);
            branch_relative8_if(rel, P.ZF == 0);
            break;
        }
        case BPL: {
            auto rel = (int8_t)fetchByte(ram);
            branch_relative8_if(rel, P.NF == 0);
            break;
        }
        case BVC: {
            auto rel = (int8_t)fetchByte(ram);
            branch_relative8_if(rel, P.OF == 0);
            break;
        }
        case BVS: {
            auto rel = (int8_t)fetchByte(ram);
            branch_relative8_if(rel, P.OF == 1);
            break;
        }

        case BIT_ZeroPage: {
            auto zp = fetchByte(ram);
            auto v = readByte(ram, DS, zp);
            P.NF = (v & 0x80) ? 1 : 0;
            P.OF = (v & 0x40) ? 1 : 0;
            P.ZF = ((A() & v) == 0) ? 1 : 0;
            break;
        }
        case BIT_Absolute: {
            uint16_t adr = fetchByte(ram) | fetchByte(ram) << 8;
            auto v = readByte(ram, DS, adr);
            P.NF = (v & 0x80) ? 1 : 0;
            P.OF = (v & 0x40) ? 1 : 0;
            P.ZF = ((A() & v) == 0) ? 1 : 0;
            break;
        }

        case BRK: {
            auto ret_adr = PC + 2;
            pushByte(ram, ret_adr >> 8);
            pushByte(ram, ret_adr & 0xff);
            pushByte(ram, P.asByte());
            uint16_t adr = readByte(ram, 0, 0xfffe) | readByte(ram, 0, 0xffff) << 8;
            P.IF = 1;
            DS = 0;
            PS = 0;
            SS = 0;
            PC = adr;
            if(haltOnBRK) state = Halt;
            break;
        }

        case CLC: {
            P.CF = 0;
            cycle();
            break;
        }
        case CLD: {
            P.DF = 0;
            cycle();
            break;
        }
        case CLI: {
            P.IF = 0;
            cycle();
            break;
        }
        case CLV: {
            P.OF = 0;
            cycle();
            break;
        }

        case CMP_Immediate: {
            auto imm = fetchByte(ram);
            auto res = A() - imm;
            set_flags(NF_Mask | ZF_Mask | CF_Mask, 8, res, A());
            break;
        }
        case CMP_ZeroPage:
        case CMP_ZeroPageX: {
            auto zp = (uint8_t)(fetchByte(ram) + (opcode == CMP_ZeroPageX ? X() : 0));
            auto v = readByte(ram, DS, zp);
            auto res = A() - v;
            set_flags(NF_Mask | ZF_Mask | CF_Mask, 8, res, A());
            break;
        }
        case CMP_Absolute:
        case CMP_AbsoluteX:
        case CMP_AbsoluteY: {
            uint16_t adr = fetchByte(ram) | fetchByte(ram) << 8;
            adr += opcode == CMP_AbsoluteX ? X() : opcode == CMP_AbsoluteY ? Y() : 0;
            auto v = readByte(ram, DS, adr);
            auto res = A() - v;
            set_flags(NF_Mask | ZF_Mask | CF_Mask, 8, res, A());
            break;
        }
        case CMP_IndirectX: {
            uint8_t zp = fetchByte(ram) + X();
            cycle();
            uint16_t adr = readByte(ram, DS, zp) | readByte(ram, DS, zp+1) << 8;
            auto v = readByte(ram, DS, adr);
            auto res = A() - v;
            set_flags(NF_Mask | ZF_Mask | CF_Mask, 8, res, A());
            break;
        }
        case CMP_IndirectY: {
            uint8_t zp = fetchByte(ram);
            uint16_t adr = readByte(ram, DS, zp) | readByte(ram, DS, zp+1) << 8;
            auto v = readByte(ram, DS, adr + Y());
            auto res = A() - v;
            set_flags(NF_Mask | ZF_Mask | CF_Mask, 8, res, A());
            break;
        }

        case CPX_Immediate: {
            uint8_t imm = fetchByte(ram);
            auto res = X() - imm;
            set_flags(NF_Mask | ZF_Mask | CF_Mask, 8, res, X());
            break;
        }
        case CPX_ZeroPage: {
            uint8_t zp = fetchByte(ram);
            uint8_t v = readByte(ram, DS, zp);
            auto res = X() - v;
            set_flags(NF_Mask | ZF_Mask | CF_Mask, 8, res, X());
            break;
        }
        case CPX_Absolute: {
            uint16_t adr = fetchByte(ram) | fetchByte(ram) << 8;
            uint8_t v = readByte(ram, DS, adr);
            auto res = X() - v;
            set_flags(NF_Mask | ZF_Mask | CF_Mask, 8, res, X());
            break;
        }

        case CPY_Immediate: {
            uint8_t imm = fetchByte(ram);
            auto res = Y() - imm;
            set_flags(NF_Mask | ZF_Mask | CF_Mask, 8, res, Y());
            break;
        }
        case CPY_ZeroPage: {
            uint8_t zp = fetchByte(ram);
            uint8_t v = readByte(ram, DS, zp);
            auto res = Y() - v;
            set_flags(NF_Mask | ZF_Mask | CF_Mask, 8, res, Y());
            break;
        }
        case CPY_Absolute: {
            uint16_t adr = fetchByte(ram) | fetchByte(ram) << 8;
            uint8_t v = readByte(ram, DS, adr);
            auto res = Y() - v;
            set_flags(NF_Mask | ZF_Mask | CF_Mask, 8, res, Y());
            break;
        }

        case DEC_ZeroPage: {
            auto zp = fetchByte(ram);
            auto v = readByte(ram, DS, zp);
            auto res = v - 1;
            cycle();
            writeByte(ram, DS, zp, res);
            set_flags(NF_Mask | ZF_Mask, 8, res, 0);
            break;            
        }
        case DEC_ZeroPageX: {
            auto zp = fetchByte(ram);
            zp = (zp + X()) & 0xff;
            cycle();
            auto v = readByte(ram, DS, zp);
            auto res = v - 1;
            cycle();
            writeByte(ram, DS, zp, res);
            set_flags(NF_Mask | ZF_Mask, 8, res, 0);
            break; 
        }
        case DEC_Absolute: {
            uint16_t adr = fetchByte(ram) | fetchByte(ram) << 8;
            uint8_t v = readByte(ram, DS, adr);
            auto res = v - 1;
            cycle();
            writeByte(ram, DS, adr, res);
            set_flags(NF_Mask | ZF_Mask, 8, res, 0);
            break;
        }
        case DEC_AbsoluteX: {
            uint16_t adr = fetchByte(ram) | fetchByte(ram) << 8;
            adr += X();
            cycle();
            uint8_t v = readByte(ram, DS, adr);
            auto res = v - 1;
            cycle();
            writeByte(ram, DS, adr, res);
            set_flags(NF_Mask | ZF_Mask, 8, res, 0);
            break;
        }
        case DEX: {
            auto res = X() - 1;
            cycle();
            setX(res);
            set_flags(NF_Mask | ZF_Mask, 8, res, 0);
            break;
        }
        case DEY: {
            auto res = Y() - 1;
            cycle();
            setY(res);
            set_flags(NF_Mask | ZF_Mask, 8, res, 0);
            break;
        }

        case EOR_Immediate: {
            auto imm = fetchByte(ram);
            auto v = A() ^ imm;
            setA(v);
            set_flags(NF_Mask | ZF_Mask, 8, v, 0);
            break;
        }
        case EOR_ZeroPage:
        case EOR_ZeroPageX: {
            auto zp = (uint8_t)(fetchByte(ram) + (opcode == ADC_ZeroPageX ? X() : 0));
            auto v = readByte(ram, DS, zp);
            auto v2 = A() ^ v;
            setA(v2);
            set_flags(NF_Mask | ZF_Mask, 8, v2, 0);
            break;
        }
        case EOR_Absolute:
        case EOR_AbsoluteX:
        case EOR_AbsoluteY: {
            uint16_t adr = fetchByte(ram) | fetchByte(ram) << 8;
            adr += opcode == ADC_AbsoluteX ? X() : opcode == ADC_AbsoluteY ? Y() : 0;
            auto v = readByte(ram, DS, adr);
            auto v2 = A() ^ v;
            setA(v2);
            set_flags(NF_Mask | ZF_Mask, 8, v2, 0);
            break;
        }
        case EOR_IndirectX: {
            uint8_t zp = fetchByte(ram) + X();
            cycle();
            uint16_t adr = readByte(ram, DS, zp) | readByte(ram, DS, zp+1) << 8;
            auto v = readByte(ram, DS, adr);
            auto v2 = A() ^ v;
            setA(v2);
            set_flags(NF_Mask | ZF_Mask, 8, v2, 0);
            break;
        }
        case EOR_IndirectY: {
            uint8_t zp = fetchByte(ram);
            uint16_t adr = readByte(ram, DS, zp) | readByte(ram, DS, zp+1) << 8;
            auto v = readByte(ram, DS, adr + Y());
            auto v2 = A() ^ v;
            setA(v2);
            set_flags(NF_Mask | ZF_Mask, 8, v2, 0);
            break;
        }

        case INC_ZeroPage: {
            auto zp = fetchByte(ram);
            auto v = readByte(ram, DS, zp);
            auto res = v + 1;
            cycle();
            writeByte(ram, DS, zp, res);
            set_flags(NF_Mask | ZF_Mask, 8, res, 0);
            break;            
        }
        case INC_ZeroPageX: {
            auto zp = fetchByte(ram);
            zp = (zp + X()) & 0xff;
            cycle();
            auto v = readByte(ram, DS, zp);
            auto res = v + 1;
            cycle();
            writeByte(ram, DS, zp, res);
            set_flags(NF_Mask | ZF_Mask, 8, res, 0);
            break; 
        }
        case INC_Absolute: {
            uint16_t adr = fetchByte(ram) | fetchByte(ram) << 8;
            uint8_t v = readByte(ram, DS, adr);
            auto res = v + 1;
            cycle();
            writeByte(ram, DS, adr, res);
            set_flags(NF_Mask | ZF_Mask, 8, res, 0);
            break;
        }
        case INC_AbsoluteX: {
            uint16_t adr = fetchByte(ram) | fetchByte(ram) << 8;
            adr += X();
            cycle();
            uint8_t v = readByte(ram, DS, adr);
            auto res = v + 1;
            cycle();
            writeByte(ram, DS, adr, res);
            set_flags(NF_Mask | ZF_Mask, 8, res, 0);
            break;
        }
        case INX: {
            auto res = X() + 1;
            cycle();
            setX(res);
            set_flags(NF_Mask | ZF_Mask, 8, res, 0);
            break;
        }
        case INY: {
            auto res = Y() + 1;
            cycle();
            setY(res);
            set_flags(NF_Mask | ZF_Mask, 8, res, 0);
            break;
        }

        case JMP_Absolute: {
            auto lo = fetchByte(ram);
            PC = fetchByte(ram) << 8 | lo;
            break;
        }
        case JMP_Indirect: {
            auto lo = fetchByte(ram);
            uint16_t addr = fetchByte(ram) << 8 | lo;
            auto pclo = readByte(ram, PS, addr);
            PC = readByte(ram, PS, addr+1); 
            break;
        }

        case JSR_Absolute: {
            auto adr = fetchByte(ram) | fetchByte(ram) << 8;
            pushByte(ram, adr >> 8);
            pushByte(ram, adr & 0xff);
            cycle();
            PC = adr;
            break;
        }

        case LDA_Immediate: {
            auto imm = fetchByte(ram);
            setA(imm);
            set_flags(NF_Mask | ZF_Mask, 8, imm, 0);
            break;
        }
        case LDA_ZeroPage:
        case LDA_ZeroPageX: {
            auto zp = (uint8_t)(fetchByte(ram) + (opcode == LDA_ZeroPageX ? X() : 0));
            auto v = readByte(ram, DS, zp);
            setA(v);
            set_flags(NF_Mask | ZF_Mask, 8, v, 0);
            break;
        }
        case LDA_Absolute:
        case LDA_AbsoluteX:
        case LDA_AbsoluteY: {
            uint16_t adr = fetchByte(ram) | fetchByte(ram) << 8;
            adr += opcode == LDA_AbsoluteX ? X() : opcode == LDA_AbsoluteY ? Y() : 0;
            auto v = readByte(ram, DS, adr);
            setA(v);
            set_flags(NF_Mask | ZF_Mask, 8, v, 0);
            break;
        }
        case LDA_IndirectX: {
            uint8_t zp = fetchByte(ram) + X();
            cycle();
            uint16_t adr = readByte(ram, DS, zp) | readByte(ram, DS, zp+1) << 8;
            auto v = readByte(ram, DS, adr);
            setA(v);
            set_flags(NF_Mask | ZF_Mask, 8, v, 0);
            break;
        }
        case LDA_IndirectY: {
            uint8_t zp = fetchByte(ram);
            uint16_t adr = readByte(ram, DS, zp) | readByte(ram, DS, zp+1) << 8;
            auto v = readByte(ram, DS, adr + Y());
            setA(v);
            set_flags(NF_Mask | ZF_Mask, 8, v, 0);
            break;
        }

        case LDX_Immediate: {
            auto imm = fetchByte(ram);
            setX(imm);
            set_flags(NF_Mask | ZF_Mask, 8, imm, 0);
            break;
        }
        case LDX_ZeroPage:
        case LDX_ZeroPageY: {
            auto zp = (uint8_t)(fetchByte(ram) + (opcode == LDX_ZeroPageY ? Y() : 0));
            auto v = readByte(ram, DS, zp);
            setX(v);
            set_flags(NF_Mask | ZF_Mask, 8, v, 0);
            break;
        }
        case LDX_Absolute:
        case LDX_AbsoluteY: {
            uint16_t adr = fetchByte(ram) | fetchByte(ram) << 8;
            adr += opcode == LDX_AbsoluteY ? Y() : 0;
            auto v = readByte(ram, DS, adr);
            setX(v);
            set_flags(NF_Mask | ZF_Mask, 8, v, 0);
            break;
        }

         case LDY_Immediate: {
            auto imm = fetchByte(ram);
            setY(imm);
            set_flags(NF_Mask | ZF_Mask, 8, imm, 0);
            break;
        }
        case LDY_ZeroPage:
        case LDY_ZeroPageX: {
            auto zp = (uint8_t)(fetchByte(ram) + (opcode == LDY_ZeroPageX ? X() : 0));
            auto v = readByte(ram, DS, zp);
            setY(v);
            set_flags(NF_Mask | ZF_Mask, 8, v, 0);
            break;
        }
        case LDY_Absolute:
        case LDY_AbsoluteX: {
            uint16_t adr = fetchByte(ram) | fetchByte(ram) << 8;
            adr += opcode == LDY_AbsoluteX ? X() : 0;
            auto v = readByte(ram, DS, adr);
            setY(v);
            set_flags(NF_Mask | ZF_Mask, 8, v, 0);
            break;
        }

        case LSR_Implied: {
            cycle();
            P.CF = A() & 0x01;
            auto v = (A() >> 1);
            P.NF = 0;
            P.ZF = A() == 0 ? 1 : 0;
            break;
        }
        case LSR_ZeroPage: {
            auto zp = fetchByte(ram);
            auto res = readByte(ram, DS, zp);
            P.CF = res & 0x01;
            res = (res >> 1);
            cycle();
            writeByte(ram, DS, zp, res);
            P.NF = 0;
            P.ZF = res == 0 ? 1 : 0;            
            P.NF = 0;
            P.ZF = res == 0 ? 1 : 0;
            break;
        }
        case LSR_ZeroPageX: {
            auto zp = fetchByte(ram);
            zp = (zp + X()) & 0xff;
            cycle();
            auto res = readByte(ram, DS, zp);
            P.CF = res & 0x01;
            res = (res >> 1);
            cycle();
            writeByte(ram, DS, zp, res);
            P.NF = 0;
            P.ZF = res == 0 ? 1 : 0;
            break;
        }
        case LSR_Absolute: {
            uint16_t adr = fetchByte(ram) | fetchByte(ram) << 8;
            auto res = readByte(ram, DS, adr);
            P.CF = res & 0x01;
            res = (res >> 1);
            cycle();
            writeByte(ram, DS, adr, res);
            P.NF = 0;
            P.ZF = res == 0 ? 1 : 0;
            break;
        }
        case LSR_AbsoluteX: {
            uint16_t adr = fetchByte(ram) | fetchByte(ram) << 8;
            adr = (adr + X()) & 0xffff;
            cycle();
            auto res = readByte(ram, DS, adr);
            P.CF = res & 0x01;
            res = (res >> 1);
            cycle();
            writeByte(ram, DS, adr, res);
            P.NF = 0;
            P.ZF = res == 0 ? 1 : 0;
            break;
        }

        case NOP: {
            cycle();
            break;
        }

        case ORA_Immediate: {
            auto imm = fetchByte(ram);
            auto v = A() | imm;
            setA(v);
            set_flags(NF_Mask | ZF_Mask, 8, v, 0);
            break;
        }
        case ORA_ZeroPage:
        case ORA_ZeroPageX: {
            auto zp = (uint8_t)(fetchByte(ram) + (opcode == AND_ZeroPageX ? X() : 0));
            auto v = readByte(ram, DS, zp);
            auto v2 = A() | v;
            setA(v2);
            set_flags(NF_Mask | ZF_Mask, 8, v2, 0);
            break;
        }
        case ORA_Absolute:
        case ORA_AbsoluteX:
        case ORA_AbsoluteY: {
            uint16_t adr = fetchByte(ram) | fetchByte(ram) << 8;
            adr += opcode == AND_AbsoluteX ? X() : opcode == AND_AbsoluteY ? Y() : 0;
            auto v = readByte(ram, DS, adr);
            auto v2 = A() | v;
            setA(v2);
            set_flags(NF_Mask | ZF_Mask, 8, v2, 0);
            break;
        }
        case ORA_IndirectX: {
            uint8_t zp = fetchByte(ram) + X();
            cycle();
            uint16_t adr = readByte(ram, DS, zp) | readByte(ram, DS, zp+1) << 8;
            auto v = readByte(ram, DS, adr);
            auto v2 = A() | v;
            setA(v2);
            set_flags(NF_Mask | ZF_Mask, 8, v2, 0);
            break;
        }
        case ORA_IndirectY: {
            uint8_t zp = fetchByte(ram);
            uint16_t adr = readByte(ram, DS, zp) | readByte(ram, DS, zp+1) << 8;
            auto v = readByte(ram, DS, adr + Y());
            auto v2 = A() | v;
            setA(v2);
            set_flags(NF_Mask | ZF_Mask, 8, v2, 0);
            break;
        }

        case PHA: {
            pushByte(ram, A());
            cycle();
            break;
        }
        case PHP: {
            auto v = P.asByte();
            pushByte(ram, v);
            cycle();
            break;
        }
        case PLA: {
            auto v = popByte(ram);
            cycle();
            setA(v);
            set_flags(NF_Mask | ZF_Mask, 8, v, 0);
            break;
        }
        case PLP: {
            auto v = popByte(ram);
            cycle();
            P.setByte(v);
            break;
        }

        case ROL_Implied: {
            cycle();
            auto cf = P.CF;
            P.CF = A() & 0x80;
            auto v = (A() << 1) | cf;
            setA(v);
            set_flags(NF_Mask | ZF_Mask, 8, v, 0);
            break;
        }
        case ROL_ZeroPage: {
            auto zp = fetchByte(ram);
            auto res = readByte(ram, DS, zp);
            auto cf = P.CF;
            P.CF = res & 0x80;
            res = (res << 1) | cf;
            cycle();
            writeByte(ram, DS, zp, res);
            set_flags(NF_Mask | ZF_Mask, 8, res, 0);
            break;
        }
        case ROL_ZeroPageX: {
            auto zp = fetchByte(ram);
            zp = (zp + X()) & 0xff;
            cycle();
            auto res = readByte(ram, DS, zp);
            auto cf = P.CF;
            P.CF = res & 0x80;
            res = (res << 1) | cf;
            cycle();
            writeByte(ram, DS, zp, res);
            set_flags(NF_Mask | ZF_Mask, 8, res, 0);
            break;
        }
        case ROL_Absolute: {
            uint16_t adr = fetchByte(ram) | fetchByte(ram) << 8;
            auto res = readByte(ram, DS, adr);
            auto cf = P.CF;
            P.CF = res & 0x80;
            res = (res << 1) | cf;
            cycle();
            writeByte(ram, DS, adr, res);
            set_flags(NF_Mask | ZF_Mask, 8, res, 0);
            break;
        }
        case ROL_AbsoluteX: {
            uint16_t adr = fetchByte(ram) | fetchByte(ram) << 8;
            adr = (adr + X()) & 0xffff;
            cycle();
            auto res = readByte(ram, DS, adr);
            auto cf = P.CF;
            P.CF = res & 0x80;
            res = (res << 1) | cf;
            cycle();
            writeByte(ram, DS, adr, res);
            set_flags(NF_Mask | ZF_Mask, 8, res, 0);
            break;
        }

        case ROR_Implied: {
            cycle();
            auto cf = P.CF;
            P.CF = A() & 0x01;
            auto v = (A() >> 1) | (cf ? 0x80 : 0);
            setA(v);
            set_flags(NF_Mask | ZF_Mask, 8, v, 0);
            break;
        }
        case ROR_ZeroPage: {
            auto zp = fetchByte(ram);
            auto res = readByte(ram, DS, zp);
            auto cf = P.CF;
            P.CF = res & 0x01;
            res = (res >> 1) | (cf ? 0x80 : 0);
            cycle();
            writeByte(ram, DS, zp, res);
            set_flags(NF_Mask | ZF_Mask, 8, res, 0);
            break;
        }
        case ROR_ZeroPageX: {
            auto zp = fetchByte(ram);
            zp = (zp + X()) & 0xff;
            cycle();
            auto res = readByte(ram, DS, zp);
            auto cf = P.CF;
            P.CF = res & 0x01;
            res = (res >> 1) | (cf ? 0x80 : 0);
            cycle();
            writeByte(ram, DS, zp, res);
            set_flags(NF_Mask | ZF_Mask, 8, res, 0);
            break;
        }
        case ROR_Absolute: {
            uint16_t adr = fetchByte(ram) | fetchByte(ram) << 8;
            auto res = readByte(ram, DS, adr);
            auto cf = P.CF;
            P.CF = res & 0x01;
            res = (res >> 1) | (cf ? 0x80 : 0);
            cycle();
            writeByte(ram, DS, adr, res);
            set_flags(NF_Mask | ZF_Mask, 8, res, 0);
            break;
        }
        case ROR_AbsoluteX: {
            uint16_t adr = fetchByte(ram) | fetchByte(ram) << 8;
            adr = (adr + X()) & 0xffff;
            cycle();
            auto res = readByte(ram, DS, adr);
            auto cf = P.CF;
            P.CF = res & 0x01;
            res = (res >> 1) | (cf ? 0x80 : 0);
            cycle();
            writeByte(ram, DS, adr, res);
            set_flags(NF_Mask | ZF_Mask, 8, res, 0);
            break;
        }

        case RTI: {
            auto p = popByte(ram);
            uint16_t pc = popByte(ram) | popByte(ram) << 8;
            P.setByte(p);
            PC = pc;
            cycle();
            cycle();
            break;
        }

        case RTS: {
            uint16_t pc = popByte(ram) | popByte(ram) << 8;
            PC = pc;
            cycle();
            cycle();
            break;
        }

        case SBC_Immediate: {
            auto imm = fetchByte(ram);
            auto a = A();
            auto v = a - imm - ~P.CF;
            setA(v);
            set_flags(NF_Mask | ZF_Mask | CF_Mask | OF_Mask, 8, v, a);
            break;
        }
        case SBC_ZeroPage:
        case SBC_ZeroPageX: {
            auto zp = (uint8_t)(fetchByte(ram) + (opcode == SBC_ZeroPageX ? X() : 0));
            auto v = readByte(ram, DS, zp);
            auto a = A();
            auto v2 = a - v - ~P.CF;
            setA(v2);
            set_flags(NF_Mask | ZF_Mask | CF_Mask | OF_Mask, 8, v2, a);
            break;
        }
        case SBC_Absolute:
        case SBC_AbsoluteX:
        case SBC_AbsoluteY: {
            uint16_t adr = fetchByte(ram) | fetchByte(ram) << 8;
            adr += opcode == SBC_AbsoluteX ? X() : opcode == SBC_AbsoluteY ? Y() : 0;
            auto v = readByte(ram, DS, adr);
            auto a = A();
            auto v2 = a - v - ~P.CF;
            setA(v2);
            set_flags(NF_Mask | ZF_Mask | CF_Mask | OF_Mask, 8, v2, a);
            break;
        }
        case SBC_IndirectX: {
            uint8_t zp = fetchByte(ram) + X();
            cycle();
            uint16_t adr = readByte(ram, DS, zp) | readByte(ram, DS, zp+1) << 8;
            auto v = readByte(ram, DS, adr);
            auto a = A();            
            auto v2 = a - v - ~P.CF;
            set_flags(NF_Mask | ZF_Mask | CF_Mask | OF_Mask, 8, v2, a);
            break;
        }
        case SBC_IndirectY: {
            uint8_t zp = fetchByte(ram);
            uint16_t adr = readByte(ram, DS, zp) | readByte(ram, DS, zp+1) << 8;
            auto v = readByte(ram, DS, adr + Y());
            auto a = A();
            auto v2 = a - v - ~P.CF;
            setA(v2);
            set_flags(NF_Mask | ZF_Mask | CF_Mask | OF_Mask, 8, v2, a);
            break;
        }

        case SEC: {
            P.CF = 1;
            cycle();
            break;
        }

        case SED: {
            P.DF = 1;
            cycle();
            break;
        }

        case SEI: {
            P.IF = 1;
            cycle();
            break;
        }

       case STA_ZeroPage: {
            auto zp = fetchByte(ram);
            writeByte(ram, DS, zp, A());
            break;
        }
        case STA_ZeroPageX: {
            uint8_t zp = fetchByte(ram);
            zp += X();
            cycle();
            writeByte(ram, DS, zp, A());
            break;
        }
        case STA_Absolute: {
            uint16_t adr = fetchByte(ram) | fetchByte(ram) << 8;
            writeByte(ram, DS, adr, A());
            break;
        }
        case STA_AbsoluteX: {
            uint16_t adr = fetchByte(ram) | fetchByte(ram) << 8;
            adr += X();
            cycle();
            writeByte(ram, DS, adr, A());
            break;
        }
        case STA_AbsoluteY: {
            uint16_t adr = fetchByte(ram) | fetchByte(ram) << 8;
            adr += Y();
            cycle();
            writeByte(ram, DS, adr, A());
            break;
        }
        case STA_IndirectX: {
            uint8_t zp = fetchByte(ram);
            zp += X();
            cycle();
            uint16_t adr = readByte(ram, DS, zp) | readByte(ram, DS, zp + 1);
            writeByte(ram, DS, adr, A());
            break;
        }
        case STA_IndirectY: {
            uint8_t zp = fetchByte(ram);
            uint16_t adr = readByte(ram, DS, zp) | readByte(ram, DS, zp + 1);
            adr += X();
            cycle();
            writeByte(ram, DS, adr, A());
            break;
        }

        case STX_ZeroPage: {
            auto zp = fetchByte(ram);
            writeByte(ram, DS, zp, X());
            break;
        }
        case STX_ZeroPageY: {
            auto zp = fetchByte(ram);
            zp += Y();
            cycle();
            writeByte(ram, DS, zp, X());
            break;
        }
        case STX_Absolute: {
            uint16_t adr = fetchByte(ram) | fetchByte(ram) << 8;
            writeByte(ram, DS, adr, X());
            break;
        }

        case STY_ZeroPage: {
            auto zp = fetchByte(ram);
            writeByte(ram, DS, zp, Y());
            break;
        }
        case STY_ZeroPageX: {
            auto zp = fetchByte(ram);
            zp += X();
            cycle();
            writeByte(ram, DS, zp, Y());
            break;
        }
        case STY_Absolute: {
            uint16_t adr = fetchByte(ram) | fetchByte(ram) << 8;
            writeByte(ram, DS, adr, Y());
            break;
        }

        case TAX: {
            cycle();
            setX(A());
            break;
        }
        case TAY: {
            cycle();
            setY(A());
            break;
        }
        case TSX: {
            cycle();
            setX(SP & (0xff));
            break;
        }
        case TXA: {
            cycle();
            setA(X());
            break;
        }
        case TXS: {
            cycle();
            SP = 0x100 | X();
            break;
        }
        case TYA: {
            cycle();
            setA(Y());
            break;
        }

        case BRA: {
            CHECK_CPU_MODE(allow65c02, opcode);
            auto rel8 = fetchByte(ram);
            branch_relative8_if(rel8, true);
            break;
        }

        case STZ_ZeroPage: {
            CHECK_CPU_MODE(allow65c02, opcode);
            auto zp = fetchByte(ram);
            writeByte(ram, DS, zp, 0);
            break;
        }
        case STZ_ZeroPageX: {
            CHECK_CPU_MODE(allow65c02, opcode);
            auto zp = fetchByte(ram);
            zp += X();
            cycle();
            writeByte(ram, DS, zp, 0);
            break;
        }
        case STZ_Absolute: {
            CHECK_CPU_MODE(allow65c02, opcode);
            auto adr = fetchWord(ram);
            writeByte(ram, DS, adr, 0);
            break;
        }
        case STZ_AbsoluteX: {
            CHECK_CPU_MODE(allow65c02, opcode);
            auto adr = fetchWord(ram);
            adr += X();
            cycle();
            writeByte(ram, DS, adr, 0);
            break;
        }

        case XTOP1: {
            CHECK_CPU_MODE(allow65x02, opcode);
            xtop1_decodeAndExecute(ram, *this);
            break;
        }

        case XTOP2: {
            CHECK_CPU_MODE(allow65x02, opcode);
            xtop2_decodeAndExecute(ram, *this);
            break;
        }

        case XTOP3: {
            CHECK_CPU_MODE(allow65x02, opcode);
            xtop3_decodeAndExecute(ram, *this);
            break;
        }

        case XTOP1_TRX: {
            CHECK_CPU_MODE(allow65x02, opcode);
            xtop1_trx_decodeAndExecute(ram, *this);
            break;
        }

        case XTOP1_MATH: {
            CHECK_CPU_MODE(allow65x02, opcode);
            xtop1_math_decodeAndExecute(ram, *this);
            break;
        }

        case XTOP2_MATH: {
            CHECK_CPU_MODE(allow65x02, opcode);
            xtop2_math_decodeAndExecute(ram, *this);
            break;
        }

        case XTOP3_MATH: {
            CHECK_CPU_MODE(allow65x02, opcode);
            xtop3_math_decodeAndExecute(ram, *this);
            break;
        }

        case XTOP1_STOR: {
            CHECK_CPU_MODE(allow65x02, opcode);
            xtop1_stor_decodeAndExecute(ram, *this);
            break;
        }

        default: {
            illegalInstruction(opcode);
            break; 
        }
    }
}
/*
    little endianness and register value mapping
    | d0 | d1 | d2 | d3 |
    | EF | BE | AD | DE |
    |   wj    |  wj+1   | wj= 0xbeef, wj+1=0xdead
    |  EF BE  |  AD DE  |
    |        xk         |
    | EF   BE   AD   DE | xk = 0xdeadbeef
*/

uint8_t CPU::register8(uint8_t sel) const {
    uint8_t sel32 = sel / 4;
    switch(sel % 4) {
        case 3: return reg32[sel32] >> 24;
        case 2: return (reg32[sel32] >> 16) & 0xff;
        case 1: return (reg32[sel32] >> 8) & 0xff;
        default: return (reg32[sel32]) & 0xff;
    }
}

uint16_t CPU::register16(uint8_t sel) const {
    uint8_t sel32 = sel / 2;
    switch(sel % 2) {        
        case 0: return reg32[sel32] &0xffff;
        default: return reg32[sel32] >> 16;
    }
}

uint32_t CPU::register32(uint8_t sel) const {
    return reg32[sel];
}

void CPU::set_register8(uint8_t sel, uint8_t val) {
    uint8_t sel32 = sel / 4;
    switch(sel % 4) {
        case 3: { reg32[sel32] = (reg32[sel32] & 0x00ffffff) | (val << 24); break; }
        case 2: { reg32[sel32] = (reg32[sel32] & 0xff00ffff) | (val << 16); break; }
        case 1: { reg32[sel32] = (reg32[sel32] & 0xffff00ff) | (val << 8); break; }
        default: { reg32[sel32] = (reg32[sel32] & 0xffffff00) | val; break; }
    }
}

void CPU::set_register16(uint8_t sel, uint16_t val) {
    uint8_t sel32 = sel / 2;
    switch(sel % 2) {
        case 1: { reg32[sel32] = (reg32[sel32] & 0x0000ffff) | (val<<16); break; }
        default: { reg32[sel32] = (reg32[sel32] & 0xffff0000) | (val); break; }
    }
}

void CPU::set_register32(uint8_t sel, uint32_t val) {
    reg32[sel] = val;
}

uint8_t CPU::registerPS() const {
    return PS;
}

void CPU::set_registerPS(uint8_t val) {
    PS = val;
}

uint8_t CPU::registerDS() const {
    return DS;
}

void CPU::set_registerDS(uint8_t val) {
    DS = val;
}

uint8_t CPU::registerSS() const {
    return SS;
}

void CPU::set_registerSS(uint8_t val) {
    SS = val;
}

void CPU::set_flags(uint8_t flags, size_t size, unsigned v1, unsigned v2) {
    if(flags & ZF_Mask) {
        P.ZF = (v1 == 0) ? 1 : 0;
    }
    if(flags & NF_Mask) {
        unsigned nmask = mask(size-1, size-1);
        P.NF = (v1 & nmask) ? 1 : 0;
    }
    if(flags & CF_Mask) {
        P.CF = (v1 < v2) ? 1 : 0;
    }
    if(flags & OF_Mask) {
        P.OF = 0; // TODO
    }
}

void CPU::branch_relative8_if(int8_t rel, bool cond) {
    if(cond) {
        auto newPC = (uint16_t)(PC + rel);
        if((newPC & 0xff00) != (PC & 0xff00)) {
            cycle();
        }
        PC = newPC;
    }
}

void CPU::pushByte(Memory& ram, uint8_t byte) {
    writeByte(ram, SS, SP, byte);
    SP = ((SP - 1) & 0xff) + 0x100; // TODO: fix this
}

uint8_t CPU::popByte(Memory& ram) {
    auto byte = readByte(ram, SS, SP);
    SP = ((SP + 1) & 0xff) + 0x100; // TODO: fix this
    return byte;
}

uint8_t CPU::readByte(Memory& ram, uint8_t seg, uint16_t addr) {
    auto data = ram.read(seg, addr);
    if (tracing) {
        std::cout << format("  read %02X:%04X=%02X", seg, addr, data) << std::endl;
    }
    cycle();
    return data;
}

uint16_t CPU::readWord(Memory& ram, uint8_t seg, uint16_t addr) {
    uint16_t data = readByte(ram, seg, addr);
    data |= (readByte(ram, seg, addr+1) << 8);
    if (tracing) std::cout << format("  read %02X:%04X=%04X\n", seg, addr, data);
    return data;
}

uint32_t CPU:: readLongWord(Memory& ram, uint8_t seg, uint16_t addr) {
    uint32_t data = readByte(ram, seg, addr);
    data |= (readByte(ram, seg, addr+1) << 8);
    data |= (readByte(ram, seg, addr+2) << 16);
    data |= (readByte(ram, seg, addr+3) << 24);
    if (tracing) std::cout << format("  read %02X:%04X=%08X\n", seg, addr, data);
    return data;
}

uint8_t CPU::fetchByte(Memory& ram) {
    return readByte(ram, PS, PC++);
}

uint16_t CPU::fetchWord(Memory& ram) {
    auto word = readWord(ram, PS, PC);
    PC += 2;
    return word;
}

uint32_t CPU::fetchLongWord(Memory& ram) {
    auto lword = readLongWord(ram, PS, PC);
    PC += 4;
    return lword;
}

void CPU::writeByte(Memory& ram, uint8_t seg, uint16_t addr, uint8_t byte) {
    ram.write(seg, addr, byte);
    if (tracing) std::cout << format("  wrote %02X:%04X=%02X\n", seg, addr, byte);
    cycle();
}

void CPU::writeWord(Memory& ram, uint8_t seg, uint16_t addr, uint16_t word) {
    writeByte(ram, seg, addr, word & 0xff);
    writeByte(ram, seg, addr+1, word >> 8);
}

void CPU::writeLongWord(Memory& ram, uint8_t seg, uint16_t addr, uint32_t word) {
    writeByte(ram, seg, addr, word & 0xff);
    writeByte(ram, seg, addr+1, (word >> 8) & 0xff);
    writeByte(ram, seg, addr+2, (word >> 16) & 0xff);
    writeByte(ram, seg, addr+3, (word >> 24) & 0xff);
}

void CPU::cycle() {
    cycles += 1;
}
