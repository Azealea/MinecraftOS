CC := ccache gcc

SRC_DIR := src
SRC := $(shell find $(SRC_DIR) -type f -name '*.c')

SHADER_DIR = src/shaders
SHADERS_SRC = $(SHADER_DIR)/shader.vert $(SHADER_DIR)/shader.frag
SHADERS_SPV := $(SHADERS_SRC:%=%.spv)
SHADERS_C := $(SHADERS_SPV:%=%.c)

SRC += $(SHADERS_C)
OBJ := $(SRC:.c=.o)
DEP := $(OBJ:.o=.d)

PKG = glfw3 vulkan cglm
PKG_CFLAGS = $(shell pkg-config --cflags $(PKG))
PKG_LDFLAGS = $(shell pkg-config --libs $(PKG))

CSTD := -std=c23
WARN = -Wall -Wextra -Wpedantic -Werror
CFLAGS := $(CSTD) $(WARN) -I$(SRC_DIR) -Ilibraries $(DEFS) $(PKG_CFLAGS) -MMD -MP
LDFLAGS := -lm $(PKG_LDFLAGS)

TARGET := vulkan_hello_world

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

$(SHADER_DIR)/%.spv: $(SHADER_DIR)/%
	glslc $< -o $@

$(SHADER_DIR)/%.spv.c: $(SHADER_DIR)/%.spv
	xxd -i $< > $@

debug: CFLAGS += -g -fsanitize=address,undefined
debug: LDFLAGS += -fsanitize=address,undefined
debug: $(TARGET)

clean:
	rm -f $(OBJ) $(DEP) $(SHADERS_SPV) $(SHADERS_C) $(TARGET)

fix:
	find src/ \( -name "*.h" -o -name "*.c" \) -exec clang-format -i {} \+

.PHONY: all clean
