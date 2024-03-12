#ifndef __XTOP3IMP_H
#define __XTOP3IMP_H

#include "memory.h"
#include "cpu65x.h"

void xtop3_decodeAndExecute(Memory& ram, CPU& cpu);
void xtop3_math_decodeAndExecute(Memory& ram, CPU& cpu);
void xtop3_regind_decodeAndExecute(Memory& ram, CPU& cpu);

#endif 