### DEFINITIONS ###

# Toolchain definitions
CC = g++
AR = ar
CP = cp

# Player driver (Fribbler) definitions
PLAYER_OUTPUT  = libFribbler.so
PLAYER_OBJECTS = Fribbler.o
PLAYER_CFLAGS  = -Wall -fPIC -c -I./include `pkg-config --cflags playercore`
PLAYER_LFLAGS  = -Wall -shared -nostartfiles `pkg-config --libs playercore`
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

# Player driver (Fribbler)
Fribbler: $(PLAYER_OBJECTS) $(SCRIBBLER_OBJECTS)
	$(CC) $(PLAYER_LFLAGS) $(PLAYER_OBJECTS) $(SCRIBBLER_OBJECTS) -o $(PLAYER_OUTPUT)

Fribbler.o: $(PLAYER_SOURCES) $(PLAYER_HEADERS) $(SCRIBBLER_HEADERS)
	$(CC) $(PLAYER_CFLAGS) $(PLAYER_SOURCES_DIR)/Fribbler.cpp

# Player driver test engine
test-fribbler: src/test-fribbler.cpp
	$(CC) src/test-fribbler.cpp `pkg-config --cflags --libs playerc++` -o test-fribbler

# Stand-alone Scribbler library.
libscribbler: $(SCRIBBLER_OBJECTS)
	$(AR) rs $(SCRIBBLER_OUTPUT) $(SCRIBBLER_OBJECTS)
	$(CP) ./$(SCRIBBLER_OUTPUT) ./lib/$(SCRIBBLER_OUTPUT)

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

# Sam's test program
test:
	$(CC) src/square.cpp -I./include/Scribbler -L./lib -lscribbler

# Square program
square: src/square-fribbler.cpp
	$(CC) src/square-fribbler.cpp `pkg-config --cflags --libs playerc++` -o square-fribbler

clean:
	rm -rf *.a *.so *.o a.out log1 test-fribbler square-fribbler
