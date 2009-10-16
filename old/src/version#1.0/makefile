include OS.COMM



all: light check simple wall scrib cali wander wanderf sensors sensorf photo try video 

poly: poly.o 
	g++ -ggdb -o poly poly.o 

poly.o: poly.cpp 
	g++ -ggdb -Wall -c poly.cpp

try: try.o scribbler.o robot.o data.o camera.o $(FILE).o $(SERIAL).o
	g++ -ggdb -o try try.o scribbler.o robot.o data.o camera.o $(FILE).o $(SERIAL).o $(LIBS)

try.o: try.cpp scribbler.h
	g++ -ggdb -Wall -c try.cpp

light: light.o scribbler.o robot.o data.o camera.o $(FILE).o $(SERIAL).o
	g++ -ggdb -o light light.o scribbler.o robot.o data.o camera.o $(FILE).o $(SERIAL).o $(LIBS)

light.o: light.cpp scribbler.h
	g++ -ggdb -Wall -c light.cpp

simple: simple.o scribbler.o robot.o data.o camera.o $(FILE).o $(SERIAL).o
	g++ -ggdb -o simple simple.o scribbler.o robot.o data.o camera.o $(FILE).o $(SERIAL).o $(LIBS)

simple.o: simple.cpp scribbler.h
	g++ -ggdb -Wall -c simple.cpp

check: check.o scribbler.o robot.o data.o camera.o $(FILE).o $(SERIAL).o
	g++ -ggdb -o check check.o scribbler.o robot.o data.o camera.o $(FILE).o $(SERIAL).o $(LIBS)

check.o: check.cpp scribbler.h
	g++ -ggdb -Wall -c check.cpp


wall: wall.o scribbler.o robot.o data.o camera.o $(FILE).o $(SERIAL).o
	g++ -ggdb -o wall wall.o scribbler.o robot.o data.o camera.o $(FILE).o $(SERIAL).o $(LIBS)

wall.o: wall.cpp scribbler.h
	g++ -ggdb -Wall -c wall.cpp



photo: photo.o scribbler.o robot.o data.o camera.o $(FILE).o $(SERIAL).o
	g++ -o photo photo.o scribbler.o robot.o data.o camera.o $(FILE).o $(SERIAL).o $(LIBS)

photo.o: photo.cpp scribbler.h data.h
	g++ -ggdb -Wall -c photo.cpp


video: video.o scribblercmd.o flukecmd.o data.o $(FILE).o $(SERIAL).o gamengin.o camera.o poly.o
	g++ -ggdb -o video $(shell sdl-config --cflags --libs) scribblercmd.o flukecmd.o gamengin.o video.o camera.o data.o poly.o $(SERIAL).o $(FILE).o -lSDL_image -lSDL_mixer -lSDL_gfx $(LIBS)

scribblercmd.o: scribblercmd.cpp VirtScribbler.h
	g++  -ggdb -Wall $(INCS) $(shell sdl-config --cflags) -c scribblercmd.cpp

flukecmd.o: flukecmd.cpp VirtScribbler.h
	g++ -ggdb -Wall $(INCS) $(shell sdl-config --cflags) -c flukecmd.cpp

video.o: video.cpp VirtScribbler.h
	g++ -ggdb -Wall $(INCS)  -I/usr/include/SDL -I/usr/local/include/SDL -c video.cpp 

camera.o: camera.cpp camera.h
	g++ -ggdb -Wall -c camera.cpp

sensors: sensors.o data.o scribbler.o robot.o camera.o $(SERIAL).o $(FILE).o
	g++ -ggdb -o sensors sensors.o scribbler.o robot.o data.o camera.o $(SERIAL).o $(FILE).o $(LIBS)

sensorf: sensorf.o data.o scribbler.o robot.o camera.o $(SERIAL).o $(FILE).o
	g++ -ggdb -o sensorf sensorf.o scribbler.o robot.o data.o camera.o $(SERIAL).o $(FILE).o $(LIBS)


wander: wander.o data.o scribbler.o robot.o camera.o $(SERIAL).o $(FILE).o
	g++ -ggdb -funsigned-char -o wander wander.o scribbler.o robot.o data.o camera.o $(SERIAL).o $(FILE).o $(LIBS)

wanderf: wanderf.o data.o scribbler.o robot.o camera.o $(SERIAL).o $(FILE).o
	g++ -ggdb -funsigned-char -o wanderf wanderf.o scribbler.o robot.o data.o camera.o $(SERIAL).o $(FILE).o $(LIBS)

scrib: scrib.o data.o scribbler.o robot.o camera.o $(SERIAL).o $(FILE).o
	g++ -ggdb -funsigned-char -o scrib scrib.o scribbler.o robot.o data.o camera.o $(SERIAL).o $(FILE).o $(LIBS)

cali: cali.o data.o scribbler.o robot.o camera.o $(SERIAL).o $(FILE).o
	g++ -ggdb -funsigned-char -o cali cali.o scribbler.o robot.o data.o camera.o $(SERIAL).o $(FILE).o $(LIBS)

surv: surv.o surveyor.o robot.o camera.o $(SERIAL).o $(FILE).o data.o
	g++ -ggdb -funsigned-char -o surv surv.o surveyor.o robot.o data.o camera.o $(SERIAL).o $(FILE).o $(LIBS)

cali.o: cali.cpp data.h scribbler.h
	g++ -ggdb -funsigned-char -c cali.cpp

data.o: data.cpp data.h
	g++ -ggdb -funsigned-char -c data.cpp

scrib.o: scrib.cpp scribbler.h data.h 
	g++ -ggdb -Wall -funsigned-char  -c scrib.cpp

wander.o: wander.cpp scribbler.h data.h
	g++ -ggdb -Wall -funsigned-char  -c wander.cpp

wanderf.o: wanderf.cpp scribbler.h data.h
	g++ -ggdb -Wall -c wanderf.cpp

sensors.o: sensors.cpp scribbler.h data.h
	g++ -ggdb -Wall -c sensors.cpp

sensorf.o: sensorf.cpp scribbler.h data.h
	g++ -ggdb -Wall -c sensorf.cpp

gamengin.o: gamengin.cpp gamengin.h
	g++ -ggdb -Wall $(INCS) $(shell sdl-config --cflags --libs) -c gamengin.cpp 



scribbler.o: scribbler.cpp scribbler.h data.h
	g++ -ggdb -Wall -funsigned-char -c scribbler.cpp


WinSerial.o: WinSerial.cpp WinSerial.h data.h
	g++ -ggdb -Wall -funsigned-char -c WinSerial.cpp

WinFile.o : WinFile.cpp WinFile.h data.h
	g++ -ggdb -Wall -funsigned-char -c WinFile.cpp


PosixSerial.o: PosixSerial.cpp PosixSerial.h data.h
	g++ -ggdb -Wall -funsigned-char -c PosixSerial.cpp

SocSerial.o: SocSerial.cpp SocSerial.h data.h
	g++ -ggdb -Wall -c SocSerial.cpp

Socket.o: Socket.cpp Socket.h
	g++ -ggdb -Wall -c Socket.cpp

SocServer: SocServer.cpp SocSerial.h data.h
	g++ -ggdb -Wall -o SocServer SocServer.cpp



PosixFile.o : PosixFile.cpp PosixFile.h data.h
	g++ -ggdb -Wall -funsigned-char -c PosixFile.cpp






clean:
	rm *.o
	rm simple video scrib cali wander wanderf sensors sensorf photo try 




