CXX = g++
CXXSTD = -std=c++17
WARNINGS = -Wall -Wextra -Wpedantic
OPTIMIZE = -O3
DEBUG_FLAGS = -g -O0 -DDEBUG

# Default build type (can override with make BUILD_TYPE=debug)
BUILD_TYPE ?= release

ifeq ($(BUILD_TYPE),debug)
    CXXFLAGS = $(CXXSTD) $(WARNINGS) $(DEBUG_FLAGS)
    BUILD_DIR = build/debug
else
    CXXFLAGS = $(CXXSTD) $(WARNINGS) $(OPTIMIZE)
    BUILD_DIR = build/release
endif

# Source files and object files
CORE_SRCS = Order.cpp OrderModify.cpp Trade.cpp orderbook.cpp
CORE_OBJS = $(addprefix $(BUILD_DIR)/,$(CORE_SRCS:.cpp=.o))

# Main executable sources and objects
MAIN_SRC = main.cpp
MAIN_OBJ = $(BUILD_DIR)/main.o

# Test executable sources and objects
TEST_SRC = orderbook_test.cpp
TEST_OBJ = $(BUILD_DIR)/orderbook_test.o

# All dependencies
DEPS = $(CORE_OBJS:.o=.d) $(MAIN_OBJ:.o=.d) $(TEST_OBJ:.o=.d)

# Targets
LIB_TARGET = $(BUILD_DIR)/liborderbook.a
MAIN_TARGET = $(BUILD_DIR)/orderbook
TEST_TARGET = $(BUILD_DIR)/orderbook_test
MKDIR_P = mkdir -p

.PHONY: all clean debug release test lib main directories

# Default target
all: directories lib main test

# Library target
lib: directories $(LIB_TARGET)

# Main executable target
main: directories lib $(MAIN_TARGET)

# Test executable target
test: directories lib $(TEST_TARGET)

# Build with debug flags
debug:
	$(MAKE) BUILD_TYPE=debug

# Build with release flags
release:
	$(MAKE) BUILD_TYPE=release

# Create build directories
directories:
	$(MKDIR_P) $(BUILD_DIR)

# Build static library
$(LIB_TARGET): $(CORE_OBJS)
	ar rcs $@ $^

# Link the main executable
$(MAIN_TARGET): $(MAIN_OBJ) $(LIB_TARGET)
	$(CXX) $(CXXFLAGS) -o $@ $(MAIN_OBJ) -L$(BUILD_DIR) -lorderbook

# Link the test executable
$(TEST_TARGET): $(TEST_OBJ) $(LIB_TARGET)
	$(CXX) $(CXXFLAGS) -o $@ $(TEST_OBJ) -L$(BUILD_DIR) -lorderbook

# Compile main source
$(MAIN_OBJ): $(MAIN_SRC)
	$(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@

# Compile test source
$(TEST_OBJ): $(TEST_SRC)
	$(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@

# Compile and generate dependencies for core files
$(BUILD_DIR)/%.o: %.cpp
	$(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@

# Include generated dependencies
-include $(DEPS)

# Run the main program
run: $(MAIN_TARGET)
	./$(MAIN_TARGET)

# Run the test program
run-test: $(TEST_TARGET)
	./$(TEST_TARGET)

# Clean build artifacts
clean:
	rm -rf build
	rm -f *.o *.d orderbook orderbook_test

# Install to system (optional)
install: $(MAIN_TARGET)
	cp $(MAIN_TARGET) /usr/local/bin/orderbook

# Uninstall from system
uninstall:
	rm -f /usr/local/bin/orderbook