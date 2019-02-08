#ifdef WIN32
#include <crtdbg.h>
#endif

#include "game.h"

int main() {
#ifdef WIN32
	// memory leak checks
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
	
	// allocation
	auto app = new Game();

	// initialise and loop
	app->run("aie physics engine", 854, 480, false);

	// deallocation
	delete app;

	return 0;
}
