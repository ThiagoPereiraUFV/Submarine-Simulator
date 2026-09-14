#include "GlutRenderer.h"

#include <GL/freeglut.h>

InputHandler* GlutRenderer::current_ = nullptr;

namespace {

SpecialKey toSpecialKey(int glutKey) {
	switch (glutKey) {
	case GLUT_KEY_UP:
		return SpecialKey::Up;
	case GLUT_KEY_DOWN:
		return SpecialKey::Down;
	case GLUT_KEY_LEFT:
		return SpecialKey::Left;
	case GLUT_KEY_RIGHT:
		return SpecialKey::Right;
	case GLUT_KEY_F11:
		return SpecialKey::F11;
	default:
		return SpecialKey::Other;
	}
}

} // namespace

Renderer::WindowSize GlutRenderer::createWindow(int argc, char** argv) {
	glutInit(&argc, argv);

	const int width = static_cast<int>(glutGet(GLUT_SCREEN_WIDTH) * 0.6);
	const int height = static_cast<int>(glutGet(GLUT_SCREEN_HEIGHT) * 0.6);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_MULTISAMPLE);
	glutInitWindowSize(width, height);
	glutInitWindowPosition((glutGet(GLUT_SCREEN_WIDTH) - width) / 2,
						   (glutGet(GLUT_SCREEN_HEIGHT) - height) / 2);
	glutCreateWindow("Submarine simulator by Thiago Pereira");
	// Without this, glutLeaveMainLoop() (quit()) would not make glutMainLoop() return.
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_CONTINUE_EXECUTION);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LINE_SMOOTH);
	glHint(GL_LINE_SMOOTH, GL_NICEST);
	glEnable(GL_POINT_SMOOTH);
	glHint(GL_POINT_SMOOTH, GL_NICEST);
	glEnable(GL_POLYGON_SMOOTH);
	glShadeModel(GL_SMOOTH);
	glutSetOption(GLUT_MULTISAMPLE, 4);
	glEnable(GL_MULTISAMPLE);
	glutIgnoreKeyRepeat(true);

	const GLfloat sunlight[] = {0.8f, 0.8f, 0.8f, 1.0f};
	const GLfloat spotlight[] = {0.8f, 0.8f, 0.8f, 1.0f};
	glLightfv(GL_LIGHT0, GL_AMBIENT, sunlight);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, spotlight);

	return {width, height};
}

void GlutRenderer::run(InputHandler& handler) {
	current_ = &handler;
	glutDisplayFunc(displayCallback);
	glutReshapeFunc(reshapeCallback);
	glutSpecialFunc(specialDownCallback);
	glutSpecialUpFunc(specialUpCallback);
	glutKeyboardFunc(keyDownCallback);
	glutKeyboardUpFunc(keyUpCallback);
	glutIdleFunc(displayCallback);
	glutMainLoop();
}

void GlutRenderer::beginFrame(const FrameSetup& setup) {
	glClearColor(setup.background.r, setup.background.g, setup.background.b, setup.background.a);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();
	gluLookAt(setup.eye.x, setup.eye.y, setup.eye.z, setup.target.x, setup.target.y, setup.target.z,
			  0.0, 1.0, 0.0);

	setup.lighting ? glEnable(GL_LIGHTING) : glDisable(GL_LIGHTING);
	setup.light0 ? glEnable(GL_LIGHT0) : glDisable(GL_LIGHT0);
	setup.light1 ? glEnable(GL_LIGHT1) : glDisable(GL_LIGHT1);
	glShadeModel(setup.smoothShading ? GL_SMOOTH : GL_FLAT);

	const GLfloat spotPos[] = {static_cast<GLfloat>(setup.spotlightPos.x),
							   static_cast<GLfloat>(setup.spotlightPos.y),
							   static_cast<GLfloat>(setup.spotlightPos.z), 0.0f};
	const GLfloat spotDir[] = {static_cast<GLfloat>(setup.spotlightDir.x),
							   static_cast<GLfloat>(setup.spotlightDir.y),
							   static_cast<GLfloat>(setup.spotlightDir.z)};
	glLightfv(GL_LIGHT1, GL_POSITION, spotPos);
	glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, spotDir);

	glTranslated(setup.pivot.x, setup.pivot.y, setup.pivot.z);
	glRotated(setup.worldRotationDeg, 0.0, 1.0, 0.0);
	glTranslated(-setup.pivot.x, -setup.pivot.y, -setup.pivot.z);
}

void GlutRenderer::drawSun(Vec3 pos, double radius) {
	glColor3f(1.0f, 0.9f, 0.0f);
	glPushMatrix();
	glTranslated(pos.x, pos.y, pos.z);
	glutSolidSphere(radius, 100, 8);
	glPopMatrix();

	const GLfloat lightPos[] = {static_cast<GLfloat>(pos.x), static_cast<GLfloat>(pos.y),
								static_cast<GLfloat>(pos.z), -1.0f};
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
}

void GlutRenderer::drawSeaFace(const Vec3 corners[4], Vec3 offset) {
	glColor4f(0.0f, 0.0f, 1.0f, 0.9f);
	glPushMatrix();
	glTranslated(offset.x, offset.y, offset.z);
	glScaled(3.83, 1.0, 3.83);
	glBegin(GL_QUADS);
	for (int i = 0; i < 4; ++i)
		glVertex3d(corners[i].x, corners[i].y, corners[i].z);
	glEnd();
	glPopMatrix();
}

void GlutRenderer::drawMesh(const Object3D& object) {
	const Vec3 pos = object.position();
	const Vec3 color = object.color();
	const std::vector<Vec3>& vertices = object.vertices();
	const std::vector<Vec3>& normals = object.normals();
	const std::vector<Vec3>& texcoords = object.texcoords();
	const std::vector<Face>& faces = object.faces();
	const std::vector<Face>& faceNormals = object.faceNormals();
	const std::vector<Face>& faceTexcoords = object.faceTexcoords();

	const bool hasNormals = !normals.empty() && !faceNormals.empty();
	const bool hasTexcoords = !texcoords.empty() && !faceTexcoords.empty();

	glPushMatrix();
	glColor3d(color.x, color.y, color.z);
	glTranslated(pos.x, pos.y, pos.z);
	glRotated(-object.rotY(), 0.0, 1.0, 0.0);
	glRotated(-object.rotZ(), 0.0, 0.0, 1.0);

	for (std::size_t i = 0; i < faces.size(); ++i) {
		const int vertexRefs[3] = {faces[i].a, faces[i].b, faces[i].c};
		const Face* normalRefs = hasNormals ? &faceNormals[i] : nullptr;
		const Face* texcoordRefs = hasTexcoords ? &faceTexcoords[i] : nullptr;

		glBegin(GL_TRIANGLES);
		for (int v = 0; v < 3; ++v) {
			// Emitted before glVertex, unlike the original (which emitted them
			// after all 3 vertices, lighting each triangle with the previous
			// triangle's normal).
			if (normalRefs) {
				const int n = (v == 0) ? normalRefs->a : (v == 1) ? normalRefs->b : normalRefs->c;
				glNormal3d(normals[n].x, normals[n].y, normals[n].z);
			}
			if (texcoordRefs) {
				const int t = (v == 0)	 ? texcoordRefs->a
							  : (v == 1) ? texcoordRefs->b
										 : texcoordRefs->c;
				glTexCoord2d(texcoords[t].x, texcoords[t].y);
			}
			glVertex3d(vertices[vertexRefs[v]].x, vertices[vertexRefs[v]].y,
					   vertices[vertexRefs[v]].z);
		}
		glEnd();
	}

	glPopMatrix();
}

void GlutRenderer::drawText(Vec3 pos, double rotYDeg, const std::string& text) {
	glPushMatrix();
	glLineWidth(2);
	glColor3f(1.0f, 0.0f, 0.0f);
	glTranslated(pos.x, pos.y + 16, pos.z);
	glScaled(0.005, 0.005, 0.005);
	glRotated(-rotYDeg, 0.0, 1.0, 0.0);
	glutStrokeString(GLUT_STROKE_MONO_ROMAN, reinterpret_cast<const unsigned char*>(text.c_str()));
	glPopMatrix();
}

void GlutRenderer::endFrame() {
	glFlush();
	glutSwapBuffers();
}

void GlutRenderer::scheduleTimer(TimerId id, int millis) {
	glutTimerFunc(millis, timerCallback, static_cast<int>(id));
}

void GlutRenderer::toggleFullscreen() {
	glutFullScreenToggle();
}

void GlutRenderer::quit() {
	glutLeaveMainLoop();
}

void GlutRenderer::displayCallback() {
	if (current_)
		current_->onDisplay();
}

void GlutRenderer::reshapeCallback(int width, int height) {
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(80.0, static_cast<double>(width) / height, 0.5, height * 6.0);
	glMatrixMode(GL_MODELVIEW);
	if (current_)
		current_->onResize(width, height);
	glutPostRedisplay();
}

void GlutRenderer::specialDownCallback(int key, int, int) {
	if (current_)
		current_->onSpecialKeyDown(toSpecialKey(key));
}

void GlutRenderer::specialUpCallback(int key, int, int) {
	if (current_)
		current_->onSpecialKeyUp(toSpecialKey(key));
}

void GlutRenderer::keyDownCallback(unsigned char key, int, int) {
	if (current_)
		current_->onKeyDown(key);
}

void GlutRenderer::keyUpCallback(unsigned char key, int, int) {
	if (current_)
		current_->onKeyUp(key);
}

void GlutRenderer::timerCallback(int value) {
	if (current_)
		current_->onTimer(static_cast<TimerId>(value));
}
