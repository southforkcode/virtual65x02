#ifndef __UTILS_H
#define __UTILS_H

#include <string>

uint32_t mask(size_t h, size_t l);
uint8_t bitsv(uint8_t b, size_t h, size_t l);
uint16_t bitsv(uint16_t w, size_t h, size_t l);
uint32_t bitsv(uint32_t lw, size_t h, size_t l);

std::string format(const char* format, ...);

#endif