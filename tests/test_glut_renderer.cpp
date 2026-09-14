// glutInit() may only be called once per process ("illegal glutInit()
// reinitialization attempt" otherwise), and createWindow() is the only place
// that calls it. Every GL assertion in this file therefore lives in a single
// TEST_CASE with no SUBCASEs -- doctest re-enters the whole case for each
// SUBCASE, which would call createWindow() (and glutInit()) again and crash.
//
// A second, less obvious constraint verified on this machine: once
// glutLeaveMainLoop() lets glutMainLoop() return, freeglut tears itself down
// far enough that *any* further GLUT/GL call -- even glutTimerFunc, not just
// drawing -- fails with "called without first calling glutInit". So every
// static callback below is exercised from *inside* onDisplay, while the main
// loop is still running, and quit() is only called once nothing else is left
// to cover.
#include <sstream>
#include <GL/freeglut.h>
#include "doctest.h"
#include "GlutRenderer.h"
#include "ObjParser.h"

namespace {

class SmokeHandler : public InputHandler {
public:
	explicit SmokeHandler(Renderer& renderer) : renderer_(renderer) {}

	void onDisplay() override {
		++frame_;

		FrameSetup setup{};
		setup.background = {0.0f, 0.5f, 1.0f, 1.0f};
		setup.lighting = (frame_ == 1);
		setup.light0 = (frame_ == 1);
		setup.light1 = true;
		setup.smoothShading = (frame_ == 1);
		setup.eye = {0, 10, 20};
		setup.target = {0, 0, 0};
		setup.spotlightPos = {0, -10, -20};
		setup.spotlightDir = {0, 0, 0};
		setup.pivot = {0, 0, 0};
		setup.worldRotationDeg = 15.0;

		renderer_.beginFrame(setup);
		renderer_.drawSun({40, 20, -40}, 5.0);

		const Vec3 corners[4] = {{-10, -10, 10}, {-10, 10, 10}, {10, 10, 10}, {10, -10, 10}};
		renderer_.drawSeaFace(corners, {0, -10, 0});

		renderer_.drawMesh(fish_);
		renderer_.drawMesh(texturedMesh_);
		renderer_.drawText({0, 0, 0}, 15.0, "abc");
		renderer_.endFrame();

		if (frame_ == 2) {
			// Every other callback GLUT would invoke, called directly (no
			// X events to synthesize) while the loop is still alive.
			GlutRenderer::reshapeCallback(640, 480);

			GlutRenderer::specialDownCallback(GLUT_KEY_UP, 0, 0);
			GlutRenderer::specialDownCallback(GLUT_KEY_DOWN, 0, 0);
			GlutRenderer::specialDownCallback(GLUT_KEY_LEFT, 0, 0);
			GlutRenderer::specialDownCallback(GLUT_KEY_RIGHT, 0, 0);
			GlutRenderer::specialDownCallback(GLUT_KEY_F11, 0, 0);
			GlutRenderer::specialDownCallback(GLUT_KEY_HOME, 0, 0); // maps to SpecialKey::Other
			GlutRenderer::specialUpCallback(GLUT_KEY_UP, 0, 0);

			GlutRenderer::keyDownCallback('w', 0, 0);
			GlutRenderer::keyUpCallback('w', 0, 0);

			GlutRenderer::timerCallback(0); // TimerId::Submarine
			GlutRenderer::timerCallback(1); // TimerId::SeaAnimals
			GlutRenderer::timerCallback(2); // TimerId::Ships

			renderer_.scheduleTimer(TimerId::Submarine, 60000); // fires well after quit()
			renderer_.toggleFullscreen();

			renderer_.quit();
		}
	}

	void onResize(int, int) override { resizeCount_++; }
	void onSpecialKeyDown(SpecialKey) override { specialDownCount_++; }
	void onSpecialKeyUp(SpecialKey) override { specialUpCount_++; }
	void onKeyDown(unsigned char) override { keyDownCount_++; }
	void onKeyUp(unsigned char) override { keyUpCount_++; }
	void onTimer(TimerId) override { timerCount_++; }

	int frame_ = 0, resizeCount_ = 0, specialDownCount_ = 0, specialUpCount_ = 0;
	int keyDownCount_ = 0, keyUpCount_ = 0, timerCount_ = 0;

private:
	Renderer& renderer_;
	Object3D fish_ = obj::parseFile("models/fish1.obj");
	// A tiny mesh with an f a/b/c face, to exercise drawMesh's texcoord
	// path (fish1.obj has none).
	Object3D texturedMesh_ = [] {
		std::istringstream in("v 0 0 0\n"
							  "v 1 0 0\n"
							  "v 0 1 0\n"
							  "vt 0 0 0\n"
							  "vt 1 0 0\n"
							  "vt 0 1 0\n"
							  "vn 0 0 1\n"
							  "vn 0 0 1\n"
							  "vn 0 0 1\n"
							  "f 1/1/1 2/2/2 3/3/3\n");
		return obj::parse(in);
	}();
};

} // namespace

TEST_CASE(
	"GlutRenderer drives a real GL/GLUT window end-to-end (single TEST_CASE: see file header)") {
	GlutRenderer renderer;

	Renderer::WindowSize size = renderer.createWindow(0, nullptr);
	CHECK(size.width > 0);
	CHECK(size.height > 0);

	SmokeHandler handler(renderer);
	renderer.run(handler); // blocks until frame 2 calls quit()

	// reshapeCallback's glutPostRedisplay() may queue one more frame before
	// quit()'s glutLeaveMainLoop() actually takes effect, so this is >=, not ==.
	CHECK(handler.frame_ >= 2);
	CHECK(handler.resizeCount_ >= 1);
	CHECK(handler.specialDownCount_ == 6);
	CHECK(handler.specialUpCount_ == 1);
	CHECK(handler.keyDownCount_ == 1);
	CHECK(handler.keyUpCount_ == 1);
	CHECK(handler.timerCount_ == 3);
}
