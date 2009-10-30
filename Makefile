CC = g++
AR = ar

PLAYER_CFLAGS = -Wall -fPIC -c -I./include `pkg-config --cflags playercore`
PLAYER_LFLAGS = -Wall -o libFribbler.so -shared -nostartfiles -L./lib -lscribbler `pkg-config --libs playercore`
PLAYER_HEADERS = include/Player
PLAYER_SOURCES = src/Player
PLAYER_OBJECTS = Fribbler.o

SCRIBBLER_CFLAGS = -Wall -c -I$(SCRIBBLER_HEADERS)
SCRIBBLER_LFLAGS = -Wall
SCRIBBLER_HEADERS = include/Scribbler
SCRIBBLER_SOURCES = src/Scribbler
SCRIBBLER_OBJECTS = data.o PosixSerial.o camera.o robot.o scribbler.o

# Player driver
default: $(PLAYER_OBJECTS) ./lib/libscribbler.a
	$(CC) $(PLAYER_LFLAGS) $(PLAYER_OBJECTS)

Fribbler.o: $(PLAYER_SOURCES)/Fribbler.cpp $(PLAYER_HEADERS)/Fribbler.h $(SCRIBBLER_HEADERS)/scribbler.h
	$(CC) $(PLAYER_CFLAGS) $(PLAYER_SOURCES)/Fribbler.cpp

# Build the Scribbler library.
libscribbler: $(SCRIBBLER_OBJECTS)
	$(AR) rs libscribbler.a $(SCRIBBLER_OBJECTS)

data.o: $(SCRIBBLER_SOURCES)/data.cpp $(SCRIBBLER_HEADERS)/data.h
	$(CC) $(SCRIBBLER_CFLAGS) $(SCRIBBLER_SOURCES)/data.cpp

PosixSerial.o: $(SCRIBBLER_SOURCES)/PosixSerial.cpp $(SCRIBBLER_HEADERS)/data.h
	$(CC) $(SCRIBBLER_CFLAGS) $(SCRIBBLER_SOURCES)/PosixSerial.cpp

camera.o: $(SCRIBBLER_SOURCES)/camera.cpp $(SCRIBBLER_HEADERS)/camera.h
	$(CC) $(SCRIBBLER_CFLAGS) $(SCRIBBLER_SOURCES)/camera.cpp

robot.o: $(SCRIBBLER_SOURCES)/robot.cpp $(SCRIBBLER_HEADERS)/robot.h $(SCRIBBLER_HEADERS)/camera.h
	$(CC) $(SCRIBBLER_CFLAGS) $(SCRIBBLER_SOURCES)/robot.cpp

scribbler.o: $(SCRIBBLER_SOURCES)/scribbler.cpp $(SCRIBBLER_HEADERS)/scribbler.h $(SCRIBBLER_HEADERS)/const.h $(SCRIBBLER_HEADERS)/robot.h $(SCRIBBLER_HEADERS)/data.h
	$(CC) $(SCRIBBLER_CFLAGS) $(SCRIBBLER_SOURCES)/scribbler.cpp

test:
	$(CC) src/square.cpp -I./include/Scribbler -L./lib -lscribbler

clean:
	rm -rf *.a *.so *.o a.out log1
