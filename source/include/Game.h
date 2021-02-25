#ifndef GAME_H
#define GAME_H

#include <GL/gl.h>
#include <GL/glut.h>
#include <GL/freeglut.h>
#include <cstdlib>
#include <unordered_map>
#include <vector>
#include <cmath>
#include <string>
#include <thread>
#include <algorithm>
#include "Object3D.h"
#include "Parser.h"

//	Using resources
using std::thread;
using std::for_each;

class Game {
	private:
		static GLsizei view_w, view_h;	//  Current window width and height size
		static bool acc, turn, immerse, emerge;	//	Submarine motion state variables
		static GLfloat velocity, deltaVel, angVelocity, deltaAngVel;	//	Submarine motion variables
		static bool started;	//  State variable
		static bool help;	//  Help box state variable
		static bool light, lightMode, l1, l2;	//  Lights state variables
		static const vector<GLfloat> spotlight;	//	Spotlight position
		static const vector<GLfloat> sunlight;	//	Sunlight position
		static constexpr GLfloat c1 = 0.003, c2 = 0.008;	//	Constant aux variables
		static vector<GLdouble> viewer, center;	//	Viewers position and center
		static bool fp;	//	Camera state variable
		static bool upSub, upA, upShip;	//	Aux variables
		static int dispSub, dispA, dispShip;	//	Aux variables
		static const GLint nSeaAnimals = 20, nShips = 5, nHelis = 5;	//  Objects quantities
		static Object3D submarine;	//	Vector containing game submarine
		static vector<Object3D> ships;	//	Vector containing all game ships
		static vector<Object3D> fishes;	//	Vector containing all game fishes
		static vector<Object3D> sharks;	//	Vector containing all game sharks
		static vector<Object3D> helis;	//	Vector containing all game helicopters
		static vector<vector<GLfloat>> verticesSea;	//	Vertices representing sea
	public:
		static void game(int, char**);
		static void init();
		static void updateVariables(const GLsizei, const GLsizei);
		static void display();
		static void reshape(const GLsizei, const GLsizei);
		static void SpecialKeys(const int, const int, const int);
		static void SpecialKeysUp(const int, const int, const int);
		static void HandleKeyboard(const unsigned char, const int, const int);
		static void HandleKeyboardUp(const unsigned char, const int, const int);
		static void drawAxes();
		static void drawFaceSea(const GLint, const GLint, const GLint, const GLint);
		static void drawSea();
		static void drawSun();
		static void subMotion(const int);
		static void subAnimalsAnimation(const int);
		static void shipAnimation(const int);
		static void drawObject(const Object3D&);
		static void drawText(const GLdb3,const string);
		static void drawHelpMenu();
		static void loadSeaAnimals();
		static void loadShips();
		static void loadHelis();
};

#endif