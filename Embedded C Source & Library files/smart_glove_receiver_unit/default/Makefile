###############################################################################
# Makefile for the project smart_glove_receiver_unit
###############################################################################

## General Flags
PROJECT = smart_glove_receiver_unit
MCU = atmega32
TARGET = smart_glove_receiver_unit.elf
CC = avr-gcc

CPP = avr-g++

## Options common to compile, link and assembly rules
COMMON = -mmcu=$(MCU)

## Compile options common for all C compilation units.
CFLAGS = $(COMMON)
CFLAGS += -Wall -gdwarf-2 -std=gnu99    -DF_CPU=16000000UL -Os -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums
CFLAGS += -MD -MP -MT $(*F).o -MF dep/$(@F).d 

## Assembly specific flags
ASMFLAGS = $(COMMON)
ASMFLAGS += $(CFLAGS)
ASMFLAGS += -x assembler-with-cpp -Wa,-gdwarf2

## Linker flags
LDFLAGS = $(COMMON)
LDFLAGS +=  -Wl,-Map=smart_glove_receiver_unit.map


## Intel Hex file production flags
HEX_FLASH_FLAGS = -R .eeprom -R .fuse -R .lock -R .signature

HEX_EEPROM_FLAGS = -j .eeprom
HEX_EEPROM_FLAGS += --set-section-flags=.eeprom="alloc,load"
HEX_EEPROM_FLAGS += --change-section-lma .eeprom=0 --no-change-warnings


## Include Directories
INCLUDES = -I"C:\avrprojects\smart_glove_receiver_unit\..\..\avrinlcude\project_use_lib" 

## Library Directories
LIBDIRS = -L"C:\avrinlcude\project_use_lib" 

## Objects that must be built in order to link
OBJECTS = smart_glove_receiver_unit.o uart.o 

## Objects explicitly added by the user
LINKONLYOBJECTS = 

## Build
all: $(TARGET) smart_glove_receiver_unit.hex smart_glove_receiver_unit.eep smart_glove_receiver_unit.lss size

## Compile
smart_glove_receiver_unit.o: ../smart_glove_receiver_unit.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

uart.o: ../../../avrinlcude/project_use_lib/uart.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

##Link
$(TARGET): $(OBJECTS)
	 $(CC) $(LDFLAGS) $(OBJECTS) $(LINKONLYOBJECTS) $(LIBDIRS) $(LIBS) -o $(TARGET)

%.hex: $(TARGET)
	avr-objcopy -O ihex $(HEX_FLASH_FLAGS)  $< $@

%.eep: $(TARGET)
	-avr-objcopy $(HEX_EEPROM_FLAGS) -O ihex $< $@ || exit 0

%.lss: $(TARGET)
	avr-objdump -h -S $< > $@

size: ${TARGET}
	@echo
	@avr-size -C --mcu=${MCU} ${TARGET}

## Clean target
.PHONY: clean
clean:
	-rm -rf $(OBJECTS) smart_glove_receiver_unit.elf dep/* smart_glove_receiver_unit.hex smart_glove_receiver_unit.eep smart_glove_receiver_unit.lss smart_glove_receiver_unit.map


## Other dependencies
-include $(shell mkdir dep 2>/dev/null) $(wildcard dep/*)

