CXX ?= g++
CXXFLAGS ?= -O3 -Wall -Wextra -std=c++17
AVX512FLAGS ?= -mavx512f -mavx512vl
CPPFLAGS ?= -Iinclude

BUILD_DIR := build
AVXFISH_OBJ := $(BUILD_DIR)/avxfish.o
TEST_OBJ := $(BUILD_DIR)/test.o
BENCH_OBJ := $(BUILD_DIR)/bench.o
TEST_BIN := $(BUILD_DIR)/test
BENCH_BIN := $(BUILD_DIR)/bench

.PHONY: all test bench run-test run-bench clean

all: $(TEST_BIN) $(BENCH_BIN)

test: $(TEST_BIN)

bench: $(BENCH_BIN)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(AVXFISH_OBJ): src/avxfish.cpp include/avxfish.h | $(BUILD_DIR)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(AVX512FLAGS) -c $< -o $@

$(TEST_OBJ): src/test.cpp include/avxfish.h | $(BUILD_DIR)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(AVX512FLAGS) -c $< -o $@

$(BENCH_OBJ): src/bench.cpp include/avxfish.h | $(BUILD_DIR)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(AVX512FLAGS) -c $< -o $@

$(TEST_BIN): $(AVXFISH_OBJ) $(TEST_OBJ)
	$(CXX) $(CXXFLAGS) $(AVX512FLAGS) $^ -o $@

$(BENCH_BIN): $(AVXFISH_OBJ) $(BENCH_OBJ)
	$(CXX) $(CXXFLAGS) $(AVX512FLAGS) $^ -o $@

run-test: $(TEST_BIN)
	./$(TEST_BIN)

run-bench: $(BENCH_BIN)
	./$(BENCH_BIN)

clean:
	rm -rf $(BUILD_DIR)
