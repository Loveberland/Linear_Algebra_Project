# For build run clean and test

ifeq ($(OS),Windows_NT)
	DETECTED_OS := Windows
	EXE_EXT := .exe

	MKDIR = if not exist "$(1)" mkdir "$(1)"
	RMDIR = if exist "$(1)" rmdir /S /Q "$(1)"
else
	DETECTED_OS := $(shell uname -s)
	EXE_EXT :=

	MKDIR = mkdir -p "$(1)"
	RMDIR = rm -rf "$(1)"
endif

CC = gcc

CFLAGS = -O3 -Wall -Wextra -Werror -g $(shell pkg-config --cflags gtk4)
LDFLAGS = $(shell pkg-config --libs gtk4)

SRC_DIR = src
OUT_DIR = out

TARGET = $(OUT_DIR)/app$(EXE_EXT)

SRC = $(filter-out $(SRC_DIR)/t_%.c, $(wildcard $(SRC_DIR)/*.c))
OBJ = $(patsubst $(SRC_DIR)/%.c, $(OUT_DIR)/%.o, $(SRC))

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET) $(LDFLAGS)

$(OUT_DIR)/%.o: $(SRC_DIR)/%.c
	$(call MKDIR,$(OUT_DIR))
	$(CC) $(CFLAGS) -c $< -o $@

run: $(TARGET)
	$(TARGET)

clean:
	$(call RMDIR,$(OUT_DIR))

os:
	@echo OS: $(DETECTED_OS)

.PHONY: all run clean os