#include "Game.h"

GLsizei Game::view_w = WINDOW_WIDTH, Game::view_h = WINDOW_HEIGHT;
bool Game::started = false, Game::started1 = false, Game::help = false, Game::fp = false;
bool Game::light = true, Game::lightMode = false, Game::l1 = true, Game::l2 = true;
const vector<GLfloat> Game::spotlight{0.8, 0.8, 0.8, 1.0};
const vector<GLfloat> Game::sunlight{0.8, 0.8, 0.8, 1.0};
vector<GLdouble> Game::viewer, Game::center;
bool Game::upSub = true, Game::upA = true, Game::upShip = true;
int Game::dispSub = 0, Game::dispA = 0, Game::dispShip = 0;
Object3D Game::submarine;
vector<Object3D> Game::ships, Game::fishes, Game::sharks, Game::helis;
vector<vector<GLfloat>> Game::verticesSea;

//	Set up OpenGl and start game
void Game::game(int argc, char** argv) {
	glutInit(&argc, argv);
	init();
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutSpecialFunc(SpecialKeys);
	glutKeyboardFunc(HandleKeyboard);
	glutIdleFunc(display);
	glutMainLoop();
}

//	Set up OpenGl window
void Game::init() {
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutInitWindowPosition(
		(glutGet(GLUT_SCREEN_WIDTH) - WINDOW_WIDTH) / 2,
		(glutGet(GLUT_SCREEN_HEIGHT) - WINDOW_HEIGHT) / 2
	);
	glutCreateWindow("Submarine simulator by Wallace & Thiago");
	glClearColor(0.0, (GLfloat)227 / (GLfloat)255, 1.0, 1.0);
	//glutFullScreen();

	//	Enabling features
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_DEPTH_TEST);
	srand(time(NULL));

	//	Setting up lights
	glLightfv(GL_LIGHT0, GL_AMBIENT, &sunlight[0]);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, &spotlight[0]);

	//	Loading submarine
	submarine = parserOBJ::parse("models/submarine.obj");
	GLdb3 color, pos;
	color.x = (GLdouble)(30) / (GLdouble)255;
	color.y = (GLdouble)(50) / (GLdouble)255;
	color.z = (GLdouble)(20) / (GLdouble)255;
	submarine.setColor(color);

	//	Loading sea animals
	for(int i = 0; i < nSeaAnimals; i++) {
		//	Fishes
		const int dist = view_h;
		fishes.push_back(parserOBJ::parse("models/fish1.obj"));

		pos.x = (rand() % 2 == 0) ? -(rand() % dist) : rand() % dist;
		pos.z = (rand() % 2 == 0) ? -(rand() % dist) : rand() % dist;
		pos.y = -(rand() % dist);
		fishes[i].setPos(pos);

		color.x = (GLdouble)(rand() % 255) / (GLdouble)255;
		color.y = (GLdouble)(rand() % 255) / (GLdouble)255;
		color.z = (GLdouble)(rand() % 255) / (GLdouble)255;
		fishes[i].setColor(color);

		//	Shark
		sharks.push_back(parserOBJ::parse("models/shark1.obj"));

		pos.x = (rand() % 2 == 0) ? -(rand() % dist) : rand() % dist;
		pos.z = (rand() % 2 == 0) ? -(rand() % dist) : rand() % dist;
		pos.y = -(rand() % dist) - 5;
		sharks[i].setPos(pos);

		color.x = (GLdouble)(rand() % 255) / (GLdouble)255;
		color.y = (GLdouble)(rand() % 255) / (GLdouble)255;
		color.z = (GLdouble)(rand() % 255) / (GLdouble)255;
		sharks[i].setColor(color);
	}

	//	Loading ships
	for(int i = 0; i < nShips; i++) {
		const int dist = 1000;
		ships.push_back(parserOBJ::parse("models/ship1.obj"));

		pos.x = (rand() % 2 == 0) ? -(rand() % dist) : rand() % dist;
		pos.z = (rand() % 2 == 0) ? -(rand() % dist) : rand() % dist;
		pos.y = (GLdouble)0.0;
		ships[i].setPos(pos);

		const double c = rand() % 255;
		color.x = color.y = color.z = (GLdouble)c / (GLdouble)255;
		ships[i].setColor(color);
	}

	//	Loading helicopters
	for(int i = 0; i < nHelis; i++) {
		const int dist = 1000;
		helis.push_back(parserOBJ::parse("models/helicopter.obj"));

		pos.x = (rand() % 2 == 0) ? -(rand() % dist) : rand() % dist;
		pos.z = (rand() % 2 == 0) ? -(rand() % dist) : rand() % dist;
		pos.y = (rand() % (dist/5)) + 50;
		helis[i].setPos(pos);
	}
}

void Game::updateVariables(const GLsizei w, const GLsizei h) {
	view_w = w;
	view_h = h;
	verticesSea = {
		{-(GLfloat)view_h, -(GLfloat)view_h, (GLfloat)view_h},
		{-(GLfloat)view_h, (GLfloat)view_h, (GLfloat)view_h},
		{(GLfloat)view_h, (GLfloat)view_h, (GLfloat)view_h},
		{(GLfloat)view_h, -(GLfloat)view_h, (GLfloat)view_h},
		{-(GLfloat)view_h, -(GLfloat)view_h, -(GLfloat)view_h},
		{-(GLfloat)view_h, (GLfloat)view_h, -(GLfloat)view_h},
		{(GLfloat)view_h, (GLfloat)view_h, -(GLfloat)view_h},
		{(GLfloat)view_h, -(GLfloat)view_h, -(GLfloat)view_h}
	};

	if(!started) {
		viewer = {0.0, 2.5 * c1 * view_h + 10, 5.0 * c1 * view_h + 10};
		const GLdb3 pos = submarine.getPos();
		center = {pos.x, pos.y, pos.z};
		started = true;
		subAnimation(0);
		subAnimalsAnimation(0);
		shipAnimation(0);
	}
}

//	Resize game elements to keep ratio
void Game::reshape(const GLsizei w, const GLsizei h) {
	if(w < WINDOW_WIDTH) {
		glutReshapeWindow(WINDOW_WIDTH, h);
	}

	updateVariables(w, h);
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(70.0, (GLfloat)view_w / (GLfloat)view_h, 2.0, view_h * 6);
	glMatrixMode(GL_MODELVIEW);
	glutPostRedisplay();
}

//	Render game elements
void Game::display() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//	Setting up observer camera
	glLoadIdentity();
	gluLookAt(
	    viewer[0], (fp) ? viewer[1] - 5 : viewer[1], (fp) ? viewer[2] - 22 : viewer[2],
	    center[0], (fp) ? center[1] + 5 : center[1] + 10, (fp) ? center[2] - 22 : center[2] + 2,
	    0.0, 1.0, 0.0	//	View up vector
	);

	const GLdb3 disp = submarine.getPos();
	const GLdouble degRotation = submarine.getRot();

	//	Setting spotlight
	const vector<GLfloat> lPos = {(GLfloat)viewer[0], (GLfloat)viewer[1], (GLfloat)viewer[2], 1.0};
	const vector<GLfloat> lDir = {(GLfloat)center[0], (GLfloat)center[1], (GLfloat)center[2]};
	glLightfv(GL_LIGHT1, GL_POSITION, &lPos[0]);
	glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, &lDir[0]);

	glTranslated(disp.x, disp.y, disp.z);
	glRotated(degRotation, 0.0, 1.0, 0.0);
	glTranslated(-disp.x, -disp.y, -disp.z);

	//drawAxes();
	drawSun();
	drawSea();

	for(const auto ship : ships) {
		ship.draw();
	}

	for(const auto heli : helis) {
		heli.draw();
	}

	for(long unsigned int i = 0; i < fishes.size() && i < sharks.size(); i++) {
		fishes[i].draw();
		sharks[i].draw();
	}

	submarine.draw();

	if(help)
		drawHelpMenu();

	if(!started1)
		started1 = true;

	glFlush();
	glutSwapBuffers();
}

//	Handle arrow keys events
void Game::SpecialKeys(const int key, const int x, const int y) {
	GLdouble degRotation = submarine.getRot();
	switch(key) {
		case GLUT_KEY_UP:
			if(center[1] < 0) {
				viewer[1] += 1.0;
				center[1] += 1.0;
			}
			break;
		case GLUT_KEY_DOWN:
			if(center[1] >= -view_h) {
				viewer[1] -= 1.0;
				center[1] -= 1.0;
			}
			break;
		case GLUT_KEY_RIGHT:
			degRotation += ROTATION;
			if(degRotation >= 360)
				degRotation = 0;
			break;
		case GLUT_KEY_LEFT:
			degRotation -= ROTATION;
			if(degRotation <= 0)
				degRotation = 360;
			break;
	}
	GLdb3 ct;
	ct.x = center[0];
	ct.y = center[1];
	ct.z = center[2];
	submarine.setRot(degRotation);
	submarine.setPos(ct);
	glutPostRedisplay();
}

//	Handle enter, esc e spacebar keys events
void Game::HandleKeyboard(const unsigned char key, const int x, const int y) {
	const GLdouble radRotation = submarine.getRot()*(M_PI/180.0);
	const GLdouble cos_rot = cos(radRotation), sin_rot = sin(radRotation);
	switch(key) {
		case 'W':
		case 'w':
			viewer[2] -= 1.0 * cos_rot;
			center[2] -= 1.0 * cos_rot;
			viewer[0] += 1.0 * sin_rot;
			center[0] += 1.0 * sin_rot;
			break;
		case 'S':
		case 's':
			viewer[2] += 1.0 * cos_rot;
			center[2] += 1.0 * cos_rot;
			viewer[0] -= 1.0 * sin_rot;
			center[0] -= 1.0 * sin_rot;
			break;
		case 'H':
		case 'h':
			help = (help) ? false : true;
			break;
		case 'F':
		case 'f':
			fp = false;
			break;
		case 'I':
		case 'i':
			fp = true;
			break;
		case 'L':
		case 'l':
			if(light) {
				light = false;
				l1 = l2 = true;
				glDisable(GL_LIGHTING);
				glEnable(GL_LIGHT0);
				glEnable(GL_LIGHT1);
				glClearColor(0.0, (GLfloat)227 / (GLfloat)255, 1.0, 1.0);
			} else {
				glEnable(GL_LIGHTING);
				light = true;
			}
			break;
		case 'G':
		case 'g':
			glShadeModel((lightMode) ? GL_SMOOTH : GL_FLAT);
			lightMode = (lightMode) ? false : true;
			break;
		case '1':
			if(l1 && light) {
				glDisable(GL_LIGHT0);
				l1 = false;
				glClearColor((GLfloat)8 / (GLfloat)255,
				             (GLfloat)24 / (GLfloat)255,
				             (GLfloat)97 / (GLfloat)255, 1.0);
			} else if(light) {
				glEnable(GL_LIGHT0);
				l1 = true;
				glClearColor(0.0, (GLfloat)227 / (GLfloat)255, 1.0, 1.0);
			}
			break;
		case '2':
			if(l2 && light) {
				glDisable(GL_LIGHT1);
				l2 = false;
			} else if(light) {
				glEnable(GL_LIGHT1);
				l2 = true;
			}
			break;
		case 27:
			exit(0);
			break;
	}
	GLdb3 ct;
	ct.x = center[0];
	ct.y = center[1];
	ct.z = center[2];
	submarine.setPos(ct);
	glutPostRedisplay();
}

void Game::drawAxes() {
	const GLfloat length = 1000;
	glPushMatrix();
	glColor3f(1.0, 0.0, 0.0);
	glBegin(GL_LINES);
		glVertex3f(0.0, 0.0, length);
		glVertex3f(0.0, 0.0, -length);
		glVertex3f(0.0, length, 0.0);
		glVertex3f(0.0, -length, 0.0);
		glVertex3f(length, 0.0, 0.0);
		glVertex3f(-length, 0.0, 0.0);
	glEnd();
	glPopMatrix();
}

void Game::drawFaceSea(const GLint a, const GLint b, const GLint c, const GLint d) {
	glColor4f(0.0, 0.0, 1.0, 0.9);
	glPushMatrix();
	glTranslatef(viewer[0], -view_h, viewer[2]);
	glScalef(3.83, 1.0, 3.83);
	glBegin(GL_QUADS);
		glVertex3fv(&verticesSea[a][0]);
		glVertex3fv(&verticesSea[b][0]);
		glVertex3fv(&verticesSea[c][0]);
		glVertex3fv(&verticesSea[d][0]);
	glEnd();
	glPopMatrix();
}

void Game::drawSea() {
	drawFaceSea(1, 2, 3, 0);
	drawFaceSea(6, 7, 3, 2);
	drawFaceSea(3, 7, 4, 0);
	drawFaceSea(5, 6, 2, 1);
	drawFaceSea(7, 6, 5, 4);
	drawFaceSea(4, 5, 1, 0);
}

void Game::drawSun() {
	const GLfloat cRadiusSun = (!l1 && light) ? 0.01 : 0.02, cXSun = 0.09, cYSun = 0.08, cZSun = 0.18;
	glColor3f(1.0, 0.9, 0.0);
	glPushMatrix();
		glTranslatef(viewer[0] + cXSun*view_h*3, cYSun*view_h*2, viewer[2] - cZSun*view_h*3);
		glutSolidSphere(cRadiusSun*view_h, 100, 8);
	glPopMatrix();
	const vector<GLfloat> light_position{
		(GLfloat)viewer[0] + cXSun*view_h*3, cYSun*view_h*2,
		(GLfloat)viewer[2] - cZSun*view_h*3,
		0.0
	};
    glLightfv(GL_LIGHT0, GL_POSITION, &light_position[0]);
}

void Game::subAnimation(const int x) {
	if(!upSub) {
		if(dispSub >= 5)
			upSub = true;
		else {
			dispSub++;
			center[1] += 0.05;
		}
	} else if(upSub) {
			if(dispSub <= 0)
				upSub = false;
			else {
				dispSub--;
				center[1] -= 0.05;
			}
	}
	GLdb3 ct;
	ct.x = center[0];
	ct.y = center[1];
	ct.z = center[2];
	submarine.setPos(ct);
	glutTimerFunc(250, subAnimation, x);
	glutPostRedisplay();
}

void Game::subAnimalsAnimation(const int x) {
	for(long unsigned int i = 0; i < fishes.size() && i < sharks.size(); i++) {
		GLdb3 ct;

		if(!upA) {
			if(dispA >= 5)
				upA = true;
			else {
				dispA++;
				ct.y = fishes[i].getPos().y + 0.02;
			}
		} else if(upA) {
				if(dispA <= 0)
					upA = false;
				else {
					dispA--;
					ct.y = fishes[i].getPos().y - 0.02;
				}
		}

		ct.x = fishes[i].getPos().x + 0.1*sin(0);
		ct.z = fishes[i].getPos().z + 0.1*cos(0);
		fishes[i].setPos(ct);


		if(!upA) {
			if(dispA >= 5)
				upA = true;
			else {
				dispA++;
				ct.y = sharks[i].getPos().y + 0.02;
			}
		} else if(upA) {
				if(dispA <= 0)
					upA = false;
				else {
					dispA--;
					ct.y = sharks[i].getPos().y - 0.02;
				}
		}

		ct.x = sharks[i].getPos().x + 0.1*sin(4.71239);
		ct.z = sharks[i].getPos().z + 0.1*cos(4.71239);
		sharks[i].setPos(ct);
	}
	glutTimerFunc(50, subAnimalsAnimation, x);
	glutPostRedisplay();
}

void Game::shipAnimation(const int x) {
	for(long unsigned int i = 0; i < ships.size(); i++) {
		GLdb3 ct;
		if(!upShip) {
			if(dispShip >= 5)
				upShip = true;
			else {
				dispShip++;
				ct.y = ships[i].getPos().y + 0.01;
			}
		} else if(upShip) {
				if(dispShip <= 0)
					upShip = false;
				else {
					dispShip--;
					ct.y = ships[i].getPos().y - 0.01;
				}
		}
		ct.x = ships[i].getPos().x + 0.5*sin(4.71239);
		ct.z = ships[i].getPos().z + 0.5*cos(4.71239);
		ct.y = ships[i].getPos().y;
		ships[i].setPos(ct);
	}
	glutTimerFunc(100, shipAnimation, x);
	glutPostRedisplay();
}

void Game::drawText(const GLdb3 pos, const string text) {
	const GLdouble scale = 0.005;
	const GLdouble radRotation = submarine.getRot()*(M_PI/180.0);
	const unsigned char* t = (unsigned char*)text.c_str();

	glPushMatrix();
	glLineWidth(2);
	glColor3f(1.0, 0.0, 0.0);
	glTranslated(pos.x, pos.y+16, pos.z);
	glScaled(scale, scale, scale);
	glRotated(-(radRotation*180)/M_PI, 0.0, 1.0, 0.0);
	glutStrokeString(GLUT_STROKE_MONO_ROMAN, t);
	glPopMatrix();
}

void Game::drawHelpMenu() {
	//	Submarine position
	GLdb3 pos;
	pos.x = center[0];
	pos.y = center[1];
	pos.z = center[2];

	drawText(pos," W para acelerar");
	pos.y -= 1;
	drawText(pos," S para re");
	pos.y -= 1;
	drawText(pos, " Cima para emergir");
	pos.y -= 1;
	drawText(pos, " Baixo para imergir");
	pos.y -= 1;
	drawText(pos, " Esquerda para virar");
	pos.y -= 1;
	drawText(pos, " Direita para virar");
	pos.y -= 1;
	drawText(pos, " F ponto de vista fora");
	pos.y -= 1;
	drawText(pos, " I ponto de vista dentro");
}