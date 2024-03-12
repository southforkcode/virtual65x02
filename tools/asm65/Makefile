CXX=clang++
LD=clang

CXXFLAGS=-std=c++20 -g
LDFLAGS=-L/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/lib -lstdc++ -v

SRCS=$(wildcard *.cc)
HEADERS=$(wildcard *.h)
OBJS=$(addsuffix .o,$(SRCS))
TARGET=asm65

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(LD) $(LDFLAGS) $(OBJS) -o $@

%.cc.o: %.cc $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

%.h:


clean:
	rm -rf *.o
	rm $(TARGET)
