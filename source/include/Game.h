#ifndef GAME_H
#define GAME_H

#include "Renderer.h"
#include "Simulation.h"

// Wires a Renderer to a Simulation: translates GLUT-shaped input events into
// Simulation calls, and Simulation state into Renderer drawing calls. Holds
// no GL state of its own.
class Game : public InputHandler {
public:
	Game(Renderer& renderer, Simulation& simulation);

	void start(int argc, char** argv);

	void onDisplay() override;
	void onResize(int width, int height) override;
	void onSpecialKeyDown(SpecialKey key) override;
	void onSpecialKeyUp(SpecialKey key) override;
	void onKeyDown(unsigned char key) override;
	void onKeyUp(unsigned char key) override;
	void onTimer(TimerId id) override;

private:
	void drawSea();
	void drawMeshes();
	void drawHelp();

	Renderer& renderer_;
	Simulation& simulation_;
	bool timersScheduled_ = false;
};

#endif
