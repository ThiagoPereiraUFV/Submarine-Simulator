#include <GL/gl.h>
#include <GL/glut.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cmath>
#include "include/object3D.h"
#include "include/parser.h"
#include "include/globalVariables.cpp"
#include "include/environment.cpp"

// biblioteca para auxilio no carregamento de imagens
#define STB_IMAGE_IMPLEMENTATION
#include "include/stb_image.h"

unsigned int *texture = new unsigned int[2];
GLUquadricObj *sphere = gluNewQuadric();

struct Image {
	int sizeX;
	int sizeY;
	int nrChannels;
	unsigned char *data;
};

float xrot;
float yrot;
float zrot;
float ratio;

typedef struct Image Image;

#define checkImageWidth 64
#define checkImageHeight 64

int ImageLoad(const char *filename, Image *image) {
	FILE *file;
	unsigned size;              // size of the image in bytes.
	unsigned long i;            // standard counter.
	unsigned short int planes;  // number of planes in image (must be 1)
	unsigned short int bpp;     // number of bits per pixel (must be 24)

	char temp;  // temporary color storage for bgr-rgb conversion.
	// make sure the file is there.

	if((file = fopen(filename, "rb")) == NULL) {
		printf("File Not Found : %s\n", filename);
		return 0;
	}

	// seek through the bmp header, up to the width/height:
	fseek(file, 18, SEEK_CUR);

	// read the width
	if((i = fread(&image->sizeX, 4, 1, file)) != 1) {
		printf("Error reading width from %s.\n", filename);
		return 0;
	}
	// printf("Width of %s: %lu\n", filename, image->sizeX);

	// read the height
	if((i = fread(&image->sizeY, 4, 1, file)) != 1) {
		printf("Error reading height from %s.\n", filename);
		return 0;
	}
	// printf("Height of %s: %lu\n", filename, image->sizeY);
	// calculate the size (assuming 24 bits or 3 bytes per pixel).

	size = image->sizeX * image->sizeY * 3;
	// read the planes
	if((fread(&planes, 2, 1, file)) != 1) {
		printf("Error reading planes from %s.\n", filename);
		return 0;
	}

	if(planes != 1) {
		printf("Planes from %s is not 1: %u\n", filename, planes);
		return 0;
	}

	// read the bitsperpixel

	if((i = fread(&bpp, 2, 1, file)) != 1) {
		printf("Error reading bpp from %s.\n", filename);
		return 0;
	}

	if(bpp != 24) {
		printf("Bpp from %s is not 24: %u\n", filename, bpp);
		return 0;
	}
	// seek past the rest of the bitmap header.

	fseek(file, 24, SEEK_CUR);
	image->nrChannels = 3;
	// read the data.
	image->data = stbi_load(filename, &(image->sizeX), &(image->sizeY),
	                        &(image->nrChannels), 0);
	if(image->data == NULL) {
		printf("Error allocating memory for color-corrected image data\n");
		return 0;
	}
	if((i = fread(image->data, size, 1, file)) != 1) {
		printf("Error reading image data from %s.\n", filename);
		return 0;
	}
	/* for (i=0;i<size;i+=3) { // reverse all of the colors. (bgr -> rgb)
	    temp = image->data[i];
	    image->data[i] = image->data[i+2];
	    image->data[i+2] = temp;
	} */
	// we're done.
	return 1;
}

Image *loadTexture(const char *file_name) {
	Image *image_aux;
	// allocate space for texture
	image_aux = (Image *)malloc(sizeof(Image));
	if(image_aux == NULL) {
		printf("Error allocating space for image");
		exit(0);
	}

	if(!ImageLoad(file_name, image_aux)) {
		exit(1);
	}

	return image_aux;
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

void display() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  // limpa a janela
	glLoadIdentity();
	gluLookAt(
	    viewer[0], (fp) ? viewer[1] - 5 : viewer[1],
	    (fp) ? viewer[2] - 22 : viewer[2],  // define posicao do observador
	    center[0], (fp) ? center[1] + 5 : center[1] + 10,
	    (fp) ? center[2] - 22 : center[2] + 2,  // ponto de interesse (foco)
	    0.0, 1.0, 0.0);                         // vetor de "view up"

	GLdb3 deslocamento = submarino.getPos();
	GLdouble rot = submarino.getRot();

	lPosition = {(GLfloat)viewer[0], (GLfloat)viewer[1], (GLfloat)viewer[2],
	             1.0};
	lDirection = {(GLfloat)center[0], (GLfloat)center[1], (GLfloat)center[2]};
	glLightfv(GL_LIGHT1, GL_POSITION, &lPosition[0]);  //	Luz direcionada
	glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION,
	          &lDirection[0]);  //	Luz direcionada

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

	glEnable(GL_TEXTURE_2D);
	glPushMatrix();
	for(int i = 0; i < navio.size(); i++) {
		// glEnable(GL_TEXTURE_2D);
		if(!started1)
			navio[i].setPos(shipPos[i]);
		glPushMatrix();
		if(i % 2 == 0) {
			glBindTexture(GL_TEXTURE_2D, texture[0]);
			gluQuadricTexture(sphere, true);
		} else
			glBindTexture(GL_TEXTURE_2D, texture[1]);
		navio[i].justDraw();
		glPopMatrix();
	}
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);

	for(int i = 0; i < helicoptero.size(); i++) {
		helicoptero[i].setPos(heliPos[i]);
		helicoptero[i].draw();
	}

	for(int i = 0; i < peixe.size() && i < tubarao.size(); i++) {
		if(!started1)
			peixe[i].setPos(fishPos[i]);
		peixe[i].draw();
		if(!started1)
			tubarao[i].setPos(sharkPos[i]);
		tubarao[i].draw();
	}

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture[0]);
	glPushMatrix();

	gluQuadricTexture(sphere,
	                  true);  // gera texturas para as superficies quadricas...

	submarino.justDraw();

	glPopMatrix();
	glDisable(GL_TEXTURE_2D);

	if(help)
		drawHelpMenu();

	if(!started1)
		started1 = true;

	glFlush();
	glutSwapBuffers();  // usando double buffer (para evitar 'flicker')
}

void keyboard(const unsigned char key, const int x, const int y) {
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
	submarino.setPos(ct);
	glutPostRedisplay();
}

void SpecialKeys(const int key, const int x, const int y) {
	GLdouble rot = submarino.getRot();
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
	submarino.setRot(rot);
	submarino.setPos(ct);
	glutPostRedisplay();
}

void reshape(const GLsizei w, const GLsizei h) {
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

void loadTexture() {
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	string file_name = "models/texture/navy.bmp";
	Image *image1 = loadTexture(file_name.c_str());

	if(image1 == NULL) {
		printf("Image was not returned from loadTexture\n");
		exit(0);
	}
	//    makeCheckImage();
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	// Definindo texturas
	glGenTextures(2, texture);  // define o numero de texturas

	// Primeira textura
	glBindTexture(GL_TEXTURE_2D, texture[0]);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, image1->sizeX, image1->sizeY, 0, GL_RGB,
	             GL_UNSIGNED_BYTE, image1->data);

	stbi_image_free(image1->data);
	delete image1;

	// carregando segunda textura
	file_name = "models/texture/ship.bmp";
	image1 = loadTexture(file_name.c_str());

	if(image1 == NULL) {
		printf("Image was not returned from loadTexture\n");
		exit(0);
	}

	// Segunda textura
	glBindTexture(GL_TEXTURE_2D, texture[1]);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, image1->sizeX, image1->sizeY, 0, GL_RGB,
	             GL_UNSIGNED_BYTE, image1->data);

	stbi_image_free(image1->data);
	delete image1;

	glEnable(GL_TEXTURE_2D);
}

void init() {
	//	Ajustando configuracoes de janela e exibicao
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutInitWindowPosition((glutGet(GLUT_SCREEN_WIDTH) - WINDOW_WIDTH) / 2,
	                       (glutGet(GLUT_SCREEN_HEIGHT) - WINDOW_HEIGHT) /
	                           2);  // posicao da janela
	glutCreateWindow("Simulador de submarino by Wallace e Thiago");
	glClearColor(0.0, (GLfloat)227 / (GLfloat)255, 1.0, 1.0);

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

	loadTexture();

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

int main(int argc, char **argv) {
	glutInit(&argc, argv);
	init();
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(SpecialKeys);
	glutIdleFunc(display);
	glutMainLoop();

	delete texture;
	return 0;
}
