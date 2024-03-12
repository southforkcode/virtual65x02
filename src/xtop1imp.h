#ifndef __XTOP1IMPL_H
#define __XTOP1IMPL_H

#include <iostream>

#include "memory.h"
#include "cpu65x.h"

void xtop1_decodeAndExecute(Memory& ram, CPU& cpu);
void xtop1_trx_decodeAndExecute(Memory& ram, CPU& cpu);
void xtop1_math_decodeAndExecute(Memory& ram, CPU& cpu);
void xtop1_stor_decodeAndExecute(Memory& ram, CPU& cpu);

#endif