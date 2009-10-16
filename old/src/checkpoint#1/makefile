

all: libscribbler.a example light simple wall scrib  photo testtry video 


ifeq ($(OS),Windows)
libscribbler.a: scribbler.o data.o robot.o camera.o WinFile.o WinSerial.o WinsocSerial.o
LIBS=-lwsock32
else
libscribbler.a: scribbler.o data.o robot.o camera.o PosixFile.o PosixSerial.o SocSerial.o
endif


scribbler.o: scribbler.cpp scribbler.h data.h
	g++ -ggdb -Wall -funsigned-char -c scribbler.cpp
	ar r libscribbler.a scribbler.o

data.o: data.cpp data.h
	g++ -ggdb -funsigned-char -c data.cpp
	ar r libscribbler.a data.o

robot.o: robot.cpp robot.h
	g++ -ggdb -funsigned-char -c robot.cpp
	ar r libscribbler.a robot.o

camera.o: camera.cpp camera.h
	g++ -ggdb -Wall -c camera.cpp
	ar r libscribbler.a camera.o


PosixFile.o : PosixFile.cpp PosixFile.h data.h
	g++ -ggdb -Wall -funsigned-char -c PosixFile.cpp
	ar r libscribbler.a PosixFile.o

PosixSerial.o: PosixSerial.cpp PosixSerial.h data.h
	g++ -ggdb -Wall -funsigned-char -c PosixSerial.cpp
	ar r libscribbler.a PosixSerial.o

SocSerial.o: SocSerial.cpp SocSerial.h data.h
	g++ -ggdb -Wall -c SocSerial.cpp
	ar r libscribbler.a SocSerial.o


WinSerial.o: WinSerial.cpp WinSerial.h data.h
	g++ -ggdb -Wall -funsigned-char -c WinSerial.cpp
	ar r libscribbler.a WinSerial.o

WinsocSerial.o: WinsocSerial.cpp WinsocSerial.h data.h
	g++ -ggdb -Wall -funsigned-char -c WinsocSerial.cpp
	ar r libscribbler.a WinsocSerial.o

WinFile.o : WinFile.cpp WinFile.h data.h
	g++ -ggdb -Wall -funsigned-char -c WinFile.cpp
	ar r libscribbler.a WinFile.o




example: example.o
	g++ -ggdb -o example example.o libscribbler.a

example.o: example.cpp
	g++ -ggdb -c example.cpp

poly: poly.o 
	g++ -ggdb -o poly poly.o 

poly.o: poly.cpp 
	g++ -ggdb -Wall -c poly.cpp

testtry: testtry.o libscribbler.a 
	g++ -ggdb -o testtry testtry.o libscribbler.a $(LIBS)

testtry.o: testtry.cpp scribbler.h
	g++ -ggdb -Wall -c testtry.cpp

light: light.o libscribbler.a
	g++ -ggdb -o light light.o libscribbler.a $(LIBS)

light.o: light.cpp scribbler.h
	g++ -ggdb -Wall -c light.cpp

simple: simple.o libscribbler.a
	g++ -ggdb -o simple simple.o libscribbler.a $(LIBS)

simple.o: simple.cpp scribbler.h
	g++ -ggdb -Wall -c simple.cpp


wall: wall.o libscribbler.a
	g++ -ggdb -o wall wall.o libscribbler.a $(LIBS)

wall.o: wall.cpp scribbler.h
	g++ -ggdb -Wall -c wall.cpp



photo: photo.o libscribbler.a
	g++ -o photo photo.o libscribbler.a $(LIBS)

photo.o: photo.cpp scribbler.h data.h
	g++ -ggdb -Wall -c photo.cpp


video: video.o scribblercmd.o flukecmd.o data.o gamengin.o camera.o poly.o libscribbler.a
	g++ -ggdb -o video $(shell sdl-config --cflags --libs) scribblercmd.o flukecmd.o gamengin.o video.o poly.o libscribbler.a -lSDL_image -lSDL_mixer -lSDL_gfx $(LIBS)

scribblercmd.o: scribblercmd.cpp VirtScribbler.h
	g++  -ggdb -Wall $(INCS) $(shell sdl-config --cflags) -c scribblercmd.cpp

flukecmd.o: flukecmd.cpp VirtScribbler.h
	g++ -ggdb -Wall $(INCS) $(shell sdl-config --cflags) -c flukecmd.cpp

video.o: video.cpp VirtScribbler.h
	g++ -ggdb -Wall $(INCS)  -I/usr/include/SDL -I/usr/local/include/SDL -c video.cpp 


scrib: scrib.o libscribbler.a
	g++ -ggdb -o scrib scrib.o libscribbler.a $(LIBS)

scrib.o: scrib.cpp scribbler.h data.h 
	g++ -ggdb -Wall -funsigned-char  -c scrib.cpp



gamengin.o: gamengin.cpp gamengin.h
	g++ -ggdb -Wall $(INCS) $(shell sdl-config --cflags --libs) -c gamengin.cpp 








Socket.o: Socket.cpp Socket.h
	g++ -ggdb -Wall -c Socket.cpp

SocServer: SocServer.cpp SocSerial.h data.h
	g++ -ggdb -Wall -o SocServer SocServer.cpp









clean:
	rm *.o
	rm libscribbler.a example light simple wall scrib  photo testtry video 




