##############################################################################
# Generic Makefile for following parts AT90USBx ATMegaxUx
###############################################################################

OUTPUT=default
SOURCE_PATH=src
OBJECT_PATH=$(OUTPUT)
DEPEND_PATH=$(OUTPUT)/dep

# General Flags
TARGET = $(PROJECT).elf
CC = avr-gcc
MAKECFG = config.mk

# Options common to compile, link and assembly rules
COMMON = -mmcu=$(MCU)

# Compile options common for all C compilation units.
CFLAGS = $(COMMON)
CFLAGS += -Wall -gdwarf-2 -Os -fsigned-char -ffunction-sections
CFLAGS += -MD -MP -MT $*.o -MF $(DEPEND_PATH)/$@.d 
#CFLAGS += -MD -MP -MT $(OBJECT_PATH)/$(*F).o -MF $(DEPEND_PATH)/$(@F).d 
CFLAGS += $(INCLUDES)

# Assembly specific flags
ASMFLAGS = $(COMMON)
ASMFLAGS += -x assembler-with-cpp -Wa,-gdwarf2

# Linker flags
LDFLAGS = $(COMMON)
LDFLAGS += -Wl,-Map=$(PROJECT).map,--cref,--gc-sections,--relax

# Intel Hex file production flags
HEX_FLASH_FLAGS = -R .eeprom

# Eeprom file production flags
HEX_EEPROM_FLAGS = -j .eeprom
HEX_EEPROM_FLAGS += --set-section-flags=.eeprom="alloc,load"
HEX_EEPROM_FLAGS += --change-section-lma .eeprom=0

# Include Source files list and part informations
include $(MAKECFG)
OBJECTS = $(CSRCS:%.c=%.o) $(ASSRCS:%.s=%.o) 
#DEPENDS_= $(OBJECTS:%.o=%.o.d)
DEPENDS = $(addprefix $(DEPEND_PATH)/, $(OBJECTS:%.o=%.o.d))

vpath %.o   $(OBJECT_PATH)
vpath %.c   src
vpath %.o.d $(DEPEND_PATH)

## Build
.PHONY: all
all: $(TARGET) $(PROJECT).hex size


## Clean target
.PHONY: clean
clean:
	@echo "Clean project"
	@-rm -rf $(DEPEND_PATH)/* $(OBJECT_PATH)/* $(PROJECT).elf $(PROJECT).hex $(PROJECT).eep $(PROJECT).map

## Rebuild the project.
.PHONY: rebuild
rebuild: clean all


## Compile

# Create objects files list with sources files

.PHONY: objfiles
objfiles: $(OBJECTS)

-include $(DEPENDS)
# create object files from C source files.
%.o: %.c
	@echo 'Building file: $<'
	@$(shell mkdir -p $(OBJECT_PATH)/$(@D) 2>/dev/null)
	@$(shell mkdir -p $(DEPEND_PATH)/$(@D) 2>/dev/null)
	@$(CC) $(INCLUDES) $(CFLAGS) -c $< -o $(OBJECT_PATH)/$@


#	@$(CC) $(INCLUDES) $(CFLAGS) -c $< -o $(OBJECT_PATH)/$(@F)
	
# Preprocess & assemble: create object files from assembler source files.
%.o: %.s
	@echo 'Building file: $<'
	@$(shell mkdir -p $(OBJECT_PATH) 2>/dev/null)
	@$(shell mkdir -p $(DEPEND_PATH) 2>/dev/null)
	@$(CC) $(INCLUDES) $(ASMFLAGS) -c $< -o $(OBJECT_PATH)/$(@F)

# all dependencies


## Link
$(TARGET): $(OBJECTS)
	@echo "Linking"
	@$(CC) $(LDFLAGS) $(addprefix $(OBJECT_PATH)/,$(OBJECTS)) $(LINKONLYOBJECTS) $(LIBDIRS) $(LIBS) -o $(TARGET)

#	@$(CC) $(LDFLAGS) $(addprefix $(OBJECT_PATH)/,$(notdir $(OBJECTS))) $(LINKONLYOBJECTS) $(LIBDIRS) $(LIBS) -o $(TARGET)
%.hex: $(TARGET)
	@echo "Create hex file"
	@avr-objcopy -O ihex $(HEX_FLASH_FLAGS)  $< $@

%.eep: $(TARGET)
	@echo "Create eep file"
	@avr-objcopy $(HEX_EEPROM_FLAGS) -O ihex $< $@  || exit 0

%.lss: $(TARGET)
	@echo "Create lss file"
	@avr-objdump -h -S $< > $@

size: ${TARGET}
	@avr-size ${TARGET}

#@avr-size -C --mcu=${MCU} ${TARGET}
#@avr-size ${TARGET}

.PHONY: program
# Do not add start here, it causes it to bugg
program: $(PROJECT).hex
	$(PROGRAMMER) $(MCU) erase
	$(PROGRAMMER) $(MCU) flash $(PROJECT).hex

.PHONY: start
start:
	@echo This does not start the MCU correctly
	$(PROGRAMMER) $(MCU) start

.PHONY: tags
tags:
	ctags  . -R *
