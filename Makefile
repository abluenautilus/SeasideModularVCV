# CROSS_COMPILE = x86_64-apple-darwin22.4.0
# include ../../arch.mk

# If RACK_DIR is not defined when calling the Makefile, default to two directories above
RACK_DIR ?= ../..
DAISYSP_DIR = inc/DaisySP

# FLAGS will be passed to both the C and C++ compiler
FLAGS +=
CFLAGS += -O0
CXXFLAGS += -I$(DAISYSP_DIR)/Source
CXXFLAGS += -Iinc/libresample/src

# Add .cpp files to the build
SOURCES += $(wildcard src/*.cpp)
SOURCES += $(wildcard inc/*.cpp)

SOURCES +=${wildcard inc/DaisySP/Source/**/*.cpp}
SOURCES +=${wildcard inc/libresample/src/*.c}
SOURCES +=${wildcard inc/stk/src/*.cpp}

# Add files to the ZIP package when running `make dist`
# The compiled plugin and "plugin.json" are automatically added.
DISTRIBUTABLES += res
DISTRIBUTABLES += $(wildcard LICENSE*)
DISTRIBUTABLES += $(wildcard presets)

# Include the Rack plugin Makefile framework
include $(RACK_DIR)/plugin.mk

