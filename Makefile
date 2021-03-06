### DEFINITIONS ###

# Toolchain definitions
CC = g++
AR = ar
CP = cp

# Dependency: MetroUtil
METROUTIL := MetroUtil/lib/libMetrobotics.a

# Player driver (Fribbler) definitions
PLAYER_OUTPUT  = libFribbler.so
PLAYER_OBJECTS = Fribbler.o
PLAYER_CFLAGS  = -Wall -fPIC -c -I./include -I./MetroUtil/include `pkg-config --cflags playercore`
PLAYER_LFLAGS  = -Wall -Wl,-z,defs -shared
PLAYER_LIBS    = -L./MetroUtil/lib -lMetrobotics -L/usr/local/lib -lplayerjpeg `pkg-config --libs playercore`
PLAYER_HEADERS_DIR = include/Player
PLAYER_SOURCES_DIR = src/Player
PLAYER_SOURCES = $(PLAYER_SOURCES_DIR)/Fribbler.cpp
PLAYER_HEADERS = $(PLAYER_HEADERS_DIR)/Fribbler.h

# Stand-alone driver (Scribbler) definitions
SCRIBBLER_OUTPUT  = libscribbler.a
SCRIBBLER_OBJECTS = data.o PosixSerial.o camera.o robot.o scribbler.o
SCRIBBLER_CFLAGS  = -Wall -fPIC -c -I$(SCRIBBLER_HEADERS_DIR)
SCRIBBLER_LFLAGS  = -Wall
SCRIBBLER_HEADERS_DIR = include/Scribbler
SCRIBBLER_SOURCES_DIR = src/Scribbler
SCRIBBLER_SOURCES = $(SCRIBBLER_SOURCES_DIR)/data.cpp         \
                    $(SCRIBBLER_SOURCES_DIR)/PosixSerial.cpp  \
                    $(SCRIBBLER_SOURCES_DIR)/camera.cpp       \
                    $(SCRIBBLER_SOURCES_DIR)/robot.cpp        \
                    $(SCRIBBLER_SOURCES_DIR)/scribbler.cpp
SCRIBBLER_HEADERS = $(SCRIBBLER_HEADERS_DIR)/const.h          \
                    $(SCRIBBLER_HEADERS_DIR)/data.h           \
                    $(SCRIBBLER_HEADERS_DIR)/PosixSerial.h    \
                    $(SCRIBBLER_HEADERS_DIR)/camera.h         \
                    $(SCRIBBLER_HEADERS_DIR)/robot.h          \
                    $(SCRIBBLER_HEADERS_DIR)/scribbler.h

### TARGETS ###

# EVERYTHING!
all:
	# Scribbler library
	$(MAKE) libscribbler
	# Player driver
	$(MAKE) Fribbler
	# Driver tester
	$(MAKE) test-fribbler
	# Square driving
	$(MAKE) square
	# Player cam
	$(MAKE) playercam

# Player driver (Fribbler)
Fribbler: $(PLAYER_OBJECTS) $(SCRIBBLER_OBJECTS)
	$(CC)  $(PLAYER_LFLAGS) -o $(PLAYER_OUTPUT) $(PLAYER_OBJECTS) $(SCRIBBLER_OBJECTS) $(PLAYER_LIBS)

Fribbler.o: $(PLAYER_SOURCES) $(PLAYER_HEADERS) $(SCRIBBLER_HEADERS) $(METROUTIL)
	$(CC) $(PLAYER_CFLAGS) $(PLAYER_SOURCES_DIR)/Fribbler.cpp

# Player driver test engine
test-fribbler: src/test-fribbler.cpp
	$(CC) src/test-fribbler.cpp `pkg-config --cflags --libs playerc++` -o test-fribbler

# Stand-alone Scribbler library.
libscribbler: $(SCRIBBLER_OBJECTS)
	@$(AR) rs $(SCRIBBLER_OUTPUT) $(SCRIBBLER_OBJECTS)
	# Make sure a lib folder exists
	@if [ ! -e "./lib" ]; then \
		mkdir "./lib"; \
	elif [ ! -d "./lib" ]; then \
		echo "Failed to install library files."; \
		echo "./lib is not a valid directory."; \
		exit 1; \
	fi
	@$(CP) ./$(SCRIBBLER_OUTPUT) ./lib/$(SCRIBBLER_OUTPUT)

data.o: $(SCRIBBLER_SOURCES_DIR)/data.cpp $(SCRIBBLER_HEADERS_DIR)/data.h
	$(CC) $(SCRIBBLER_CFLAGS) $(SCRIBBLER_SOURCES_DIR)/data.cpp

PosixSerial.o: $(SCRIBBLER_SOURCES_DIR)/PosixSerial.cpp $(SCRIBBLER_HEADERS_DIR)/data.h
	$(CC) $(SCRIBBLER_CFLAGS) $(SCRIBBLER_SOURCES_DIR)/PosixSerial.cpp

camera.o: $(SCRIBBLER_SOURCES_DIR)/camera.cpp $(SCRIBBLER_HEADERS_DIR)/camera.h
	$(CC) $(SCRIBBLER_CFLAGS) $(SCRIBBLER_SOURCES_DIR)/camera.cpp

robot.o: $(SCRIBBLER_SOURCES_DIR)/robot.cpp $(SCRIBBLER_HEADERS_DIR)/robot.h $(SCRIBBLER_HEADERS_DIR)/camera.h
	$(CC) $(SCRIBBLER_CFLAGS) $(SCRIBBLER_SOURCES_DIR)/robot.cpp

scribbler.o: $(SCRIBBLER_SOURCES_DIR)/scribbler.cpp $(SCRIBBLER_HEADERS_DIR)/scribbler.h $(SCRIBBLER_HEADERS_DIR)/const.h $(SCRIBBLER_HEADERS_DIR)/robot.h $(SCRIBBLER_HEADERS_DIR)/data.h
	$(CC) $(SCRIBBLER_CFLAGS) $(SCRIBBLER_SOURCES_DIR)/scribbler.cpp

# Dependency: MetroUtil
$(METROUTIL):
	@echo Building MetroUtil
	@$(MAKE) -e --directory="./MetroUtil" install
	@if [ "$$?" != 0 ]; then \
		echo "MetroUtil failed to build"; \
		echo "Fribbler depends on MetroUtil"; \
		exit 1; \
	fi

# Sam's test program
test:
	$(CC) src/square.cpp -I./include/Scribbler -L./lib -lscribbler

# Sam's blob test program
find:
	$(CC) find_missing.cpp -I./include/Scribbler -L./lib -lscribbler -o find-missing

# Square program
square: src/square-fribbler.cpp
	$(CC) src/square-fribbler.cpp `pkg-config --cflags --libs playerc++` -o square-fribbler

#playercam	
playercam: src/playercam.cpp
		$(CC) src/playercam.cpp `pkg-config --cflags --libs playerc++` `pkg-config --libs --cflags gtk+-2.0` -o newplayercam

# Blobfinder-test program
blobtest: src/blobfinder-test.cpp
	$(CC) src/blobfinder-test.cpp `pkg-config --cflags --libs playerc++` -o blob-test

# Calibration program
calibration: src/calibration.cpp
	$(CC) src/calibration.cpp -I./include/Scribbler -L./lib -lscribbler

clean:
	@$(MAKE) -e --directory="./MetroUtil" clean
	rm -rf *.a *.so *.o a.out log1 test-fribbler square-fribbler newplayercam
