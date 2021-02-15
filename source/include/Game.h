#ifndef GAME_H
#define GAME_H

#include <GL/glut.h>
#include <unistd.h>
#include <cstdlib>
#include <unordered_map>
#include <vector>
#include <functional>
#include <string>

// -------- Initial window size --------

#define WINDOW_WIDTH 500	//	Width
#define WINDOW_HEIGHT 600	//	Height

class Game {
	private:
		static GLfloat view_w, view_h;	//  Half of game window width and height size
	public:
		static void game(int, char**);
		static void init();
		static void updateVariables(const GLsizei, const GLsizei);
		static void display();
		static void reshape(const GLsizei, const GLsizei);
		static void SpecialKeys(const int, const int, const int);
		static void HandleKeyboard(const unsigned char, const int, const int);
};

#endif