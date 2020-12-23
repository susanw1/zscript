# Makefile for the STM32G484RE (prev STM32F103C8) blink program
#



PROJECT = usb-test

# Project Structure
CPPDIR = src/test/cpp/
RCODEDIR = ../generic-cpp-receiver/src/main/cpp/
BUILDDIR = build
OCDDIR = src/test/openocd

MBEDDIR = ../../mbed-os

MBEDPROFILE = release
MBEDPROFILE_EXTRA = --profile $(MBEDDIR)/tools/profiles/extensions/lto.json
# Project target (SW modified)
MBED_TARGET = NUCLEO_G474RE
MBED_COMPILER = GCC_ARM

# Sources
SOURCES  = --source $(MBEDDIR) --source $(CPPDIR)main --source $(RCODEDIR)RCode --source $(CPPDIR)UIP --source $(CPPDIR)channels  --source $(CPPDIR)commands

# MBED Flags
MBEDFLAGS = -m $(MBED_TARGET) -t $(MBED_COMPILER) $(SOURCES) --build $(BUILDDIR) --profile $(MBEDPROFILE) $(MBEDPROFILE_EXTRA)

# Flashing
OCDSCRIPTS = /usr/share/openocd/scripts
OCDFLAGS = --search $(OCDSCRIPTS) -f $(OCDSCRIPTS)/interface/stlink.cfg  -f $(OCDSCRIPTS)/target/stm32g4x.cfg -f $(OCDDIR)/openocd.cfg


# Tools
MBEDBUILD = $(MBEDDIR)/tools/make.py
PYTHON = python
OCD = openocd

RM = rm -r

## Build process

OBJ := $(addprefix $(OBJDIR)/,$(notdir $(CPPSRC:.cpp=.o)))
OBJ += $(addprefix $(OBJDIR)/,$(notdir $(CSRC:.c=.o)))
OBJ += $(addprefix $(OBJDIR)/,$(notdir $(ASM:.s=.o)))


all:: $(BUILDDIR)/$(PROJECT).bin

Build: $(BUILDDIR)/$(PROJECT).bin

install: $(BUILDDIR)/$(PROJECT).bin
	$(OCD) $(OCDFLAGS)


cleanBuild: clean

clean:
	$(RM) $(BUILDDIR)

# Compilation

$(BUILDDIR)/$(PROJECT).bin:
	$(PYTHON) $(MBEDBUILD) $(MBEDFLAGS)


