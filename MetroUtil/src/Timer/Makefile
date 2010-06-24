# Toolchain/Environment
SHELL := /bin/bash
CC    := g++


# Options
INSTALLDIR := $(CURDIR)
CFLAGS     := -Wall


# Files
SUBSYSTEM := Timer
OUTPUT    := libTimer.a
SOURCES   := $(wildcard *.cpp)
OBJECTS   := $(patsubst %.cpp, %.o, $(SOURCES))
HEADERS   := $(wildcard *.h)


# General targets
.PHONY: install clean


# Default target: build the library
$(OUTPUT): $(OBJECTS)
	ar rs $(OUTPUT) $(OBJECTS)


# Install the headers and library files
install: $(OUTPUT)
# Make sure an include directory exists
	@if [ ! -e "$(INSTALLDIR)/include" ]; then \
		mkdir "$(INSTALLDIR)/include"; \
		mkdir "$(INSTALLDIR)/include/$(SUBSYSTEM)"; \
	elif [ ! -d "$(INSTALLDIR)/include" ]; then \
		echo "Failed to install headers files."; \
		echo "$(INSTALLDIR)/include is not a valid directory."; \
		exit 1; \
	else \
		if [ ! -e "$(INSTALLDIR)/include/$(SUBSYSTEM)" ]; then \
			mkdir "$(INSTALLDIR)/include/$(SUBSYSTEM)"; \
		elif [ ! -d "$(INSTALLDIR)/include/$(SUBSYSTEM)" ]; then \
			echo "Failed to install headers files."; \
			echo "$(INSTALLDIR)/include/$(SUBSYSTEM) is not a valid directory."; \
			exit 1; \
		fi \
	fi
# Install the header files
	@for file in $(HEADERS); do \
		cp "$$file" "$(INSTALLDIR)/include/$(SUBSYSTEM)"; \
	done
# Make sure a lib folder exists
	@if [ ! -e "$(INSTALLDIR)/lib" ]; then \
		mkdir "$(INSTALLDIR)/lib"; \
	elif [ ! -d "$(INSTALLDIR)/lib" ]; then \
		echo "Failed to install library files."; \
		echo "$(INSTALLDIR)/lib is not a valid directory."; \
		exit 1; \
	fi
# Install the library 
	@cp "$(OUTPUT)" "$(INSTALLDIR)/lib"


# Remove unnecessary output files
clean:
	rm -rf $(OUTPUT) $(OBJECTS)


# Individual source targets
PosixTimer.o: PosixTimer.cpp PosixTimer.h Timer.h
	$(CC) -c $(CFLAGS) PosixTimer.cpp
