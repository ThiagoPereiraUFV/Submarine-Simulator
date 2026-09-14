#include "Game.h"

namespace {

// Indices into Simulation::seaCorners() for the 6 faces of the sea cube,
// ported verbatim from Game.cpp's drawSea() (Game.cpp:320-327).
const int seaFaceCorners[6][4] = {{1, 2, 3, 0}, {6, 7, 3, 2}, {3, 7, 4, 0},
								  {5, 6, 2, 1}, {7, 6, 5, 4}, {4, 5, 1, 0}};

} // namespace

Game::Game(Renderer& renderer, Simulation& simulation)
	: renderer_(renderer), simulation_(simulation) {}

void Game::start(int argc, char** argv) {
	Renderer::WindowSize size = renderer_.createWindow(argc, argv);
	simulation_.setViewport(size.width, size.height);
	renderer_.run(*this);
}

void Game::onResize(int width, int height) {
	simulation_.setViewport(width, height);
	if (!timersScheduled_) {
		renderer_.scheduleTimer(TimerId::Submarine, 1);
		renderer_.scheduleTimer(TimerId::SeaAnimals, 50);
		renderer_.scheduleTimer(TimerId::Ships, 50);
		timersScheduled_ = true;
	}
}

void Game::onTimer(TimerId id) {
	switch (id) {
	case TimerId::Submarine:
		simulation_.stepSubmarine();
		renderer_.scheduleTimer(TimerId::Submarine, 1);
		break;
	case TimerId::SeaAnimals:
		simulation_.stepSeaAnimals();
		renderer_.scheduleTimer(TimerId::SeaAnimals, 50);
		break;
	case TimerId::Ships:
		simulation_.stepShips();
		renderer_.scheduleTimer(TimerId::Ships, 50);
		break;
	}
}

void Game::onSpecialKeyDown(SpecialKey key) {
	if (key == SpecialKey::F11) {
		renderer_
			.toggleFullscreen(); // a window-manager concern the renderer owns, not the simulation
		return;
	}
	simulation_.pressSpecial(key);
}

void Game::onSpecialKeyUp(SpecialKey key) {
	if (key == SpecialKey::F11)
		return;
	simulation_.releaseSpecial(key);
}

void Game::onKeyDown(unsigned char key) {
	if (simulation_.pressKey(key))
		renderer_.quit();
}

void Game::onKeyUp(unsigned char key) {
	simulation_.releaseKey(key);
}

void Game::drawSea() {
	const std::vector<Vec3>& corners = simulation_.seaCorners();
	const Vec3 viewer = simulation_.viewer();
	// seaCorners()[2] is the (+h, +h, +h) corner, so its y is exactly the
	// current view height without Simulation needing a dedicated getter for it.
	const double h = corners[2].y;
	const Vec3 offset{viewer.x, -h, viewer.z};

	for (const int(&indices)[4] : seaFaceCorners) {
		const Vec3 face[4] = {corners[indices[0]], corners[indices[1]], corners[indices[2]],
							  corners[indices[3]]};
		renderer_.drawSeaFace(face, offset);
	}
}

void Game::drawMeshes() {
	for (const Object3D& ship : simulation_.ships())
		renderer_.drawMesh(ship);
	for (const Object3D& heli : simulation_.helicopters())
		renderer_.drawMesh(heli);
	for (const Object3D& fish : simulation_.fishes())
		renderer_.drawMesh(fish);
	for (const Object3D& shark : simulation_.sharks())
		renderer_.drawMesh(shark);
	renderer_.drawMesh(simulation_.submarine());
}

void Game::drawHelp() {
	if (!simulation_.helpVisible())
		return;

	const Vec3 center = simulation_.center();
	const double rotY = simulation_.submarine().rotY();
	for (const HelpLine& line : simulation_.helpLines())
		renderer_.drawText({center.x, center.y + line.yOffset, center.z}, rotY, line.text);
}

void Game::onDisplay() {
	const Vec3 viewer = simulation_.viewer();
	const Vec3 center = simulation_.center();
	const bool fp = simulation_.firstPerson();

	const Vec3 background = simulation_.backgroundColor();

	FrameSetup setup{};
	setup.background = {static_cast<float>(background.x), static_cast<float>(background.y),
						static_cast<float>(background.z)};
	setup.lighting = simulation_.lightingOn();
	setup.light0 = simulation_.light0On();
	setup.light1 = simulation_.light1On();
	setup.smoothShading = simulation_.smoothShading();
	setup.eye = {viewer.x, viewer.y + (fp ? -5.0 : 0.0), viewer.z + (fp ? -22.0 : 0.0)};
	setup.target = {center.x, center.y + (fp ? 5.0 : 10.0), center.z + (fp ? -22.0 : 0.0)};
	setup.spotlightPos = {-viewer.x, -viewer.y, -viewer.z};
	setup.spotlightDir = center;
	setup.pivot = simulation_.submarine().position();
	setup.worldRotationDeg = simulation_.submarine().rotY();

	renderer_.beginFrame(setup);

	const double h = simulation_.seaCorners()[2].y;
	renderer_.drawSun({viewer.x + 4 * h, 2 * h, viewer.z - 4 * h}, simulation_.sunRadius() * h);
	drawSea();
	drawMeshes();
	drawHelp();

	renderer_.endFrame();
}
