#ifndef __MEMORY_H
#define __MEMORY_H

#include <stdint.h>
#include <stdlib.h>

#include <iostream>
#include <vector>

struct MemorySegment {
    static constexpr size_t SEGMENT_SIZE = 1024 * 64;
    uint8_t memory[SEGMENT_SIZE];
    void init() {
        for(auto i=0; i<SEGMENT_SIZE; i++) {
            memory[i] = 0;
        }
    }
};

struct Memory {
    static constexpr size_t NUM_SEGMENTS = 256;
    MemorySegment *segments = nullptr;
    
    void init() {
        if(!segments) {
            segments = new MemorySegment[NUM_SEGMENTS];
        }
        for(auto i=0; i<NUM_SEGMENTS; i++) {
            segments[i].init();
        }
    }

    ~Memory() {
        if(segments != nullptr) delete [] segments;
    }
    
    uint8_t read(uint8_t seg, uint16_t adr);
    void write(uint8_t seg, uint16_t adr, uint8_t byte);

    uint16_t program(uint8_t seg, uint16_t adr, const std::vector<uint8_t>& bytes);
    void dump_memory(std::ostream& ostr, uint8_t seg, uint16_t adr, unsigned width, unsigned count);
};



#endif