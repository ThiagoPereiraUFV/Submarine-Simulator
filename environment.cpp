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

void drawFaceSea(const GLint a, const GLint b, const GLint c, const GLint d) {
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

void drawSea() {
	drawFaceSea(1, 2, 3, 0); 
	drawFaceSea(6, 7, 3, 2);
	drawFaceSea(3, 7, 4, 0);
	drawFaceSea(5, 6, 2, 1); 
	drawFaceSea(7, 6, 5, 4); 
	drawFaceSea(4, 5, 1, 0);
}

void drawSun() {
	const GLfloat cRadiusSun = (!l1 && light) ? 0.01 : 0.02, cXSun = 0.09, cYSun = 0.08, cZSun = 0.18;
	glColor3f(1.0, 0.9, 0.0);
	glPushMatrix();
		glTranslatef(viewer[0] + cXSun*view_h*3, cYSun*view_h*2, viewer[2] - cZSun*view_h*3);
		glutSolidSphere(cRadiusSun*view_h, 100, 8);
	glPopMatrix();
	const vector<GLfloat> light_position{(GLfloat)viewer[0] + cXSun*view_h*3, cYSun*view_h*2, (GLfloat)viewer[2] - cZSun*view_h*3, 0.0};
    glLightfv(GL_LIGHT0, GL_POSITION, &light_position[0]);
}

void subAnimation(const int x) {
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
	submarino.setPos(ct);
	glutTimerFunc(250, subAnimation, x);
	glutPostRedisplay();
}

void subAnimalsAnimation(const int x) {
	for(int i = 0; i < peixe.size() && i < tubarao.size(); i++) {
		GLdb3 ct;

		if(!upA) {
			if(dispA >= 5)
				upA = true;
			else {
				dispA++;
				ct.y = peixe[i].getPos().y + 0.02;
			}
		} else if(upA) {
				if(dispA <= 0)
					upA = false;
				else {
					dispA--;
					ct.y = peixe[i].getPos().y - 0.02;
				}
		}
		
		ct.x = peixe[i].getPos().x + 0.1*sin(0);
		ct.z = peixe[i].getPos().z + 0.1*cos(0);
		peixe[i].setPos(ct);


		if(!upA) {
			if(dispA >= 5)
				upA = true;
			else {
				dispA++;
				ct.y = tubarao[i].getPos().y + 0.02;
			}
		} else if(upA) {
				if(dispA <= 0)
					upA = false;
				else {
					dispA--;
					ct.y = tubarao[i].getPos().y - 0.02;
				}
		}
		
		ct.x = tubarao[i].getPos().x + 0.1*sin(4.71239);
		ct.z = tubarao[i].getPos().z + 0.1*cos(4.71239);
		tubarao[i].setPos(ct);
	}
	glutTimerFunc(50, subAnimalsAnimation, x);
	glutPostRedisplay();
}

void shipAnimation(const int x) {
	for(int i = 0; i < navio.size(); i++) {
		GLdb3 ct;
		if(!upShip) {
			if(dispShip >= 5)
				upShip = true;
			else {
				dispShip++;
				ct.y = navio[i].getPos().y + 0.01;
			}
		} else if(upShip) {
				if(dispShip <= 0)
					upShip = false;
				else {
					dispShip--;
					ct.y = navio[i].getPos().y - 0.01;
				}
		}
		ct.x = navio[i].getPos().x + 0.5*sin(4.71239);
		ct.z = navio[i].getPos().z + 0.5*cos(4.71239);
		navio[i].setPos(ct);
	}
	glutTimerFunc(100, shipAnimation, x);
	glutPostRedisplay();
}

void drawText(const GLdb3 pos,const string text) {
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

void drawHelpMenu() {
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