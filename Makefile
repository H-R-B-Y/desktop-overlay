CXX := g++
TARGET := overlay

SRC_DIR := src
INC_DIR := include
BUILD_DIR := build

SRCS := $(wildcard $(SRC_DIR)/*.cpp)
OBJS := $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SRCS))

# Most distros expose layer-shell pkg-config as gtk4-layer-shell-0.
PKGS ?= gtk4 gtk4-layer-shell-0
CXXFLAGS += -Wall -Wextra -Wpedantic -g3 -O2 $(shell pkg-config --cflags $(PKGS)) -I$(INC_DIR)
LDFLAGS += $(shell pkg-config --libs $(PKGS))

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $@ $(LDFLAGS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -rf $(BUILD_DIR) $(TARGET)