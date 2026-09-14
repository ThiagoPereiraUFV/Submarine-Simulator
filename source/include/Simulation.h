#ifndef SIMULATION_H
#define SIMULATION_H

#include <functional>
#include <string>
#include <vector>
#include "Object3D.h"
#include "Vec3.h"

// Uniform random integer in [0, n), the same operation as `rand() % n` but
// injectable: tests script it for reproducible spawn positions, and
// makeSystemRng() is what the real game uses.
using Rng = std::function<int(int)>;
Rng makeSystemRng();

enum class SpecialKey { Up, Down, Left, Right, F11, Other };

// F11 is not handled here: it is a window-manager concern the renderer owns,
// so Game intercepts it before it ever reaches Simulation.
struct SubmarineControls {
	bool accelerate = false, turn = false, immerse = false, emerge = false;
	// Doubles as both "what the held key requests" and the ramp accumulator
	// stepSubmarine() advances every tick, mirroring the original statics.
	float deltaVel = 0.0f, deltaAngVel = 0.0f;
};

struct HelpLine {
	std::string text;
	double yOffset;
};

// Pure world state and physics: no GL/GLUT, no I/O. The renderer reads
// everything it needs to draw a frame through these getters.
class Simulation {
public:
	Simulation(Object3D submarine, Object3D fish, Object3D shark, Object3D ship,
			   Object3D helicopter, Rng rng);

	// Recomputes the sea-cube corners for the given size; on the very
	// first call it also fixes the initial viewer/center and spawns the
	// sea animals, ships and helicopters (their spawn range is this
	// call's height/1000, matching the original screen-size-driven values).
	void setViewport(int width, int height);

	void stepSubmarine();
	void stepSeaAnimals();
	void stepShips();

	void pressSpecial(SpecialKey key);
	void releaseSpecial(SpecialKey key);
	// Returns true only for ESC (27), signalling the game should quit.
	bool pressKey(unsigned char key);
	void releaseKey(unsigned char key);

	Vec3 viewer() const { return viewer_; }
	Vec3 center() const { return center_; }
	bool helpVisible() const { return help_; }
	bool firstPerson() const { return firstPerson_; }
	bool lightingOn() const { return lighting_; }
	bool light0On() const { return light0On_; }
	bool light1On() const { return light1On_; }
	bool smoothShading() const { return smoothShading_; }
	Vec3 backgroundColor() const;
	double sunRadius() const;
	const std::vector<Vec3>& seaCorners() const { return seaCorners_; }
	const Object3D& submarine() const { return submarine_; }
	const std::vector<Object3D>& ships() const { return ships_; }
	const std::vector<Object3D>& fishes() const { return fishes_; }
	const std::vector<Object3D>& sharks() const { return sharks_; }
	const std::vector<Object3D>& helicopters() const { return helicopters_; }
	std::vector<HelpLine> helpLines() const;

private:
	void spawn(int spawnRange);
	double randomSignedCoordinate(int range);
	Vec3 randomColor();

	Object3D submarine_;
	Object3D fishTemplate_, sharkTemplate_, shipTemplate_, helicopterTemplate_;
	Rng rng_;

	std::vector<Object3D> fishes_, sharks_, ships_, helicopters_;

	bool started_ = false;
	int viewHeight_ = 0;
	std::vector<Vec3> seaCorners_;
	Vec3 viewer_, center_;

	SubmarineControls controls_;
	float velocity_ = 0.0f, angularVelocity_ = 0.0f;
	bool bobbingUp_ = true;
	int bobbingStep_ = 0;

	bool help_ = false, firstPerson_ = false;
	bool lighting_ = true, light0On_ = true, light1On_ = true;
	bool smoothShading_ = true;
};

#endif
