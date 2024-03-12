#include "memory.h"
#include "utils.h"

uint8_t Memory::read(uint8_t seg, uint16_t adr) {
    return segments[seg].memory[adr];
}

void Memory::write(uint8_t seg, uint16_t adr, uint8_t byte) {
    segments[seg].memory[adr] = byte;
}

uint16_t Memory::program(uint8_t seg, uint16_t adr, const std::vector<uint8_t>& bytes) {
    uint16_t a = adr;
    for(auto byte : bytes) {
        segments[seg].memory[a++] = byte;
    }
    return a;
}

void Memory::dump_memory(std::ostream& ostr, uint8_t seg, uint16_t adr, unsigned width, unsigned count) {
    for(int i=0; i<count; i++) {
        ostr << format("%02X:%04X : ", seg, adr);
        for(int j=0; j<width; j++) {
            if(j && j % 8 == 0) ostr << " ";
            ostr << format("%02X", segments[seg].memory[adr++]);
            if(j<width-1) ostr << " ";
        }
        ostr << std::endl;
    }
}

