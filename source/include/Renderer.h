#ifndef RENDERER_H
#define RENDERER_H

#include <string>
#include "Object3D.h"
#include "Simulation.h" // reuses SpecialKey, so Simulation and the renderer agree on one enum

// The GL/GLUT seam: the smallest set of drawing intents Game needs each
// frame, expressed without any GL type leaking into this header. GlutRenderer
// is the one production implementation; fake_renderer.h is the test double
// that justifies keeping this as an interface at all.
struct Color {
	float r = 0.0f, g = 0.0f, b = 0.0f, a = 1.0f;
};

struct FrameSetup {
	Color background;
	bool lighting = false, light0 = false, light1 = false, smoothShading = false;
	Vec3 eye, target;
	Vec3 spotlightPos, spotlightDir;
	Vec3 pivot;
	double worldRotationDeg = 0.0;
};

enum class TimerId { Submarine = 0, SeaAnimals = 1, Ships = 2 };

// GLUT's callbacks are free functions with no user-data slot, so the real
// adapter (GlutRenderer) keeps one static InputHandler* and forwards to it;
// Game is the only production InputHandler.
class InputHandler {
public:
	virtual ~InputHandler() = default;
	virtual void onDisplay() = 0;
	virtual void onResize(int width, int height) = 0;
	virtual void onSpecialKeyDown(SpecialKey key) = 0;
	virtual void onSpecialKeyUp(SpecialKey key) = 0;
	virtual void onKeyDown(unsigned char key) = 0;
	virtual void onKeyUp(unsigned char key) = 0;
	virtual void onTimer(TimerId id) = 0;
};

class Renderer {
public:
	struct WindowSize {
		int width, height;
	};

	virtual ~Renderer() = default;

	// Applies the Game.cpp:33-66 GL setup (blend, lighting, depth test,
	// multisample, ...) and returns a window sized at screen*0.6.
	virtual WindowSize createWindow(int argc, char** argv) = 0;
	// Registers every GLUT callback (display/reshape/keyboard/special/idle)
	// against handler and blocks in glutMainLoop() until quit() is called.
	virtual void run(InputHandler& handler) = 0;
	// Clears the frame buffer, positions the camera and spotlight, applies
	// the lighting/shading flags and rotates the world around pivot.
	virtual void beginFrame(const FrameSetup& setup) = 0;
	virtual void drawSun(Vec3 pos, double radius) = 0;
	virtual void drawSeaFace(const Vec3 corners[4], Vec3 offset) = 0;
	virtual void drawMesh(const Object3D& object) = 0;
	virtual void drawText(Vec3 pos, double rotYDeg, const std::string& text) = 0;
	virtual void endFrame() = 0;
	virtual void scheduleTimer(TimerId id, int millis) = 0;
	virtual void toggleFullscreen() = 0;
	// Lets run() return; the adapter must set
	// GLUT_ACTION_CONTINUE_EXECUTION for glutLeaveMainLoop() to work.
	virtual void quit() = 0;
};

#endif
