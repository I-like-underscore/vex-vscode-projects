# VEXcode makefile 2019_03_26_01

# show compiler output
VERBOSE = 0

# Locate the SDK and ARM tools installed by the VEXcode VS Code extension.
VEXCODE_STORAGE ?= $(HOME)/Library/Application Support/Code/User/globalStorage/vexrobotics.vexcode
VEX_TOOLCHAIN_BIN ?= $(VEXCODE_STORAGE)/tools/cpp/toolchain_osxarm64/gcc/bin

ifeq ($(origin T),undefined)
VEX_SDK_PATH ?= $(shell find "$(VEXCODE_STORAGE)/sdk/cpp/V5" -mindepth 1 -maxdepth 1 -type d -name 'V5_*' -print 2>/dev/null | sort | tail -n 1)
VEX_SDK_PATH ?= $(HOME)/sdk
endif

# include toolchain options
include vex/mkenv.mk

ifneq ($(shell test -x "$(VEX_TOOLCHAIN_BIN)/arm-none-eabi-ld" && echo yes),)
LINK := "$(VEX_TOOLCHAIN_BIN)/arm-none-eabi-ld"
SIZE := "$(VEX_TOOLCHAIN_BIN)/arm-none-eabi-size"
OBJCOPY := "$(VEX_TOOLCHAIN_BIN)/arm-none-eabi-objcopy"
ARCH := "$(VEX_TOOLCHAIN_BIN)/arm-none-eabi-ar"
endif

# location of the project source cpp and c files
SRC_C  = $(wildcard src/*.cpp) 
SRC_C += $(wildcard src/*.c)
SRC_C += $(wildcard src/*/*.cpp) 
SRC_C += $(wildcard src/*/*.c)

OBJ = $(addprefix $(BUILD)/, $(addsuffix .o, $(basename $(SRC_C))) )

# location of include files that c and cpp files depend on
SRC_H  = $(wildcard include/*.h)

# additional dependancies
SRC_A  = makefile

# project header file locations
INC_F  = include

# build targets
all: $(BUILD)/$(PROJECT).bin

# include build rules
include vex/mkrules.mk
