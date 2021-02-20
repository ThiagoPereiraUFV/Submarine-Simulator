optFlags = -Ofast -std=c++11 -Wall
extraFlags = -c -Wall
oglFlags = $(optFlags) -lglut -lGL -lGLU -lm
includePath = ./source/include

Simulator : Game.o
	g++ ./source/main.cpp ./bin/*.o $(oglFlags) -o simulator.out

Game.o : Parser.o Object3D.o
	g++ $(includePath)/Game.cpp $(extraFlags) -o ./bin/Game.o

Parser.o : Object3D.o
	g++ $(includePath)/Parser.cpp $(extraFlags) -o ./bin/Parser.o

Object3D.o :
	g++ $(includePath)/Object3D.cpp $(extraFlags) -o ./bin/Object3D.o

start :
	./simulator.out

clean :
	clear && rm ./bin/*.o *.out