CC = g++
CFLAGS = -Wall -c
LFLAGS = -Wall

OBJS = data.o PosixSerial.o camera.o robot.o scribbler.o

test: temp.o $(OBJS)
	$(CC) $(LFLAGS) temp.o $(OBJS) -o a.out

temp.o: temp.cpp
	$(CC) $(CFLAGS) temp.cpp

data.o: data.cpp data.h
	$(CC) $(CFLAGS) data.cpp

PosixSerial.o: PosixSerial.cpp data.h
	$(CC) $(CFLAGS) PosixSerial.cpp

camera.o: camera.cpp camera.h
	$(CC) $(CFLAGS) camera.cpp

robot.o: robot.cpp robot.h camera.h
	$(CC) $(CFLAGS) robot.cpp

scribbler.o: scribbler.cpp scribbler.h const.h robot.h data.h
	$(CC) $(CFLAGS) scribbler.cpp

clean:
	rm -rf *.o a.out log1
