#include "Game.h"

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
	glutCreateWindow("Simulador de submarino by Wallace e Thiago");
	glClearColor(0.0, (GLfloat)227 / (GLfloat)255, 1.0, 1.0);
	//glutFullScreen();

	//	Ativando e inicializando funcionalidades
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_DEPTH_TEST);
	srand(time(NULL));

	//	Configurando luzes
	glLightfv(GL_LIGHT0, GL_AMBIENT, &sunlight[0]);   //	Sol
	glLightfv(GL_LIGHT1, GL_DIFFUSE, &spotlight[0]);  //	Luz direcionada

	//	Carregando modelos
	submarino = parserOBJ::parse("models/submarine.obj");
	GLdb3 color, pos;
	color.x = (GLdouble)(30) / (GLdouble)255;
	color.y = (GLdouble)(50) / (GLdouble)255;
	color.z = (GLdouble)(20) / (GLdouble)255;
	submarino.setColor(color);
	for(int i = 0; i < nSeaAnimals; i++) {
		peixe.push_back(parserOBJ::parse("models/fish1.obj"));
		tubarao.push_back(parserOBJ::parse("models/shark1.obj"));
		const int dist = 200;
		if(rand() % 2 == 0)
			pos.x = -(rand() % dist);
		else
			pos.x = rand() % dist;

		if(rand() % 2 == 0)
			pos.z = -(rand() % dist);
		else
			pos.z = rand() % dist;

		pos.y = -(rand() % dist);
		fishPos.push_back(pos);
		color.x = (GLdouble)(rand() % 255) / (GLdouble)255;
		color.y = (GLdouble)(rand() % 255) / (GLdouble)255;
		color.z = (GLdouble)(rand() % 255) / (GLdouble)255;
		peixe[i].setColor(color);

		if(rand() % 2 == 0)
			pos.x = -(rand() % dist) - 5;
		else
			pos.x = rand() % dist;

		if(rand() % 2 == 0)
			pos.z = -(rand() % dist);
		else
			pos.z = rand() % dist;

		pos.y = -(rand() % dist) - 5;
		sharkPos.push_back(pos);
		color.x = (GLdouble)(rand() % 255) / (GLdouble)255;
		color.y = (GLdouble)(rand() % 255) / (GLdouble)255;
		color.z = (GLdouble)(rand() % 255) / (GLdouble)255;
		tubarao[i].setColor(color);
	}
	for(int i = 0; i < nShips; i++) {
		navio.push_back(parserOBJ::parse("models/ship1.obj"));
		const int dist = 1000;
		if(rand() % 2 == 0)
			pos.x = -(rand() % dist);
		else
			pos.x = rand() % dist;

		if(rand() % 2 == 0)
			pos.z = -(rand() % dist);
		else
			pos.z = rand() % dist;

		pos.y = 0.0;
		shipPos.push_back(pos);
		double c = rand() % 255;
		color.x = color.y = color.z = (GLdouble)c / (GLdouble)255;
		navio[i].setColor(color);
	}
	for(int i = 0; i < nHelis; i++) {
		helicoptero.push_back(parserOBJ::parse("models/helicopter.obj"));
		const int dist = 500;
		if(rand() % 2 == 0)
			pos.x = -(rand() % dist);
		else
			pos.x = rand() % dist;

		if(rand() % 2 == 0)
			pos.z = -(rand() % dist);
		else
			pos.z = rand() % dist;

		pos.y = rand() % 10 + 20;
		heliPos.push_back(pos);
	}
}

void updateVariables(const GLsizei w, const GLsizei h) {
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
		GLdb3 pos = submarino.getPos();
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
	gluPerspective(70.0, (GLfloat)view_w / (GLfloat)view_h, 2.0,
	               view_h * 2);  // projecao perspectiva
	glMatrixMode(GL_MODELVIEW);
	glutPostRedisplay();
}

//	Render game elements
void Game::display() {
	updateColors();

	switch(state) {
		case 0:
			Menu::setup(colors, color, option, selecN);
			Menu::renderMenu();
			break;
		case 1:
			srand(time(NULL));
			Play::setup(colors, color, option);
			state = 2;
			Play::setState(state);
			break;
		case 2:
			Play::renderGameFrame();
			state = Play::getState();
			break;
		case 3:
			usleep(3000000);
			state = 0;
			break;
		default:
			exit(0);
			break;
	}

	/*
	glColor3f(1.0, 0.0, 0.0);
	glBegin(GL_LINES);
		glVertex2f(0, view_h);
		glVertex2f(0, -view_h);
		glVertex2f(view_w, 0);
		glVertex2f(-view_w, 0);
	glEnd();
	*/

	glutSwapBuffers();
	glutPostRedisplay();
}

//	Handle arrow keys events
void Game::SpecialKeys(const int key, const int x, const int y) {
	if(state == 0) {
		switch(key) {
			case GLUT_KEY_UP:
				if(selecN["INICIAR"].second) {
					selec = selecN["SAIR"].first;
					selecN["SAIR"].second = 1;
					selecN["INICIAR"].second = 0;
				} else if(selecN["SAIR"].second) {
					selec = selecN["NORMAL2"].first;
					selecN["NORMAL2"].second = 1;
					selecN["SAIR"].second = 0;
				} else if(selecN["NORMAL2"].second || selecN["BEBADO"].second) {
					selec = selecN["CORES1"].first;
					selecN["CORES1"].second = 1;
					selecN["NORMAL2"].second = selecN["BEBADO"].second = 0;
				} else if(selecN["CORES1"].second || selecN["CORES2"].second ||
						  selecN["CORES3"].second) {
					selec = selecN["20x10"].first;
					selecN["20x10"].second = 1;
					selecN["CORES1"].second = selecN["CORES2"].second =
						selecN["CORES3"].second = 0;
				} else if(selecN["20x10"].second || selecN["30x15"].second ||
						  selecN["50x25"].second) {
					selec = selecN["NORMAL1"].first;
					selecN["NORMAL1"].second = 1;
					selecN["20x10"].second = selecN["30x15"].second =
						selecN["50x25"].second = 0;
				} else if(selecN["NORMAL1"].second || selecN["RAPIDO"].second ||
						  selecN["TURBO"].second) {
					selec = selecN["INICIAR"].first;
					selecN["INICIAR"].second = 1;
					selecN["NORMAL1"].second = selecN["RAPIDO"].second =
						selecN["TURBO"].second = 0;
				}
				break;
			case GLUT_KEY_DOWN:
				if(selecN["INICIAR"].second) {
					selec = selecN["NORMAL1"].first;
					selecN["NORMAL1"].second = 1;
					selecN["INICIAR"].second = 0;
				} else if(selecN["NORMAL1"].second || selecN["RAPIDO"].second ||
						  selecN["TURBO"].second) {
					selec = selecN["20x10"].first;
					selecN["20x10"].second = 1;
					selecN["NORMAL1"].second = selecN["RAPIDO"].second =
						selecN["TURBO"].second = 0;
				} else if(selecN["20x10"].second || selecN["30x15"].second ||
						  selecN["50x25"].second) {
					selec = selecN["CORES1"].first;
					selecN["CORES1"].second = 1;
					selecN["20x10"].second = selecN["30x15"].second =
						selecN["50x25"].second = 0;
				} else if(selecN["CORES1"].second || selecN["CORES2"].second ||
						  selecN["CORES3"].second) {
					selec = selecN["NORMAL2"].first;
					selecN["NORMAL2"].second = 1;
					selecN["CORES1"].second = selecN["CORES2"].second =
						selecN["CORES3"].second = 0;
				} else if(selecN["NORMAL2"].second || selecN["BEBADO"].second) {
					selec = selecN["SAIR"].first;
					selecN["SAIR"].second = 1;
					selecN["NORMAL2"].second = selecN["BEBADO"].second = 0;
				} else if(selecN["SAIR"].second) {
					selec = selecN["INICIAR"].first;
					selecN["INICIAR"].second = 1;
					selecN["SAIR"].second = 0;
				}
				break;
			case GLUT_KEY_RIGHT:
				if(selecN["NORMAL1"].second) {
					selec = selecN["RAPIDO"].first;
					selecN["RAPIDO"].second = 1;
					selecN["NORMAL1"].second = 0;
				} else if(selecN["RAPIDO"].second) {
					selec = selecN["TURBO"].first;
					selecN["TURBO"].second = 1;
					selecN["RAPIDO"].second = 0;
				} else if(selecN["TURBO"].second) {
					selec = selecN["NORMAL1"].first;
					selecN["NORMAL1"].second = 1;
					selecN["TURBO"].second = 0;
				}
				if(selecN["20x10"].second) {
					selec = selecN["30x15"].first;
					selecN["30x15"].second = 1;
					selecN["20x10"].second = 0;
				} else if(selecN["30x15"].second) {
					selec = selecN["50x25"].first;
					selecN["50x25"].second = 1;
					selecN["30x15"].second = 0;
				} else if(selecN["50x25"].second) {
					selec = selecN["20x10"].first;
					selecN["20x10"].second = 1;
					selecN["50x25"].second = 0;
				}
				if(selecN["CORES1"].second) {
					selec = selecN["CORES2"].first;
					selecN["CORES2"].second = 1;
					selecN["CORES1"].second = 0;
				} else if(selecN["CORES2"].second) {
					selec = selecN["CORES3"].first;
					selecN["CORES3"].second = 1;
					selecN["CORES2"].second = 0;
				} else if(selecN["CORES3"].second) {
					selec = selecN["CORES1"].first;
					selecN["CORES1"].second = 1;
					selecN["CORES3"].second = 0;
				}
				if(selecN["NORMAL2"].second) {
					selec = selecN["BEBADO"].first;
					selecN["BEBADO"].second = 1;
					selecN["NORMAL2"].second = 0;
				} else if(selecN["BEBADO"].second) {
					selec = selecN["NORMAL2"].first;
					selecN["NORMAL2"].second = 1;
					selecN["BEBADO"].second = 0;
				}
				break;
			case GLUT_KEY_LEFT:
				if(selecN["NORMAL1"].second) {
					selec = selecN["TURBO"].first;
					selecN["TURBO"].second = 1;
					selecN["NORMAL1"].second = 0;
				} else if(selecN["TURBO"].second) {
					selec = selecN["RAPIDO"].first;
					selecN["RAPIDO"].second = 1;
					selecN["TURBO"].second = 0;
				} else if(selecN["RAPIDO"].second) {
					selec = selecN["NORMAL1"].first;
					selecN["NORMAL1"].second = 1;
					selecN["RAPIDO"].second = 0;
				}
				if(selecN["20x10"].second) {
					selec = selecN["50x25"].first;
					selecN["50x25"].second = 1;
					selecN["20x10"].second = 0;
				} else if(selecN["50x25"].second) {
					selec = selecN["30x15"].first;
					selecN["30x15"].second = 1;
					selecN["50x25"].second = 0;
				} else if(selecN["30x15"].second) {
					selec = selecN["20x10"].first;
					selecN["20x10"].second = 1;
					selecN["30x15"].second = 0;
				}
				if(selecN["CORES1"].second) {
					selec = selecN["CORES3"].first;
					selecN["CORES3"].second = 1;
					selecN["CORES1"].second = 0;
				} else if(selecN["CORES3"].second) {
					selec = selecN["CORES2"].first;
					selecN["CORES2"].second = 1;
					selecN["CORES3"].second = 0;
				} else if(selecN["CORES2"].second) {
					selec = selecN["CORES1"].first;
					selecN["CORES1"].second = 1;
					selecN["CORES2"].second = 0;
				}
				if(selecN["NORMAL2"].second) {
					selec = selecN["BEBADO"].first;
					selecN["BEBADO"].second = 1;
					selecN["NORMAL2"].second = 0;
				} else if(selecN["BEBADO"].second) {
					selec = selecN["NORMAL2"].first;
					selecN["NORMAL2"].second = 1;
					selecN["BEBADO"].second = 0;
				}
				break;
		}
	} else if(state == 2) {
		switch(key) {
			case GLUT_KEY_LEFT:
				Play::setLastKey('l');
				break;
			case GLUT_KEY_RIGHT:
				Play::setLastKey('r');
				break;
			case GLUT_KEY_DOWN:
				Play::setLastKey('a');
				break;
		}
	}
}

//	Handle enter, esc e spacebar keys events
void Game::HandleKeyboard(const unsigned char key, const int x, const int y) {
	if(state == 0) {
		switch(key) {
			case 13:
				if(selecI[selec] == "NORMAL1" || selecI[selec] == "RAPIDO" || selecI[selec] == "TURBO") {
					option["NORMAL1"] = option["RAPIDO"] = option["TURBO"] = 0;
					option[selecI[selec]] = 1;
				} else if(selecI[selec] == "20x10" || selecI[selec] == "30x15" || selecI[selec] == "50x25") {
					option["20x10"] = option["30x15"] = option["50x25"] = 0;
					option[selecI[selec]] = 1;
				} else if(selecI[selec] == "CORES1" || selecI[selec] == "CORES2" || selecI[selec] == "CORES3") {
					option["CORES1"] = option["CORES2"] = option["CORES3"] = 0;
					option[selecI[selec]] = 1;
				} else if(selecI[selec] == "NORMAL2" || selecI[selec] == "BEBADO") {
					option["NORMAL2"] = option["BEBADO"] = 0;
					option[selecI[selec]] = 1;
				}
				if(selecI[selec] == "INICIAR")
					state = 1;
				if(selecI[selec] == "SAIR")
					state = 4;
				break;
			case 27:
				selecN[selecI[selec]].second = 0;
				selecN["SAIR"].second = 1;
				selec = selecN["SAIR"].first;
				break;
		}
	} else if(state == 2) {
		switch(key) {
			case 27:
				state = 0;
				break;
			case 32:
				Play::setLastKey('s');
				break;
		}
	}
}