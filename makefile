all : object3D.o parser.o
	g++ main.cpp -O3 -lglut -lGL -lGLU -lm -std=c++11 object3D.o parser.o -o main.out

object3D.o : object3D.cpp object3D.h
	g++ -c object3D.cpp

parser.o : parser.cpp parser.h
	g++ -c parser.cpp

clean:
	rm -f -r *.out *.o #limpa tudo
