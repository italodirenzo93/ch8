BUILD_DIR := builddir
EXE_NAME := ch8

all: build run

.PHONY: setup
setup:
	meson setup $(BUILD_DIR)

.PHONY: build
build:
	meson compile -C $(BUILD_DIR)

.PHONY: run
run:
	./$(BUILD_DIR)/$(EXE_NAME)
