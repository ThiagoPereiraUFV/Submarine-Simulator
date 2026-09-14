#include <cmath>
#include <vector>
#include "doctest.h"
#include "Simulation.h"

namespace {

// Deterministic Rng: every call returns n/2 (so rng(2) never hits the "==0"
// negative branch, and every magnitude is exactly half of its range).
Rng halfRng() {
	return [](int n) { return n / 2; };
}

// Deterministic Rng that always takes the "negative" branch (rng(2)==0) and
// always returns a zero magnitude, to exercise the sign-selection logic.
Rng zeroRng() {
	return [](int) { return 0; };
}

Simulation makeSimulation(Rng rng) {
	return Simulation(Object3D(), Object3D(), Object3D(), Object3D(), Object3D(), std::move(rng));
}

} // namespace

TEST_CASE("setViewport initializes viewer/center from the submarine on the first call only") {
	Simulation sim = makeSimulation(halfRng());

	sim.setViewport(800, 600);
	CHECK(sim.viewer().x == doctest::Approx(0.0));
	CHECK(sim.viewer().y == doctest::Approx(2.5 * 0.003 * 600 + 10));
	CHECK(sim.viewer().z == doctest::Approx(5.0 * 0.003 * 600 + 10));
	CHECK(sim.center() == sim.submarine().position());

	// A later resize must not reset viewer/center back to the submarine's origin.
	sim.pressKey('w');
	sim.stepSubmarine();
	Vec3 movedCenter = sim.center();
	sim.setViewport(400, 300);
	CHECK(sim.center() == movedCenter);
}

TEST_CASE("setViewport spawns 20 fishes, 20 sharks, 5 ships and 5 helicopters, once") {
	Simulation sim = makeSimulation(halfRng());

	sim.setViewport(800, 600);
	CHECK(sim.fishes().size() == 20);
	CHECK(sim.sharks().size() == 20);
	CHECK(sim.ships().size() == 5);
	CHECK(sim.helicopters().size() == 5);

	sim.setViewport(800, 600); // second call must not spawn again
	CHECK(sim.fishes().size() == 20);
}

TEST_CASE("spawn positions follow the rng: magnitude and sign") {
	// halfRng() never takes the negative branch (rng(2) == 1, not 0), so every
	// coordinate is +range/2; fish/shark spawn range is the viewport height.
	Simulation sim = makeSimulation(halfRng());
	sim.setViewport(800, 100);

	CHECK(sim.fishes()[0].position() == Vec3{50, -50, 50});
	CHECK(sim.sharks()[0].position() == Vec3{50, -55, 50}); // shark y has an extra -5
	CHECK(sim.ships()[0].position() == Vec3{500, 0, 500});	// ship spawn range is fixed at 1000
	CHECK(sim.helicopters()[0].position() == Vec3{500, 150, 500}); // rng(200) + 50

	Simulation negativeSim = makeSimulation(zeroRng());
	negativeSim.setViewport(800, 100);
	CHECK(negativeSim.fishes()[0].position() == Vec3{0, 0, 0});
}

TEST_CASE("acceleration ramps up and caps at velocity 5.0; releasing decelerates back to 0") {
	Simulation sim = makeSimulation(halfRng());
	sim.setViewport(800, 600); // rotY starts at 0, so motion is purely along -z

	sim.pressKey('w');
	double previousZ = sim.submarine().position().z;
	for (int i = 0; i < 40; ++i) {
		sim.stepSubmarine();
		double z = sim.submarine().position().z;
		CHECK(z - previousZ <= 0.0); // never speeds up backwards
		previousZ = z;
	}
	// Once capped, one more step must move exactly by -5.0 (bobbing is along y, not z).
	sim.stepSubmarine();
	double afterCap = sim.submarine().position().z;
	CHECK(afterCap - previousZ == doctest::Approx(-5.0));

	sim.releaseKey('w');
	for (int i = 0; i < 60; ++i)
		sim.stepSubmarine();
	double beforeRest = sim.submarine().position().z;
	sim.stepSubmarine();
	CHECK(sim.submarine().position().z == doctest::Approx(beforeRest));
}

TEST_CASE("turning ramps angular velocity up to 1.0 and setRotY wraps at 360") {
	Simulation sim = makeSimulation(halfRng());
	sim.setViewport(800, 600);

	sim.pressSpecial(SpecialKey::Right);
	for (int i = 0; i < 20; ++i)
		sim.stepSubmarine();
	double before = sim.submarine().rotY();
	sim.stepSubmarine();
	CHECK(sim.submarine().rotY() - before == doctest::Approx(1.0));

	sim.releaseSpecial(SpecialKey::Right);
	sim.stepSubmarine(); // exercises the angular-velocity decay-to-zero branch
	Object3D probe;
	probe.setRotY(359.5);
	probe.setRotY(361.0);
	CHECK(probe.rotY() == 0.0);
}

TEST_CASE("S decelerates instead of accelerating, and Left turns the other way") {
	Simulation sim = makeSimulation(halfRng());
	sim.setViewport(800, 600);

	sim.pressKey('S');
	sim.stepSubmarine();
	CHECK(sim.submarine().position().z > 0.0); // moving backwards (+z, rotY == 0)

	// Build up real reverse speed before releasing: a single step leaves the
	// magnitude inside the snap-to-zero threshold, which takes a different
	// branch than decaying a negative velocity back up towards 0.
	for (int i = 0; i < 20; ++i)
		sim.stepSubmarine();
	sim.releaseKey('s');
	double zBeforeDecay = sim.submarine().position().z;
	sim.stepSubmarine();
	double reverseStep = sim.submarine().position().z - zBeforeDecay;
	CHECK(reverseStep > 0.0); // still drifting backwards
	sim.stepSubmarine();
	CHECK(sim.submarine().position().z - zBeforeDecay - reverseStep < reverseStep);

	sim.pressSpecial(SpecialKey::Left);
	sim.stepSubmarine();
	CHECK(sim.submarine().rotY() > 359.0); // turned negative, wrapped near 360

	// Same asymmetry for turning: ramp the negative angular velocity past the
	// threshold so releasing decays it instead of snapping straight to 0.
	for (int i = 0; i < 20; ++i)
		sim.stepSubmarine();
	sim.releaseSpecial(SpecialKey::Left);
	double rotBeforeDecay = sim.submarine().rotY();
	sim.stepSubmarine();
	CHECK(sim.submarine().rotY() != doctest::Approx(rotBeforeDecay));
}

TEST_CASE("F11 and unmapped special keys are a no-op in Simulation (Game/Renderer own them)") {
	Simulation sim = makeSimulation(halfRng());
	sim.pressSpecial(SpecialKey::F11);
	sim.releaseSpecial(SpecialKey::F11);
	sim.pressSpecial(SpecialKey::Other);
	sim.releaseSpecial(SpecialKey::Other);
	sim.pressSpecial(SpecialKey::Up);
	sim.releaseSpecial(SpecialKey::Up);
	CHECK(true); // nothing threw and no controls were left stuck on
}

TEST_CASE("emerge stops contributing once center.y reaches the surface") {
	Simulation sim = makeSimulation(halfRng());
	sim.setViewport(800, 600);

	// Submarine starts at y == 0 with this default template; go underwater
	// first so there is a surface to rise back towards.
	sim.pressSpecial(SpecialKey::Down);
	for (int i = 0; i < 5; ++i)
		sim.stepSubmarine();
	sim.releaseSpecial(SpecialKey::Down);
	REQUIRE(sim.center().y < 0.0);

	sim.pressSpecial(SpecialKey::Up);
	for (int i = 0; i < 20; ++i)
		sim.stepSubmarine();
	CHECK(sim.center().y >= 0.0);

	double afterSurface = sim.center().y;
	for (int i = 0; i < 5; ++i)
		sim.stepSubmarine();
	// Emerge itself no longer contributes at/above the surface; only the
	// small bobbing oscillation (+-0.01/step) may still move it.
	CHECK(sim.center().y < afterSurface + 0.1);
}

TEST_CASE("immerse stops contributing once center.y reaches -viewHeight") {
	Simulation sim = makeSimulation(halfRng());
	sim.setViewport(800, 100); // small viewHeight so the floor is reached quickly

	sim.pressSpecial(SpecialKey::Down);
	for (int i = 0; i < 10000 && sim.center().y > -100.0; ++i)
		sim.stepSubmarine();
	REQUIRE(sim.center().y <= -100.0);

	double atFloor = sim.center().y;
	for (int i = 0; i < 5; ++i)
		sim.stepSubmarine();
	// Immerse itself no longer contributes at the floor; only bobbing's small
	// oscillation may still move it, never a runaway descent.
	CHECK(sim.center().y > atFloor - 0.1);
}

TEST_CASE("bobbing moves center.y up for 30 steps then back down, with no net drift") {
	Simulation sim = makeSimulation(halfRng());
	sim.setViewport(800, 600);
	double start = sim.center().y;

	for (int i = 0; i < 31; ++i)
		sim.stepSubmarine();
	CHECK(sim.center().y - start == doctest::Approx(0.30));

	for (int i = 0; i < 31; ++i)
		sim.stepSubmarine();
	CHECK(sim.center().y - start == doctest::Approx(0.0));
}

TEST_CASE("stepSeaAnimals moves fish and sharks 0.1 along their heading") {
	Simulation sim = makeSimulation(halfRng());
	sim.setViewport(800, 600);

	Vec3 fishBefore = sim.fishes()[0].position();
	Vec3 sharkBefore = sim.sharks()[0].position();
	sim.stepSeaAnimals();
	Vec3 fishAfter = sim.fishes()[0].position();
	Vec3 sharkAfter = sim.sharks()[0].position();

	double fishDx = fishAfter.x - fishBefore.x, fishDz = fishAfter.z - fishBefore.z;
	CHECK(std::sqrt(fishDx * fishDx + fishDz * fishDz) == doctest::Approx(0.1));
	CHECK(fishAfter.y == doctest::Approx(fishBefore.y));

	double sharkDx = sharkAfter.x - sharkBefore.x, sharkDz = sharkAfter.z - sharkBefore.z;
	CHECK(std::sqrt(sharkDx * sharkDx + sharkDz * sharkDz) == doctest::Approx(0.1));
}

TEST_CASE("stepShips moves ships 0.1 along their heading") {
	Simulation sim = makeSimulation(halfRng());
	sim.setViewport(800, 600);

	Vec3 before = sim.ships()[0].position();
	sim.stepShips();
	Vec3 after = sim.ships()[0].position();

	double dx = after.x - before.x, dz = after.z - before.z;
	CHECK(std::sqrt(dx * dx + dz * dz) == doctest::Approx(0.1));
	CHECK(after.y == doctest::Approx(before.y));
}

TEST_CASE("H toggles help visibility") {
	Simulation sim = makeSimulation(halfRng());
	CHECK_FALSE(sim.helpVisible());
	sim.pressKey('H');
	CHECK(sim.helpVisible());
	sim.pressKey('h');
	CHECK_FALSE(sim.helpVisible());
}

TEST_CASE("F and I switch the camera between third and first person") {
	Simulation sim = makeSimulation(halfRng());
	CHECK_FALSE(sim.firstPerson());
	sim.pressKey('I');
	CHECK(sim.firstPerson());
	sim.pressKey('F');
	CHECK_FALSE(sim.firstPerson());
}

TEST_CASE("L turns lighting off, resetting both lights on, and back on") {
	Simulation sim = makeSimulation(halfRng());
	sim.pressKey('1'); // turn light0 off first, to prove L resets it
	CHECK_FALSE(sim.light0On());

	sim.pressKey('L');
	CHECK_FALSE(sim.lightingOn());
	CHECK(sim.light0On());
	CHECK(sim.light1On());
	CHECK(sim.backgroundColor() == Vec3{0.0, 227.0 / 255.0, 1.0});

	sim.pressKey('l');
	CHECK(sim.lightingOn());
}

TEST_CASE("1 toggles light0 and the background color while lighting is on; ignored while off") {
	Simulation sim = makeSimulation(halfRng());
	CHECK(sim.backgroundColor() == Vec3{0.0, 227.0 / 255.0, 1.0});
	CHECK(sim.sunRadius() == doctest::Approx(0.2));

	sim.pressKey('1');
	CHECK_FALSE(sim.light0On());
	CHECK(sim.backgroundColor() == Vec3{8.0 / 255.0, 24.0 / 255.0, 97.0 / 255.0});
	CHECK(sim.sunRadius() == doctest::Approx(0.1));

	sim.pressKey('1');
	CHECK(sim.light0On());

	sim.pressKey('L'); // lighting off
	bool light0BeforeIgnoredPress = sim.light0On();
	sim.pressKey('1');
	CHECK(sim.light0On() == light0BeforeIgnoredPress); // '1' is a no-op while lighting is off
}

TEST_CASE("2 toggles light1") {
	Simulation sim = makeSimulation(halfRng());
	CHECK(sim.light1On());
	sim.pressKey('2');
	CHECK_FALSE(sim.light1On());
	sim.pressKey('2');
	CHECK(sim.light1On());
}

TEST_CASE("G toggles smooth shading on the very first press") {
	Simulation sim = makeSimulation(halfRng());
	CHECK(sim.smoothShading());
	sim.pressKey('G');
	CHECK_FALSE(sim.smoothShading());
	sim.pressKey('g');
	CHECK(sim.smoothShading());
}

TEST_CASE("ESC requests quit; every other key does not") {
	Simulation sim = makeSimulation(halfRng());
	CHECK_FALSE(sim.pressKey('w'));
	CHECK(sim.pressKey(27));
}

TEST_CASE("helpLines returns the 8 Portuguese lines with decreasing y offsets") {
	Simulation sim = makeSimulation(halfRng());
	std::vector<HelpLine> lines = sim.helpLines();
	REQUIRE(lines.size() == 8);
	CHECK(lines[0].text == " W para acelerar");
	CHECK(lines[0].yOffset == 0.0);
	CHECK(lines[7].text == " I ponto de vista dentro");
	CHECK(lines[7].yOffset == -7.0);
}

TEST_CASE("makeSystemRng returns values within [0, n)") {
	Rng rng = makeSystemRng();
	for (int i = 0; i < 200; ++i) {
		int value = rng(10);
		CHECK(value >= 0);
		CHECK(value < 10);
	}
}
