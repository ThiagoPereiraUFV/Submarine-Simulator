#include "doctest.h"
#include "Game.h"
#include "fake_renderer.h"

namespace {

Rng halfRng() {
	return [](int n) { return n / 2; };
}

Simulation makeSimulation() {
	return Simulation(Object3D(), Object3D(), Object3D(), Object3D(), Object3D(), halfRng());
}

} // namespace

TEST_CASE("start creates the window, hands the viewport to the simulation, then runs") {
	FakeRenderer renderer;
	Simulation sim = makeSimulation();
	Game game(renderer, sim);

	int argc = 1;
	char arg0[] = "submarine";
	char* argv[] = {arg0};
	game.start(argc, argv);

	CHECK(renderer.createWindowCalled);
	CHECK(renderer.runCalled);
	CHECK(sim.fishes().size() == 20); // setViewport ran, which spawns the world
}

TEST_CASE("onResize schedules the 3 timers only on its first call") {
	FakeRenderer renderer;
	Simulation sim = makeSimulation();
	Game game(renderer, sim);

	game.onResize(800, 600);
	REQUIRE(renderer.scheduledTimers.size() == 3);
	CHECK(renderer.scheduledTimers[0] == std::make_pair(TimerId::Submarine, 1));
	CHECK(renderer.scheduledTimers[1] == std::make_pair(TimerId::SeaAnimals, 50));
	CHECK(renderer.scheduledTimers[2] == std::make_pair(TimerId::Ships, 50));

	game.onResize(800, 600);
	CHECK(renderer.scheduledTimers.size() == 3); // no extra scheduling on later resizes
}

TEST_CASE("onTimer(Submarine) steps the submarine and reschedules itself at 1ms") {
	FakeRenderer renderer;
	Simulation sim = makeSimulation();
	Game game(renderer, sim);
	game.onResize(800, 600);
	renderer.scheduledTimers.clear();

	game.onKeyDown('w');
	Vec3 before = sim.submarine().position();
	game.onTimer(TimerId::Submarine);

	CHECK(sim.submarine().position().z != before.z); // accelerating moves it
	REQUIRE(renderer.scheduledTimers.size() == 1);
	CHECK(renderer.scheduledTimers[0] == std::make_pair(TimerId::Submarine, 1));
}

TEST_CASE("onTimer(SeaAnimals) and onTimer(Ships) step and reschedule at 50ms") {
	FakeRenderer renderer;
	Simulation sim = makeSimulation();
	Game game(renderer, sim);
	game.onResize(800, 600);
	renderer.scheduledTimers.clear();

	game.onTimer(TimerId::SeaAnimals);
	game.onTimer(TimerId::Ships);

	REQUIRE(renderer.scheduledTimers.size() == 2);
	CHECK(renderer.scheduledTimers[0] == std::make_pair(TimerId::SeaAnimals, 50));
	CHECK(renderer.scheduledTimers[1] == std::make_pair(TimerId::Ships, 50));
}

TEST_CASE("onDisplay draws every mesh, six sea faces, the sun, and no help text by default") {
	FakeRenderer renderer;
	Simulation sim = makeSimulation();
	Game game(renderer, sim);
	game.onResize(800, 600);

	game.onDisplay();

	CHECK(renderer.beginFrameCount == 1);
	CHECK(renderer.drawSunCount == 1);
	CHECK(renderer.drawSeaFaceCount == 6);
	CHECK(renderer.drawMeshCount ==
		  1 + 5 + 5 + 20 + 20); // submarine + ships + helis + fishes + sharks
	CHECK(renderer.drawTextCount == 0);
	CHECK(renderer.endFrameCount == 1);
}

TEST_CASE("onDisplay draws the 8 help lines once help is toggled on") {
	FakeRenderer renderer;
	Simulation sim = makeSimulation();
	Game game(renderer, sim);
	game.onResize(800, 600);

	game.onKeyDown('H');
	game.onDisplay();

	CHECK(renderer.drawTextCount == 8);
}

TEST_CASE("first-person camera applies the (-5, -22) offsets to eye, not target") {
	FakeRenderer renderer;
	Simulation sim = makeSimulation();
	Game game(renderer, sim);
	game.onResize(800, 600);

	game.onDisplay();
	Vec3 thirdPersonEye = renderer.lastFrame.eye;

	game.onKeyDown('I'); // first person
	game.onDisplay();
	Vec3 firstPersonEye = renderer.lastFrame.eye;

	CHECK(firstPersonEye.y == doctest::Approx(thirdPersonEye.y - 5.0));
	CHECK(firstPersonEye.z == doctest::Approx(thirdPersonEye.z - 22.0));
	CHECK(firstPersonEye.x == doctest::Approx(thirdPersonEye.x));
}

TEST_CASE("F11 toggles fullscreen on the renderer without reaching the simulation") {
	FakeRenderer renderer;
	Simulation sim = makeSimulation();
	Game game(renderer, sim);

	game.onSpecialKeyDown(SpecialKey::F11);
	CHECK(renderer.toggleFullscreenCount == 1);

	game.onSpecialKeyUp(SpecialKey::F11); // must not throw or forward to Simulation either
	CHECK(renderer.toggleFullscreenCount == 1);
}

TEST_CASE("arrow keys reach the simulation through Game") {
	FakeRenderer renderer;
	Simulation sim = makeSimulation();
	Game game(renderer, sim);
	game.onResize(800, 600);

	game.onSpecialKeyDown(SpecialKey::Down); // immerse
	game.onTimer(TimerId::Submarine);
	CHECK(sim.center().y < 0.0);

	game.onSpecialKeyUp(SpecialKey::Down);
}

TEST_CASE("ESC quits through the renderer") {
	FakeRenderer renderer;
	Simulation sim = makeSimulation();
	Game game(renderer, sim);

	game.onKeyDown('a');
	CHECK(renderer.quitCount == 0);

	game.onKeyDown(27);
	CHECK(renderer.quitCount == 1);

	game.onKeyUp('a'); // releaseKey path, no observable effect on the renderer
	CHECK(renderer.quitCount == 1);
}
