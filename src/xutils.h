#ifndef __XUTILS_H
#define __XUTILS_H

#include "cpu65x.h"

enum SegmentRegisterIndex : unsigned {
    DS = 0,
    PS = 1,
    SS = 2
};

uint8_t getSegmentByIndex(const CPU& cpu, SegmentRegisterIndex index);
uint8_t getImpliedIndex(uint8_t rs);

void x_dump_regs_info(std::ostream& ostr, Memory& ram, CPU& cpu);

#endif