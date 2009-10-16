program2: program2.cpp
	g++ program2.cpp -o program2

program1: program1.cpp
	g++ program1.cpp -o program1



mytest0: mytest0.cpp
	g++ mytest0.cpp -o mytest0


all: serial.o scribbler.o mytest.o
	g++ serial.o scribbler.o mytest.o -o mytest



serial.o: serial.cpp serial.h
	g++ -g -c serial.cpp -o serial.o

scribbler.o: scribbler.cpp const.h serial.h scribbler.h
	g++ -g -c scribbler.cpp -o scribbler.o

mytest.o: mytest.cpp const.h serial.h scribbler.h
	g++ -g -c mytest.cpp -o mytest.o
