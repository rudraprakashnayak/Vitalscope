CXX      ?= g++
CXXFLAGS ?= -std=c++17 -Wall -Wextra -O2 -Iinclude -pthread
BUILDDIR := build

SRCS := src/fs_util.cpp src/cpu_collector.cpp src/mem_collector.cpp \
        src/proc_collector.cpp src/dev_collector.cpp src/sampler.cpp src/dashboard.cpp
OBJS := $(SRCS:%.cpp=$(BUILDDIR)/%.o)

all: $(BUILDDIR)/Vitalscope

$(BUILDDIR)/Vitalscope: $(OBJS) $(BUILDDIR)/src/main.o
	@mkdir -p $(BUILDDIR)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(BUILDDIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(BUILDDIR)/test_collectors: $(OBJS) $(BUILDDIR)/tests/test_collectors.o
	@mkdir -p $(BUILDDIR)
	$(CXX) $(CXXFLAGS) -o $@ $^

test: $(BUILDDIR)/test_collectors
	./$(BUILDDIR)/test_collectors tests/fixtures
	@sh ./tests/run_integration.sh $(BUILDDIR)/Vitalscope tests/fixtures

run: $(BUILDDIR)/Vitalscope
	./$(BUILDDIR)/Vitalscope

clean:
	rm -rf $(BUILDDIR)

.PHONY: all test run clean
