# Inspired by https://github.com/uwrobotics/MarsRoverFirmware

.PHONY := build clean help

TARGET = HEXIWEAR
MBED_TOOLS_FLAGS :=

# Options
PROFILE ?= develop
CLEAN ?=
FLASH ?=
STERM ?=

# Convert lowercase command args
ifneq (,$(app))
	APP = $(app)
endif
ifneq (,$(mode))
	MODE = $(mode)
endif
ifneq (,$(profile))
	PROFILE = $(profile)
endif
ifneq (,$(clean))
	CLEAN = $(clean)
endif
ifneq (,$(flash))
	FLASH = $(flash)
endif
ifneq (,$(sterm))
	STERM = $(sterm)
endif

# Set tools flags
ifneq (,$(PROFILE))
	MBED_TOOLS_FLAGS += --profile $(PROFILE)
endif
ifneq (,$(CLEAN))
	MBED_TOOLS_FLAGS += --clean
endif
ifneq (,$(FLASH))
	MBED_TOOLS_FLAGS += --flash
endif
ifneq (,$(STERM))
	MBED_TOOLS_FLAGS += --sterm
endif

help:
	@echo 'usage: make [target] [flags]'
	@echo
	@echo 'targets:'
	@echo '  build		build an application'
	@echo '  clean		clean project structure'
	@echo '  help		show this help messasge'
	@echo
	@echo 'flags:'
	@echo '  app		name of the app to build'
	@echo '  mode		one of: svm, knn, cnn, lstm'
	@echo '  profile	build profile to use'
	@echo '  clean		perform a clean build'
	@echo '  flash		flash the binary to a board'
	@echo '  sterm		open a serial connection after flashing'

build:
	@echo "Building $(APP) for $(TARGET) board with $(PROFILE) profile and mode $(MODE)".
	@export APP=$(APP); export MODE=$(MODE); mbed-tools compile -t GCC_ARM -m $(TARGET) $(MBED_TOOLS_FLAGS)

clean:
	@echo "Deleteing all build files"
	rm -rf cmake_build