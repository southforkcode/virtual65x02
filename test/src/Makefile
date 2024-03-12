LBUILD=$(BUILD)/test
LDFLAGS+=/opt/homebrew/lib/libCatch2.a /opt/homebrew/lib/libCatch2Main.a
SRCS=$(wildcard *.cc)
TEST_SRCS=$(filter-out main.cc,$(SRCS))
VM_OBJS=$(filter-out %/main.cc.o,$(wildcard $(BUILD)/vm/*.o))
TEST_OBJS=$(addprefix $(LBUILD)/,$(addsuffix .o,$(TEST_SRCS)))
TEST_EXES=$(addprefix $(LBUILD)/,$(addsuffix .test,$(TEST_SRCS)))

all: tests

tests: build_dir test_exes

test_exes: $(TEST_EXES)

%.cc.test: %.cc.o
	$(LD) $(LDFLAGS) -o $@ $< $(VM_OBJS)

$(LBUILD)/%.cc.o: %.cc
	$(CC) $(CXXFLAGS) -c $< -o $@

build_dir:
	mkdir -p $(LBUILD)

clean:
	rm -rf $(LBUILD)

.PHONY: vm testlib build_dir clean
