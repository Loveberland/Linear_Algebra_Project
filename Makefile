# For build run clean and test

CC = gcc
CFLAGS = -O3 -Wall -Wextra -Werror -g $(shell pkg-config --cflags gtk4)
LDFLAGS = $(shell pkg-config --libs gtk4)

SRC_DIR = src
OUT_DIR = out

TARGET = $(OUT_DIR)/app

SRC = $(filter-out $(SRC_DIR)/t_%.c, $(wildcard $(SRC_DIR)/*.c))
OBJ = $(patsubst $(SRC_DIR)/%.c, $(OUT_DIR)/%.o, $(SRC))

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET) $(LDFLAGS)

$(OUT_DIR)/%.o: $(SRC_DIR)/%.c
	mkdir -p $(OUT_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -rf $(OUT_DIR)

.PHONY: all run clean