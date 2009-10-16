include ../src/MyOS.XBee



all: begin demo kick robot1 robot2 robot3 robot4 findball list

SDL: test

test.o: test.cpp
	g++ -ggdb -I../src -c test.cpp  $(shell sdl-config --cflags) 

picframe.o: picframe.cpp picframe.h
	g++ -ggdb -c picframe.cpp  $(shell sdl-config --cflags) 

list.o: list.cpp
	g++ -ggdb -Wall -I../src -c list.cpp

begin.o: begin.cpp
	g++ -ggdb -Wall -I../src -c begin.cpp

robot1.o: robot1.cpp
	g++ -ggdb -Wall -I../src -c robot1.cpp

robot2.o: robot2.cpp 
	g++ -ggdb -Wall -I../src -c robot2.cpp

robot3.o: robot3.cpp 
	g++ -ggdb -Wall -I../src -c robot3.cpp


robot4.o: robot4.cpp
	g++ -ggdb -Wall -I../src -c robot4.cpp

findball.o: findball.cpp
	g++ -ggdb -Wall -I../src -c findball.cpp


kick.o: kick.cpp color.h
	g++ -ggdb -Wall -I../src -c kick.cpp


demo.o: demo.cpp 
	g++ -ggdb -Wall -I../src -c demo.cpp

football: football.o 
	g++ -ggdb -Wall -o football football.o $(LIB)

findball: findball.o 
	g++ -ggdb -Wall -o findball findball.o $(LIB)

linewalker: linewalker.o
	g++ -ggdb -Wall -o linewalker.o $(LIB)

demo: demo.o 
	g++ -ggdb -Wall -o demo demo.o $(LIB)

kick: kick.o 
	g++ -ggdb -Wall -o kick kick.o $(LIB)

robot1: robot1.o
	g++ -ggdb -Wall -o robot1 robot1.o 

robot2: robot2.o 
	g++ -ggdb -Wall -o robot2 robot2.o $(LIB)


robot3: robot3.o 
	g++ -ggdb -Wall -o robot3 robot3.o $(LIB)

robot4: robot4.o 
	g++ -ggdb -Wall -o robot4 robot4.o $(LIB)

begin: begin.o 
	g++ -ggdb -Wall -o begin begin.o $(LIB)

list: list.o 
	g++ -ggdb -Wall -o list list.o $(LIB)

test: test.o picframe.o
	g++ -ggdb -o test test.o picframe.o $(shell sdl-config --cflags --libs) ../src/surveyor.o $(OS_SPEC) $(LIB) -lSDL_image 



clean:
	$(REMOVE) *.o
	$(REMOVE) begin$(BIN) list$(bin) demo$(BIN) kick$(BIN)
	$(REMOVE) robot?$(BIN) 
	$(REMOVE) findball$(BIN)




