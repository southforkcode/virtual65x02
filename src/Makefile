LBUILD=$(BUILD)/vm
TARGET?=$(LBUILD)/vm

SRCS=$(wildcard *.cc)
HEADERS=$(addprefix $(LBUILD)/,$(wildcard *.h))
OBJS=$(addprefix $(LBUILD)/,$(addsuffix .o,$(SRCS)))

all: vm

vm: build_dir headers vm_bin

headers: $(HEADERS)

vm_bin: $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) -o $(TARGET)

$(LBUILD)/%.cc.o: %.cc
	$(CC) $(CXXFLAGS) -c $< -o $@

$(LBUILD)/%.h: %.h
	cp $< $@

build_dir:
	mkdir -p $(LBUILD)

clean:
	rm -rf $(LBUILD)

.PHONY: vm build_dir clean
