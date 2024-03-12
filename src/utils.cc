#include "utils.h"

#include <vector>
#include <cassert>

std::string format(const char* format, ...) {
    va_list args;
    va_start(args, format);
    size_t len = std::vsnprintf(NULL, 0, format, args);
    va_end(args);
    std::vector<char> vec(len + 1);
    va_start(args, format);
    std::vsnprintf(&vec[0], len + 1, format, args);
    va_end(args);
    return &vec[0];    
}

uint32_t mask(size_t h, size_t l) {
    uint32_t m = ((1 << (h-l+1)) - 1) << l;
    return m;
}

uint8_t bitsv(uint8_t b, size_t h, size_t l)
 {
    assert((h>=l) && (h<8 && h>=0) && (l<8 && l>=0));
    uint8_t m = mask(h,l);
    return (b & m) >> l;
}

uint16_t bitsv(uint16_t w, size_t h, size_t l)
 {
    assert((h>=l) && (h<16 && h>=0) && (l<16 && l>=0));
    uint8_t mask = ((1 << (h-l+1)) - 1) << l;
    return (w & mask) >> l;
}

uint32_t bitsv(uint32_t lw, size_t h, size_t l)
 {
    assert((h>=l) && (h<32 && h>=0) && (l<32 && l>=0));
    uint8_t mask = ((1 << (h-l+1)) - 1) << l;
    return (lw & mask) >> l;
}

