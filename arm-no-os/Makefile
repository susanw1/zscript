# Makefile for the STM32G484RE (prev STM32F103C8) blink program
#
# Kevin Cuzner
#

PROJECT = blink

# Project Structure
SRCDIR = src/test/cpp/blink-NoI2C
COMDIR = src/test/cpp/common
BINDIR = bin
OBJDIR = obj
INCDIR = src/test/cpp/include
LDDIR = src/test/ld
OCDDIR = src/test/openocd

# Project target (SW modified)
CPU = cortex-m4
DEVICE_FAMILY = STM32G484xx
DEVICE = STM32G484RE

# Sources
CSRC = $(wildcard $(SRCDIR)/*.c) $(wildcard $(COMDIR)/*.c)
CPPSRC = $(wildcard $(SRCDIR)/*.cpp) $(wildcard $(COMDIR)/*.cpp)
ASM = $(wildcard $(SRCDIR)/*.s) $(wildcard $(COMDIR)/*.s)

# Include directories
INCLUDE  = -I$(INCDIR) -I$(INCDIR)/Core -I$(INCDIR)/STM32G4xx

# Linker 
#LSCRIPT = STM32F103X8_FLASH.ld
LSCRIPT = $(LDDIR)/STM32G484QETX_FLASH.ld

# C Flags
GCFLAGS  = -Wall -fno-common -mthumb -mcpu=$(CPU) -D$(DEVICE_FAMILY) --specs=nosys.specs -g -Wa,-ahlms=$(addprefix $(OBJDIR)/,$(notdir $(<:.c=.lst)))
GCFLAGS += $(INCLUDE)
LDFLAGS += -T$(LSCRIPT) -mthumb -mcpu=$(CPU) --specs=nosys.specs 
ASFLAGS += -mcpu=$(CPU)

# Flashing
OCDSCRIPTS = /usr/share/openocd/scripts
OCDFLAGS = --search $(OCDSCRIPTS) -f $(OCDSCRIPTS)/interface/stlink.cfg  -f $(OCDSCRIPTS)/target/stm32g4x.cfg -f $(OCDDIR)/openocd.cfg


# Tools
CC = arm-none-eabi-gcc
AS = arm-none-eabi-as
AR = arm-none-eabi-ar
LD = arm-none-eabi-ld
OBJCOPY = arm-none-eabi-objcopy
SIZE = arm-none-eabi-size
OBJDUMP = arm-none-eabi-objdump
OCD = openocd

RM = rm -r

## Build process

OBJ := $(addprefix $(OBJDIR)/,$(notdir $(CPPSRC:.cpp=.o)))
OBJ += $(addprefix $(OBJDIR)/,$(notdir $(CSRC:.c=.o)))
OBJ += $(addprefix $(OBJDIR)/,$(notdir $(ASM:.s=.o)))


all:: $(BINDIR)/$(PROJECT).bin

Build: $(BINDIR)/$(PROJECT).bin

install: $(BINDIR)/$(PROJECT).bin
	$(OCD) $(OCDFLAGS)

$(BINDIR)/$(PROJECT).hex: $(BINDIR)/$(PROJECT).elf
	$(OBJCOPY) -R .stack -O ihex $(BINDIR)/$(PROJECT).elf $(BINDIR)/$(PROJECT).hex

$(BINDIR)/$(PROJECT).bin: $(BINDIR)/$(PROJECT).elf
	$(OBJCOPY) -R .stack -O binary $(BINDIR)/$(PROJECT).elf $(BINDIR)/$(PROJECT).bin

$(BINDIR)/$(PROJECT).elf: $(OBJ)
	@mkdir -p $(dir $@)
	$(CC) $(OBJ) $(LDFLAGS) -o $(BINDIR)/$(PROJECT).elf
	$(OBJDUMP) -D $(BINDIR)/$(PROJECT).elf > $(BINDIR)/$(PROJECT).lst
	$(SIZE) $(BINDIR)/$(PROJECT).elf

macros:
	$(CC) $(GCFLAGS) -dM -E - < /dev/null

cleanBuild: clean

clean:
	$(RM) $(BINDIR)
	$(RM) $(OBJDIR)

# Compilation
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CC) $(GCFLAGS) -c $< -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.s
	@mkdir -p $(dir $@)
	$(AS) $(ASFLAGS) -o $@ $<


$(OBJDIR)/%.o: $(COMDIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(GCFLAGS) -c $< -o $@

$(OBJDIR)/%.o: $(COMDIR)/%.s
	@mkdir -p $(dir $@)
	$(AS) $(ASFLAGS) -o $@ $<

