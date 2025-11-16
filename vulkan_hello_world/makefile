CC      := gcc
SRC_DIR := src
SRC     := $(shell find $(SRC_DIR) -type f -name '*.c')
OBJ     := $(SRC:.c=.o)

CSTD    := -std=c23
CFLAGS  := $(CSTD) -Wall -Wextra -Wpedantic -I$(SRC_DIR) $(shell pkg-config --cflags glfw3 vulkan)
LDFLAGS := $(shell pkg-config --libs glfw3 vulkan)

TARGET  := vulkan_hello_world

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(TARGET) $(OBJ)
