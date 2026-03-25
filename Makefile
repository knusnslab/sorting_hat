# ===== directories =====
BUILD_DIR      := build
BIN_DIR        := $(BUILD_DIR)/bin
APPLE_BIN_DIR  := $(BIN_DIR)/apple_arm
RISCV_BIN_DIR  := $(BIN_DIR)/riscv

# ===== sources =====
APPLE_CLASSIFIER_SRC := src/apple_arm/classifier.c
RISCV_CLASSIFIER_SRC := src/riscv/classifier.c
RISCV_INIT_SRC       := src/riscv/init.S
VERIFY_SRC           := test/verify.cpp
ACCUMET_SRC          := test/accuMet.cpp

# ===== compilers =====
CC  ?= gcc
CXX ?= g++

APPLE_CC      ?= /opt/homebrew/opt/llvm/bin/clang
APPLE_CXX     ?= /opt/homebrew/opt/llvm/bin/clang++
APPLE_OMP_CXX ?= /opt/homebrew/opt/llvm/bin/clang++

RISCV_CC      ?= gcc

# ===== llvm (apple only) =====
APPLE_LLVM_CONFIG ?= /opt/homebrew/opt/llvm/bin/llvm-config

APPLE_LLVM_CFLAGS  := $(shell $(APPLE_LLVM_CONFIG) --cflags 2>/dev/null)
APPLE_LLVM_LDFLAGS := $(shell $(APPLE_LLVM_CONFIG) --ldflags 2>/dev/null)
APPLE_LLVM_LIBS    := $(shell $(APPLE_LLVM_CONFIG) --libs 2>/dev/null)
APPLE_LLVM_SYSLIBS := $(shell $(APPLE_LLVM_CONFIG) --system-libs 2>/dev/null)
APPLE_LLVM_FLAGS   := $(APPLE_LLVM_CFLAGS) $(APPLE_LLVM_LDFLAGS) $(APPLE_LLVM_LIBS) $(APPLE_LLVM_SYSLIBS)

# ===== ISA-specific flags =====
APPLE_CFLAGS    ?= -O2 -w -g
APPLE_CXXFLAGS  ?= -O2 -g
RISCV_CFLAGS    ?= -O2 -w -g -gdwarf-4 -fomit-frame-pointer -mno-relax

APPLE_DEFS      := -DMacOS -DAPPLE_ARM
RISCV_DEFS      := -DRISCV

APPLE_RPATH     := -Wl,-rpath,/opt/homebrew/opt/llvm/lib
APPLE_OMP_FLAGS := -I/opt/homebrew/opt/llvm/include \
					-L/opt/homebrew/opt/llvm/lib \
					-Wl,-rpath,/opt/homebrew/opt/llvm/lib \
					-fopenmp

UNAME_S := $(shell uname -s)

ifeq ($(UNAME_S),Darwin)
POST_LINK_APPLE = install_name_tool -add_rpath /opt/homebrew/opt/llvm/lib $@
else
POST_LINK_APPLE = true
endif

# ===== phony =====
.PHONY: all apple riscv clean tree

all: apple riscv

apple: $(APPLE_BIN_DIR)/classifier $(APPLE_BIN_DIR)/verify $(APPLE_BIN_DIR)/accuMet
riscv: $(RISCV_BIN_DIR)/classifier

tree:
	@tree $(BUILD_DIR) 2>/dev/null || find $(BUILD_DIR) -print

# ===== directory rules =====
$(BUILD_DIR):
	mkdir -p $@

$(BIN_DIR): | $(BUILD_DIR)
	mkdir -p $@

$(APPLE_BIN_DIR): | $(BIN_DIR)
	mkdir -p $@

$(RISCV_BIN_DIR): | $(BIN_DIR)
	mkdir -p $@

# ===== apple_arm targets =====
$(APPLE_BIN_DIR)/classifier: $(APPLE_CLASSIFIER_SRC) | $(APPLE_BIN_DIR)
	$(APPLE_CC) $(APPLE_CFLAGS) $(APPLE_DEFS) -o $@ $^ $(APPLE_LLVM_FLAGS) $(APPLE_RPATH)
	$(POST_LINK_APPLE)

$(APPLE_BIN_DIR)/verify: $(VERIFY_SRC) | $(APPLE_BIN_DIR)
	$(APPLE_CXX) $(APPLE_CXXFLAGS) -o $@ $^ $(APPLE_LLVM_FLAGS) $(APPLE_RPATH)
	$(POST_LINK_APPLE)

$(APPLE_BIN_DIR)/accuMet: $(ACCUMET_SRC) | $(APPLE_BIN_DIR)
	$(APPLE_OMP_CXX) $(APPLE_CXXFLAGS) -o $@ $^ $(APPLE_OMP_FLAGS)

# ===== riscv target =====
$(RISCV_BIN_DIR)/classifier: $(RISCV_CLASSIFIER_SRC) $(RISCV_INIT_SRC) | $(RISCV_BIN_DIR)
	$(RISCV_CC) $(RISCV_CFLAGS) $(RISCV_DEFS) -o $@ $^

# ===== clean =====
clean:
	rm -rf $(BUILD_DIR)

