#include <time.h>

#include "OBoy/Environment.h"
#include "Macro.h"

int main(int argc, char* argv[])
{
  srand((int)time(NULL));

	// initialize the platform:
	OBoy::Environment::instance()->init(
		Macro::instance(),
		1024, // width
		768, // height
		false, // fullscreen
		"Macrowars", // window title
		"macro.dat", // persistence layer file name
		NULL, // no encryption
    "CEGUI"); // CEGUI interface

	// start the main loop
	OBoy::Environment::instance()->startMainLoop();

	// destroy the game:
	Macro::destroy();

	// destroy the environment:
	OBoy::Environment::instance()->destroy();

	return 0;
}

