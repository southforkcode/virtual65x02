LBUILD=$(BUILD)/test
TEST_RUNNER=$(LBUILD)/test_runner
SRCS=$(wildcard src/*.cc)
EXES=$(patsubst src/%.cc,%.cc.test,$(SRCS))
TESTS=$(addprefix $(LBUILD)/,$(EXES))
HEADERS=$(abspath $(wildcard ../src/*.h))

all: headers tests run_tests

tests: build_dir 
	cd src && $(MAKE)

headers: $(HEADERS)
	echo $(HEADERS)

run_tests: test_runner $(TESTS)
	$(TEST_RUNNER) $(TESTS)

test_runner: test_runner.cc
	$(CC) $(CXXFLAGS) test_runner.cc -o $(LBUILD)/test_runner

$(LBUILD)/%.test:
	./$@

../src/%.h:
	cp $@ $(LBUILD)/$<

build_dir:
	mkdir -p $(LBUILD)

.PHONY: build_dir headers tests