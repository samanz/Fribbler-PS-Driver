# Toolchain/Environment
export SHELL := /bin/bash
export CC    := g++


# Options
export INSTALLDIR := $(CURDIR)
export CFLAGS     := -Wall -I"../" -fPIC


# Debugging support
export DEBUG ?= 1
ifeq ($(DEBUG), 0)
	CFLAGS := $(CFLAGS) -g0
else
	CFLAGS := $(CFLAGS) -g3
endif


# Files
OUTPUT     := libMetrobotics.a
SOURCES    := $(shell find ./src/ -type f -name "*.cpp")
OBJECTS    := $(shell find ./src/ -type f -name "*.o")
HEADERS    := $(shell find ./src/ -type f -name "*.h")
LIBRARIES  := $(shell find ./src/ -type f -name "*.a")
SUBSYSTEMS := $(shell find ./src/ -maxdepth 1 -mindepth 1 -type d)


# General targets
.PHONY: compile link docs install clean purge


# Default target: build the library
$(OUTPUT): $(SOURCES) $(HEADERS)
	@$(MAKE) -e compile
	@$(MAKE) -e link


# Compile the library's sub-components
compile:
	@echo "Compiling the library's sub-components."
	@for subsystem in $(SUBSYSTEMS); do \
		$(MAKE) -e --directory="$$subsystem"; \
		if [ "$$?" != "0" ]; then \
			exit 2; \
		fi \
	done


# Link the library's sub-components into a single library file
link:
	@if [ -n "$(OBJECTS)" ]; then \
		echo "Linking the library's sub-components"; \
		ar rsv $(OUTPUT) $(OBJECTS); \
	else \
		echo "The library needs to be compiled before it can be linked."; \
		exit 1; \
	fi


# Install the library's header and library files
install: $(OUTPUT)
# Make sure an include directory exists
	@if [ ! -e "$(INSTALLDIR)/include" ]; then \
		mkdir "$(INSTALLDIR)/include"; \
	elif [ ! -d "$(INSTALLDIR)/include" ]; then \
		echo "Failed to install header files."; \
		echo "$(INSTALLDIR)/include is not a valid directory."; \
		exit 1; \
	fi
# Make sure a lib folder exists
	@if [ ! -e "$(INSTALLDIR)/lib" ]; then \
		mkdir "$(INSTALLDIR)/lib"; \
	elif [ ! -d "$(INSTALLDIR)/lib" ]; then \
		echo "Failed to install library files."; \
		echo "$(INSTALLDIR)/lib is not a valid directory."; \
		exit 1; \
	fi
# Install all of the library's sub-components
	@for subsystem in $(SUBSYSTEMS); do \
		$(MAKE) -e --directory="$$subsystem" install; \
	done
# Install the all-inclusive header file
	@cp src/metrobotics.h "$(INSTALLDIR)/include"
# Install the all-inclusive library file
	@cp "$(OUTPUT)" "$(INSTALLDIR)/lib"
# Inform the user
	@echo "Header files were installed into $(INSTALLDIR)/include"
	@echo "Library files were installed into $(INSTALLDIR)/lib"


# Generate library documentation
docs:
	@echo "Generating documentation files."
	@doxygen
	@echo "Documentation files were installed into $(INSTALLDIR)/doc."


# Remove unnecessary output files
clean:
	@echo "Removing all unnecessary output files."
	rm -rf $(OUTPUT) $(OBJECTS) $(LIBRARIES)
	@for subsystem in $(SUBSYSTEMS); do \
		$(MAKE) -e --directory="$$subsystem" clean; \
	done
	@echo "All sub-directories are now clean."

# Remove absolutely everything, including doc, include, and lib directories.
purge:
	$(MAKE) -e clean
	@echo "Purging all unnecessary files and directories."
	rm -rf ./doc ./include ./lib
	@for subsystem in $(SUBSYSTEMS); do \
		rm -rf "$$subsystem/doc" "$$subsystem/include" "$$subsystem/lib"; \
	done
	@echo "All unnecessary files and directories have been purged."
