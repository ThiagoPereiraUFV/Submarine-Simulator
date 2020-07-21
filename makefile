all : object3D.o parser.o
	g++ ./source/main.cpp -O3 -w -lglut -lGL -lGLU -lm -std=c++11 object3D.o parser.o -o main.out && ./main.out

object3D.o : ./source/object3D.cpp ./source/object3D.h
	g++ -c ./source/object3D.cpp

parser.o : ./source/parser.cpp ./source/parser.h
	g++ -c ./source/parser.cpp

clean:
	rm -f -r *.out *.o #limpa tudo
