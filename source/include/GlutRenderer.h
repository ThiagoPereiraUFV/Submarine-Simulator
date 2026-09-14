#ifndef GLUTRENDERER_H
#define GLUTRENDERER_H

#include "Renderer.h"

// The one production Renderer. GLUT callbacks are C function pointers with
// no user-data slot, so every callback is a public static trampoline
// forwarding to `current_` -- which is also what lets test_glut_renderer.cpp
// exercise them directly, without synthesizing X events.
class GlutRenderer : public Renderer {
public:
	WindowSize createWindow(int argc, char** argv) override;
	void run(InputHandler& handler) override;
	void beginFrame(const FrameSetup& setup) override;
	void drawSun(Vec3 pos, double radius) override;
	void drawSeaFace(const Vec3 corners[4], Vec3 offset) override;
	void drawMesh(const Object3D& object) override;
	void drawText(Vec3 pos, double rotYDeg, const std::string& text) override;
	void endFrame() override;
	void scheduleTimer(TimerId id, int millis) override;
	void toggleFullscreen() override;
	void quit() override;

	static void displayCallback();
	static void reshapeCallback(int width, int height);
	static void specialDownCallback(int key, int x, int y);
	static void specialUpCallback(int key, int x, int y);
	static void keyDownCallback(unsigned char key, int x, int y);
	static void keyUpCallback(unsigned char key, int x, int y);
	// value is the TimerId the timer was scheduled with.
	static void timerCallback(int value);

private:
	static InputHandler* current_;
};

#endif
