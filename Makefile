CC=clang
CXX=clang++
LD=clang

export CC
export CXX
export LD

CXXFLAGS=-std=c++20 -g -I$(abspath src) -I/opt/homebrew/include
export CXXFLAGS

LDFLAGS=-L/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/lib -lstdc++ -v
export LDFLAGS

BUILD=$(abspath build)
export BUILD

all: tests vm asm65

vm:
	cd src && $(MAKE)

asm65:
	cd tools/asm65 && $(MAKE)

tests: vm
	cd test && $(MAKE) all

build_dir:
	mkdir -p $(BUILD)

clean:
	rm -rf $(BUILD)

.PHONY: all vm asm65 test build_dir
