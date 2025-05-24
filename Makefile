CC = gcc
CFLAGS = -Wall -Wextra -O3
LDFLAGS := $(shell pkg-config --libs openssl libsodium)
OPENSSL_INC := $(shell pkg-config --cflags openssl libsodium)


# Directories
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin

# Targets
TARGETS = $(BIN_DIR)/ncrypt $(BIN_DIR)/dcrypt $(BIN_DIR)/mskertst
COMMON_OBJ = $(OBJ_DIR)/msker.o

.DEFAULT_GOAL := all

all: $(TARGETS)

# Build each target with msker.o
$(BIN_DIR)/ncrypt: $(OBJ_DIR)/ncrypt.o $(COMMON_OBJ)
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) $(OPENSSL_INC) $^ $(LDFLAGS) -o $@

$(BIN_DIR)/dcrypt: $(OBJ_DIR)/dcrypt.o $(COMMON_OBJ)
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) $(OPENSSL_INC) $^ $(LDFLAGS) -o $@

$(BIN_DIR)/mskertst: $(OBJ_DIR)/mskertst.o $(COMMON_OBJ)
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) $(OPENSSL_INC) $^ $(LDFLAGS) -o $@

# Compile .c files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) $(OPENSSL_INC) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

.PHONY: all clean

