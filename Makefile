# ============================================================
# 🖥️ HOST TEST SETTINGS
# ============================================================

PYTHON = python3
TOOLS_DIR = tools
INPUT_FILE = tools/input.txt
OUTPUT_FILE = tools/output.txt

UART0_PORT = /dev/ttyUSB0
UART1_PORT = /dev/ttyUSB1
TEST_BAUD = 9600

# ============================================================
# 🔥 UART-CLI Makefile - Arduino Mega 2560 (GCC 5.4 Compatible)
# ============================================================

# Target microcontroller
MCU = atmega2560
F_CPU = 16000000UL

# Compiler and tools
CC = avr-gcc
OBJCOPY = avr-objcopy
SIZE = avr-size
AVRDUDE = avrdude

# Programmer settings
PROGRAMMER = wiring
PORT = /dev/ttyUSB0
BAUD = 115200

# Project structure
SRC_DIR = src
DRIVERS_DIR = $(SRC_DIR)/drivers
APP_DIR = $(SRC_DIR)/app
COMMON_DIR = $(SRC_DIR)/common
BUILD_DIR = build

# Include directories
INCLUDES = -I$(SRC_DIR) \
           -I$(DRIVERS_DIR) \
           -I$(APP_DIR) \
           -I$(COMMON_DIR)

# Automatically find all .c files
SOURCES = $(wildcard $(SRC_DIR)/*.c) \
          $(wildcard $(DRIVERS_DIR)/*.c) \
          $(wildcard $(APP_DIR)/*.c) \
          $(wildcard $(COMMON_DIR)/*.c)

# Generate object file names
OBJECTS = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SOURCES))

# Output files
TARGET = $(BUILD_DIR)/main
ELF = $(TARGET).elf
HEX = $(TARGET).hex
MAP = $(TARGET).map
LST = $(TARGET).lst

# ============================================================
# 🛡️ COMPILER FLAGS (Compatible with GCC 5.4)
# ============================================================
CFLAGS = -mmcu=$(MCU) \
         -DF_CPU=$(F_CPU) \
         $(INCLUDES) \
         -Os \
         -std=gnu11 \
         -Wall \
         -Wextra \
         -Wpedantic \
         -Wstrict-prototypes \
         -Wmissing-prototypes \
         -Wcast-align \
         -Wcast-qual \
         -Wconversion \
         -Wsign-conversion \
         -Wshadow \
         -Wunused \
         -Wundef \
         -Wredundant-decls \
         -Wformat=2 \
         -Wformat-security \
         -Wpointer-arith \
         -Wwrite-strings \
         -fno-common \
         -ffunction-sections \
         -fdata-sections

# Linker flags
LDFLAGS = -mmcu=$(MCU) \
          -Wl,--gc-sections \
          -Wl,-Map=$(MAP),--cref \
          -Wl,--relax

# ============================================================
# 🎯 BUILD RULES
# ============================================================

# Default target
all: $(HEX) size
	@echo ""
	@echo "✅ Build complete!"
	@echo "📦 Output: $(HEX)"

# Create build directory structure
$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)
	@mkdir -p $(BUILD_DIR)/drivers
	@mkdir -p $(BUILD_DIR)/app
	@mkdir -p $(BUILD_DIR)/common

# Compile .c files to .o files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	@echo "🔨 Compiling $<..."
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) -c $< -o $@

# Link ELF
$(ELF): $(OBJECTS)
	@echo "🔗 Linking $(ELF)..."
	@$(CC) $(LDFLAGS) $(OBJECTS) -o $@

# Generate HEX
$(HEX): $(ELF)
	@echo "📦 Creating $(HEX)..."
	@$(OBJCOPY) -O ihex -R .eeprom $< $@

# ============================================================
# 🚀 FLASH TO ARDUINO
# ============================================================

flash: $(HEX)
	@echo ""
	@echo "⚡ Flashing to Arduino Mega on $(PORT)..."
	@$(AVRDUDE) -p $(MCU) -c $(PROGRAMMER) -P $(PORT) -b $(BAUD) -D -U flash:w:$(HEX):i
	@echo ""
	@echo "✅ Flash complete!"

# ============================================================
# 🧹 CLEANUP
# ============================================================

clean:
	@echo "🧹 Cleaning build files..."
	@rm -rf $(BUILD_DIR)
	@echo "✅ Clean complete!"

# Clean and rebuild
rebuild: clean all

# ============================================================
# 🔍 ANALYSIS TOOLS
# ============================================================

# Show memory usage
size: $(ELF)
	@echo ""
	@echo "📊 Memory Usage:"
	@$(SIZE) --format=avr --mcu=$(MCU) $(ELF)

# Show detailed size breakdown
size-detailed: $(ELF)
	@echo ""
	@echo "📊 Detailed Memory Breakdown:"
	@$(SIZE) -A -x $(ELF)

# Show memory map
map: $(MAP)
	@echo "📄 Memory map: $(MAP)"
	@cat $(MAP)

# Disassemble (see assembly code)
disasm: $(ELF)
	@avr-objdump -d -S $(ELF) > $(LST)
	@echo "📄 Disassembly saved to $(LST)"

# Show compilation database (for debugging includes)
includes:
	@echo "Include paths:"
	@echo $(INCLUDES)

# Verify all source files are found
sources:
	@echo "Source files found:"
	@echo $(SOURCES)

# ============================================================
# 🎯 HELP
# ============================================================

help:
	@echo "🔥 UART-CLI Professional Makefile"
	@echo ""
	@echo "📦 Build Commands:"
	@echo "  make              - Build the project"
	@echo "  make flash        - Build and flash to Arduino"
	@echo "  make clean        - Remove all build files"
	@echo "  make rebuild      - Clean and build from scratch"
	@echo ""
	@echo "🔍 Analysis Commands:"
	@echo "  make size         - Show memory usage summary"
	@echo "  make size-detailed - Show detailed memory breakdown"
	@echo "  make map          - Show memory map"
	@echo "  make disasm       - Generate assembly listing"
	@echo ""
	@echo "🛠️ Debug Commands:"
	@echo "  make sources      - List all source files"
	@echo "  make includes     - Show include paths"
	@echo "  make help         - Show this help"

# ============================================================
# 🧪 UART INTEGRATION TEST
# ============================================================

test:
	@echo ""
	@echo "🧪 Running UART integration test..."
	@$(PYTHON) $(TOOLS_DIR)/uart_bridge.py \
		--uart0 $(UART0_PORT) \
		--uart1 $(UART1_PORT) \
		--baud $(TEST_BAUD) \
		--input $(INPUT_FILE) \
		--output $(OUTPUT_FILE)
	@echo ""
	@echo "✅ Test complete!"

# ============================================================
# 📋 PHONY TARGETS
# ============================================================

.PHONY: all flash clean rebuild size size-detailed map disasm includes sources help test
