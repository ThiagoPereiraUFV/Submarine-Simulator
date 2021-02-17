#include "Game.h"

GLsizei Game::view_w, Game::view_h;
bool Game::started = false, Game::started1 = false, Game::help = false, Game::fp = false;
bool Game::light = true, Game::lightMode = false, Game::l1 = true, Game::l2 = true;
const vector<GLfloat> Game::spotlight{0.8, 0.8, 0.8, 1.0};
const vector<GLfloat> Game::sunlight{0.8, 0.8, 0.8, 1.0};
vector<GLdouble> Game::viewer, Game::center;
bool Game::upSub = true, Game::upA = true, Game::upShip = true;
int Game::dispSub = 0, Game::dispA = 0, Game::dispShip = 0;
vector<GLdb3> Game::fishesPos, Game::sharksPos, Game::helisPos, Game::shipsPos;
Object3D Game::submarine;
GLdouble Game::rotation;
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
	glutCreateWindow("Submarine simulator by Wallace e Thiago");
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
	glLightfv(GL_LIGHT0, GL_AMBIENT, &sunlight[0]);	//	Sun
	glLightfv(GL_LIGHT1, GL_DIFFUSE, &spotlight[0]);	//	Spotlight

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
		const int dist = 400;
		fishes.push_back(parserOBJ::parse("models/fish1.obj"));

		pos.x = (rand() % 2 == 0) ? -(rand() % dist) : rand() % dist;
		pos.z = (rand() % 2 == 0) ? -(rand() % dist) : rand() % dist;
		pos.y = -(rand() % dist);
		fishesPos.push_back(pos);

		color.x = (GLdouble)(rand() % 255) / (GLdouble)255;
		color.y = (GLdouble)(rand() % 255) / (GLdouble)255;
		color.z = (GLdouble)(rand() % 255) / (GLdouble)255;
		fishes[i].setColor(color);

		//	Shark
		sharks.push_back(parserOBJ::parse("models/shark1.obj"));

		pos.x = (rand() % 2 == 0) ? -(rand() % dist) : rand() % dist;
		pos.z = (rand() % 2 == 0) ? -(rand() % dist) : rand() % dist;
		pos.y = -(rand() % dist) - 5;
		sharksPos.push_back(pos);

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
		pos.y = 0.0;
		shipsPos.push_back(pos);

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
		pos.y = rand() % 100 + 20;
		helisPos.push_back(pos);
	}
}

void Game::updateVariables(const GLsizei w, const GLsizei h) {
	view_w = w;
	view_h = h;
	verticesSea = {{-c3 * view_h, -c3 * view_h, c3 * view_h},
	               {-c3 * view_h, c3 * view_h, c3 * view_h},
	               {c3 * view_h, c3 * view_h, c3 * view_h},
	               {c3 * view_h, -c3 * view_h, c3 * view_h},
	               {-c3 * view_h, -c3 * view_h, -c3 * view_h},
	               {-c3 * view_h, c3 * view_h, -c3 * view_h},
	               {c3 * view_h, c3 * view_h, -c3 * view_h},
	               {c3 * view_h, -c3 * view_h, -c3 * view_h}};
	if(!started) {
		viewer = {0.0, 2.5 * c1 * view_h + 10, 5.0 * c1 * view_h + 10};
		GLdb3 pos = submarine.getPos();
		center = {pos.x, pos.y, pos.z};
		started = true;
		subAnimation(0);
		subAnimalsAnimation(0);
		shipAnimation(0);
	}
}

//	Resize game elements to keep ratio
void Game::reshape(const GLsizei w, const GLsizei h) {
	//	Ajustando modo de exibicao dos objetos na tela
	updateVariables(w, h);
	glViewport(0, 0, w, h);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(70.0, (GLfloat)view_w / (GLfloat)view_h, 2.0, view_h * 2);  // projecao perspectiva
	glMatrixMode(GL_MODELVIEW);
	glutPostRedisplay();
}

//	Render game elements
void Game::display() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  // limpa a janela
	glLoadIdentity();
	gluLookAt(
	    viewer[0], (fp) ? viewer[1] - 5 : viewer[1],
	    (fp) ? viewer[2] - 22 : viewer[2],  // define posicao do observador
	    center[0], (fp) ? center[1] + 5 : center[1] + 10,
	    (fp) ? center[2] - 22 : center[2] + 2,  // ponto de interesse (foco)
	    0.0, 1.0, 0.0);                         // vetor de "view up"

	GLdb3 deslocamento = submarine.getPos();
	GLdouble rot = submarine.getRot();

	const vector<GLfloat> lPosition = {(GLfloat)viewer[0], (GLfloat)viewer[1], (GLfloat)viewer[2], 1.0};
	const vector<GLfloat> lDirection = {(GLfloat)center[0], (GLfloat)center[1], (GLfloat)center[2]};
	glLightfv(GL_LIGHT1, GL_POSITION, &lPosition[0]);  //	Luz direcionada
	glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, &lDirection[0]);  //	Luz direcionada

	glTranslated(deslocamento.x, deslocamento.y, deslocamento.z);
	glRotated(rot, 0.0, 1.0, 0.0);
	glTranslated(-deslocamento.x, -deslocamento.y, -deslocamento.z);
	drawSun();
	// drawAxes();	//	Desenha os eixos XYZ
	drawSea(); /*
	 glColor3d(1.0, 1.0, 0.0);
	 glPushMatrix();
	     glutSolidSphere(30, 100, 100);
	 glPopMatrix();*/

	glPushMatrix();
	for(int i = 0; i < ships.size(); i++) {
		if(!started1)
			ships[i].setPos(shipsPos[i]);
		glPushMatrix();
		ships[i].justDraw();
		glPopMatrix();
	}
	glPopMatrix();

	for(int i = 0; i < helis.size(); i++) {
		if(!started1)
			helis[i].setPos(helisPos[i]);
		helis[i].draw();
	}

	for(int i = 0; i < fishes.size() && i < sharks.size(); i++) {
		if(!started1)
			fishes[i].setPos(fishesPos[i]);
		fishes[i].draw();
		if(!started1)
			sharks[i].setPos(sharksPos[i]);
		sharks[i].draw();
	}

	glPushMatrix();

	submarine.justDraw();

	glPopMatrix();

	if(help)
		drawHelpMenu();

	if(!started1)
		started1 = true;

	glFlush();
	glutSwapBuffers();  // usando double buffer (para evitar 'flicker')
}

//	Handle arrow keys events
void Game::SpecialKeys(const int key, const int x, const int y) {
	GLdouble rot = submarine.getRot();
	switch(key) {
		case GLUT_KEY_UP:
			if(center[1] < 0) {
				viewer[1] += 1.0;
				center[1] += 1.0;
			}
			break;
		case GLUT_KEY_DOWN:
			if(center[1] >= -180.0) {
				viewer[1] -= 1.0;
				center[1] -= 1.0;
			}
			break;
		case GLUT_KEY_RIGHT:
			rotation += 0.0872665;
			if(rotation >= 2 * 3.1415)
				rotation = 0;

			rot += 5;
			if(rot >= 360)
				rot = 0;
			break;
		case GLUT_KEY_LEFT:
			rotation -= 0.0872665;
			if(rotation <= 0)
				rotation = 2 * PI;

			rot -= 5;
			if(rot <= 0)
				rot = 360;
			break;
	}
	GLdb3 ct;
	ct.x = center[0];
	ct.y = center[1];
	ct.z = center[2];
	submarine.setRot(rot);
	submarine.setPos(ct);
	glutPostRedisplay();
}

//	Handle enter, esc e spacebar keys events
void Game::HandleKeyboard(const unsigned char key, const int x, const int y) {
	double cos_rot = cos(rotation), sin_rot = sin(rotation);
	switch(key) {
		case 'W':
		case 'w':
			viewer[2] = -1.0 * cos_rot + viewer[2];
			center[2] = -1.0 * cos_rot + center[2];
			viewer[0] = 1.0 * sin_rot + viewer[0];
			center[0] = 1.0 * sin_rot + center[0];
			break;
		case 'S':
		case 's':
			viewer[2] = viewer[2] + 1.0 * cos_rot;
			center[2] = center[2] + 1.0 * cos_rot;
			viewer[0] = viewer[0] - 1.0 * sin_rot;
			center[0] = center[0] - 1.0 * sin_rot;
			break;
		case 'H':
		case 'h':
			if(help)
				help = false;
			else
				help = true;
			break;
		case 'F':
		case 'f':
			fp = false;
			break;
		case 'I':
		case 'i':
			if(fp)
				fp = false;
			else
				fp = true;
			break;
		case 'L':
		case 'l':
			if(light) {
				glDisable(GL_LIGHTING);
				light = false;
				glEnable(GL_LIGHT0);
				glEnable(GL_LIGHT1);
				l1 = l2 = true;
				glClearColor(0.0, (GLfloat)227 / (GLfloat)255, 1.0, 1.0);
			} else {
				glEnable(GL_LIGHTING);
				light = true;
			}
			break;
		case 'G':
		case 'g':
			if(lightMode) {
				glShadeModel(GL_SMOOTH);
				lightMode = false;
			} else {
				glShadeModel(GL_FLAT);
				lightMode = true;
			}
			break;
		case 49:
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
		case 50:
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

void drawAxes() {
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
		glTranslatef(viewer[0], -c3*view_h, viewer[2]);
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
	const vector<GLfloat> light_position{(GLfloat)viewer[0] + cXSun*view_h*3, cYSun*view_h*2, (GLfloat)viewer[2] - cZSun*view_h*3, 0.0};
    glLightfv(GL_LIGHT0, GL_POSITION, &light_position[0]);
}

void Game::subAnimation(const int x) {
	if(!upSub) {
		if(dispSub >= 5)
			upSub = true;
		else {
			dispSub++;
			center[1] += 0.1;
		}
	} else if(upSub) {
			if(dispSub <= 0)
				upSub = false;
			else {
				dispSub--;
				center[1] -= 0.1;
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
	for(int i = 0; i < fishes.size() && i < sharks.size(); i++) {
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
	for(int i = 0; i < ships.size(); i++) {
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
		ships[i].setPos(ct);
	}
	glutTimerFunc(100, shipAnimation, x);
	glutPostRedisplay();
}

void Game::drawText(const GLdb3 pos,const string text) {
	glPointSize(1);
	glLineWidth(2);
	glColor3f(1.0, 0.0, 0.0);
	glPushMatrix();
		glTranslated(pos.x, pos.y+16, pos.z);
		glScaled(0.005, 0.005, 0.005);
		glRotated(-(rotation*180)/PI, 0.0, 1.0, 0.0);
		for(int i = 0; i < text.length(); ++i) {
			glutStrokeCharacter(GLUT_STROKE_MONO_ROMAN, text[i]);
		}
    glPopMatrix();
}

void Game::drawHelpMenu() {
	GLdb3 pos;

	//	Menu acelerar e frear
	pos.x = center[0];
	pos.y = center[1];
	pos.z = center[2];
	drawText(pos," W para acelerar");
	pos.y -= 1;
	drawText(pos," S para re");

	//	Menu emergir e imergir
	pos.y -= 1;
	drawText(pos, " Cima para emergir");
	pos.y -= 1;
	drawText(pos, " Baixo para imergir");

	//	Menu virar submarino
	pos.y -= 1;
	drawText(pos, " Esquerda para virar");
	pos.y -= 1;
	drawText(pos, " Direita para virar");
	pos.y -= 1;
	drawText(pos, " F ponto de vista fora");
	pos.y -= 1;
	drawText(pos, " I ponto de vista dentro");
}