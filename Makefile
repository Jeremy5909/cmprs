# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -O3
LDFLAGS := $(shell pkg-config --libs openssl libsodium)
OPENSSL_INC := $(shell pkg-config --cflags openssl libsodium)

# Directories
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin

# Target executable
TARGET = $(BIN_DIR)/mskertst

# Automatically find all .c files and generate matching .o files
SOURCES = $(wildcard $(SRC_DIR)/*.c)
OBJECTS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SOURCES))

# Default target
.DEFAULT_GOAL := all

all: $(TARGET)

# Link executable
$(TARGET): $(OBJECTS)
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) $(OPENSSL_INC) $(OBJECTS) $(LDFLAGS) -o $@

# Compile .c to .o
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) $(OPENSSL_INC) -c $< -o $@

# Clean up
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

.PHONY: all clean
