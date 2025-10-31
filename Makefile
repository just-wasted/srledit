CC = gcc
CFLAGS = -Wall -Wextra -Werror -g -O0 -lreadline
SRC_DIR = src
BUILD_DIR = build-debug

SRCS := $(SRC_DIR)/scalc.c
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)
DEPENDS = $(SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.d)

OUT_FILE = scalc

all: $(BUILD_DIR)/$(OUT_FILE)

# Link step: build the final binary from object files
$(BUILD_DIR)/$(OUT_FILE): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@

# Compile step: each .c to .o file
# -MMD: generate dependency files to check for changed header files
# -MP: add empty dummy rules to avoid errors if headers are missing
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

-include $(DEPENDS)

$(BUILD_DIR):
	mkdir -p $@

clean:
	rm -rf $(BUILD_DIR)

# (re-)generate compile_commands.json for clangd-LSP
refresh:
	rm -rf $(BUILD_DIR)
	bear -- make

.PHONY: all clean refresh
