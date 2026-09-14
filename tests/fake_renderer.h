#ifndef FAKE_RENDERER_H
#define FAKE_RENDERER_H

#include <utility>
#include <vector>
#include "Renderer.h"

// Records every call Game makes so tests can assert on them without a
// display. No GL, no window -- that is exactly why this second
// implementation exists (see Renderer.h).
class FakeRenderer : public Renderer {
public:
	WindowSize createWindow(int, char**) override {
		createWindowCalled = true;
		return windowSize;
	}

	void run(InputHandler& handler) override {
		runCalled = true;
		handler_ = &handler;
	}

	void beginFrame(const FrameSetup& setup) override {
		lastFrame = setup;
		beginFrameCount++;
	}

	void drawSun(Vec3 pos, double radius) override {
		lastSunPos = pos;
		lastSunRadius = radius;
		drawSunCount++;
	}

	void drawSeaFace(const Vec3[4], Vec3) override { drawSeaFaceCount++; }

	void drawMesh(const Object3D&) override { drawMeshCount++; }

	void drawText(Vec3, double, const std::string&) override { drawTextCount++; }

	void endFrame() override { endFrameCount++; }

	void scheduleTimer(TimerId id, int millis) override { scheduledTimers.push_back({id, millis}); }

	void toggleFullscreen() override { toggleFullscreenCount++; }

	void quit() override { quitCount++; }

	WindowSize windowSize{800, 600};
	bool createWindowCalled = false, runCalled = false;
	InputHandler* handler_ = nullptr;

	int beginFrameCount = 0, drawSunCount = 0, drawSeaFaceCount = 0;
	int drawMeshCount = 0, drawTextCount = 0, endFrameCount = 0;
	int toggleFullscreenCount = 0, quitCount = 0;

	FrameSetup lastFrame{};
	Vec3 lastSunPos{};
	double lastSunRadius = 0.0;
	std::vector<std::pair<TimerId, int>> scheduledTimers;
};

#endif
