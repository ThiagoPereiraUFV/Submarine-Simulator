#include "include/GlutRenderer.h"
#include "include/Game.h"
#include "include/ObjParser.h"
#include "include/Simulation.h"

int main(int argc, char** argv) {
	GlutRenderer renderer;
	Simulation simulation(obj::parseFile("models/submarine.obj"),
						  obj::parseFile("models/fish1.obj"), obj::parseFile("models/shark1.obj"),
						  obj::parseFile("models/ship1.obj"),
						  obj::parseFile("models/helicopter.obj"), makeSystemRng());
	Game game(renderer, simulation);
	game.start(argc, argv);

	return 0;
}
