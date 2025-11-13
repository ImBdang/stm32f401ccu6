# Project name
PROJECT := main

# Architecture
MCU := cortex-m4
FPU := -mfpu=fpv4-sp-d16
FLOAT_ABI := -mfloat-abi=hard

# Toolchains
TOOLCHAINS_PATH := /home/bdang/toolchains/gcc-arm-none-eabi-14.3
PREFIX_TOOLCHAINS := arm-none-eabi
GCC := $(PREFIX_TOOLCHAINS)-gcc
CP := $(PREFIX_TOOLCHAINS)-objcopy
ASM := $(PREFIX_TOOLCHAINS)-as

# ============================== Directories ==============================
SRC_DIR := src
INC_DIR := StandardPeripheralLib/inc
SRC_INC_DIR := StandardPeripheralLib/src
BUILD_DIR := build
BUILD_INC_DIR := $(BUILD_DIR)/inc
LINKER_DIR := linker

# Linker script and startup
LD_SCRIPT := stm32f4_flash.ld
STARTUP_FILE := startup_stm32f40xx.s

# Source & objects
SRC_FILES := $(wildcard $(SRC_DIR)/*.c)
SRC_INC_FILES := $(wildcard $(SRC_INC_DIR)/*.c)
OBJECTS := $(SRC_INC_FILES:$(SRC_INC_DIR)/%.c=$(BUILD_INC_DIR)/%.o)
OBJECTS_MAIN := $(SRC_FILES:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

# Compiler flags
CFLAGS := -mcpu=$(MCU) -mthumb $(FPU) $(FLOAT_ABI) -Wall -O0 -g \
          -I$(INC_DIR) -I$(SRC_INC_DIR)\
          -DSTM32F40XX \
          -DUSE_STDPERIPH_DRIVER \
          -ffunction-sections -fdata-sections

LDFLAGS := -T$(LINKER_DIR)/$(LD_SCRIPT) --specs=nosys.specs \
           -Wl,--gc-sections -Wl,-Map=$(BUILD_DIR)/$(PROJECT).map

# Rules
.PHONY: all clean flash bin hex

all: $(BUILD_DIR)/$(PROJECT).bin
	@echo "Build complete! Flash file ready: $(BUILD_DIR)/$(PROJECT).bin"

bin: $(BUILD_DIR)/$(PROJECT).bin
hex: $(BUILD_DIR)/$(PROJECT).hex

# Build ELF
$(BUILD_DIR)/$(PROJECT).elf: $(OBJECTS) $(OBJECTS_MAIN) $(SRC_DIR)/$(STARTUP_FILE)
	@mkdir -p $(BUILD_DIR)
	@echo "Linking..."
	$(TOOLCHAINS_PATH)/bin/$(GCC) $(CFLAGS) $(OBJECTS) $(OBJECTS_MAIN) $(SRC_DIR)/$(STARTUP_FILE) $(LDFLAGS) -o $@
	@echo "Created: $@"
	@$(TOOLCHAINS_PATH)/bin/$(PREFIX_TOOLCHAINS)-size $@

# Compile .c to .o
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(BUILD_DIR)
	@echo "Compiling: $<"
	$(TOOLCHAINS_PATH)/bin/$(GCC) $(CFLAGS) -c $< -o $@

$(BUILD_INC_DIR)/%.o: $(SRC_INC_DIR)/%.c
	@mkdir -p $(BUILD_INC_DIR)
	@echo "Compiling (lib): $<"
	$(TOOLCHAINS_PATH)/bin/$(GCC) $(CFLAGS) -c $< -o $@

# Convert ELF to BIN
$(BUILD_DIR)/$(PROJECT).bin: $(BUILD_DIR)/$(PROJECT).elf
	@echo "Creating BIN file..."
	$(TOOLCHAINS_PATH)/bin/$(CP) -O binary $< $@
	@echo "Created: $@"
	@ls -lh $@

# Convert ELF to HEX
$(BUILD_DIR)/$(PROJECT).hex: $(BUILD_DIR)/$(PROJECT).elf
	@echo "Creating HEX file..."
	$(TOOLCHAINS_PATH)/bin/$(CP) -O ihex $< $@
	@echo "Created: $@"

# Clean
clean:
	@echo "Cleaning build files..."
	@rm -rf $(BUILD_DIR)
	@echo "Clean complete!"

# Flash
flash: $(BUILD_DIR)/$(PROJECT).bin
	@echo "Flashing to STM32..."
	st-flash write $< 0x8000000
	@echo "Flash complete!"