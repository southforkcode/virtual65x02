#ifndef __XTOP2IMP_H
#define __XTOP2IMP_H

#include "memory.h"
#include "cpu65x.h"

void xtop2_decodeAndExecute(Memory& ram, CPU& cpu);
void xtop2_math_decodeAndExecute(Memory& ram, CPU& cpu);

#endif