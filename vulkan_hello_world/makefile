CC := gcc

SRC_DIR := src
SRC := $(shell find $(SRC_DIR) -type f -name '*.c')

SHADER_DIR = src/shaders
SHADERS_SRC = $(SHADER_DIR)/shader.vert $(SHADER_DIR)/shader.frag
SHADERS_SPV := $(SHADERS_SRC:%=%.spv)
SHADERS_C := $(SHADERS_SPV:%=%.c)

SRC += $(SHADERS_C)
OBJ := $(SRC:.c=.o)

CSTD := -std=c23
CFLAGS := $(CSTD) -Wall -Wextra -Wpedantic -I$(SRC_DIR) $(shell pkg-config --cflags glfw3 vulkan cglm)
LDFLAGS := $(shell pkg-config --libs glfw3 vulkan cglm) -lm

TARGET := vulkan_hello_world

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(SHADER_DIR)/%.spv: $(SHADER_DIR)/%
	glslc $< -o $@

$(SHADER_DIR)/%.spv.c: $(SHADER_DIR)/%.spv
	xxd -i $< > $@

clean:
	rm -f $(SRC:.c=.o) $(SHADERS_SPV) $(SHADERS_C) $(TARGET)

.PHONY: all clean
