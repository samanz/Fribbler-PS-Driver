CC = g++
AR = ar

CFLAGS = -Wall -c -I$(SCRIBBLER_HEADERS) -I$(PLAYER_HEADERS)
LFLAGS = -Wall


SCRIBBLER_HEADERS = include/Scribbler
SCRIBBLER_SOURCES = src/Scribbler

PLAYER_HEADERS = include/Player
PLAYER_SOURCES = src/Player

OBJS = data.o PosixSerial.o camera.o robot.o scribbler.o
LIBS = -lscribbler

libscribbler.a: $(OBJS)
	$(AR) rs libscribbler.a $(OBJS)

data.o: $(SCRIBBLER_SOURCES)/data.cpp $(SCRIBBLER_HEADERS)/data.h
	$(CC) $(CFLAGS) $(SCRIBBLER_SOURCES)/data.cpp

PosixSerial.o: $(SCRIBBLER_SOURCES)/PosixSerial.cpp $(SCRIBBLER_HEADERS)/data.h
	$(CC) $(CFLAGS) $(SCRIBBLER_SOURCES)/PosixSerial.cpp

camera.o: $(SCRIBBLER_SOURCES)/camera.cpp $(SCRIBBLER_HEADERS)/camera.h
	$(CC) $(CFLAGS) $(SCRIBBLER_SOURCES)/camera.cpp

robot.o: $(SCRIBBLER_SOURCES)/robot.cpp $(SCRIBBLER_HEADERS)/robot.h $(SCRIBBLER_HEADERS)/camera.h
	$(CC) $(CFLAGS) $(SCRIBBLER_SOURCES)/robot.cpp

scribbler.o: $(SCRIBBLER_SOURCES)/scribbler.cpp $(SCRIBBLER_HEADERS)/scribbler.h $(SCRIBBLER_HEADERS)/const.h $(SCRIBBLER_HEADERS)/robot.h $(SCRIBBLER_HEADERS)/data.h
	$(CC) $(CFLAGS) $(SCRIBBLER_SOURCES)/scribbler.cpp

clean:
	rm -rf *.o *.a a.out log1
