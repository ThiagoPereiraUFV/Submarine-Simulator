optFlags = -Ofast -std=c++11 -Wall
extraFlags = -c -Wall
oglFlags = $(optFlags) -lglut -lGL -lGLU -lm
includePath = ./source/include

Simulator : Object3D.o Parser.o
	g++ ./source/main.cpp $(oglFlags) ./bin/*.o -o simulator.out

Object3D.o :
	g++ $(includePath)/object3D.cpp $(extraFlags) -o ./bin/Object3D.o

Parser.o :
	g++ $(includePath)/parser.cpp $(extraFlags) -o ./bin/Parser.o

start :
	./simulator.out

clean :
	rm ./bin/*.o *.out