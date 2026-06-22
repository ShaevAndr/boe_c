CC ?= gcc

TARGET := build/test_modbus
SANITIZER_TARGET := build/test_modbus_sanitize

CPPFLAGS := -I../.. -I../../Modbus -I../../Unicorn2
CFLAGS := -std=c99 -Wall -Wextra -Werror -Wno-unused-parameter -O0 -g
SANITIZER_FLAGS := -fsanitize=address,undefined -fno-omit-frame-pointer

SOURCES := \
	test_modbus.c \
	../../Modbus/ModbusUtils.c \
	../../Modbus/ErrorHandler.c \
	../../Modbus/ReadHoldingRegisters.c \
	../../Modbus/ReadInputRegisters.c \
	../../Modbus/WriteSingleRegister.c \
	../../Modbus/WriteMultipleRegisters.c \
	../../Modbus/ReadFileRecord.c \
	../../Modbus/ReadDeviceIdentification.c \
	../../Modbus/CommandParcerModbus.c \
	../../Modbus/ModbusRtuFrame.c \
	../../Unicorn2/crc16.c

.PHONY: all test sanitize clean

all: $(TARGET)

$(TARGET): $(SOURCES)
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(SOURCES) -o $@

$(SANITIZER_TARGET): $(SOURCES)
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(SANITIZER_FLAGS) $(SOURCES) -o $@

test: $(TARGET)
	./$(TARGET)

sanitize: $(SANITIZER_TARGET)
	./$(SANITIZER_TARGET)

clean:
	rm -rf build
