// Uncomment this to not start up a console
// Note: A console will still open when launching from Visual Studio, but the program will not interact with it
//#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")

#include <stdlib.h>
#include "ezgraphix.h"

void keyCallback(int key, int action) {
	ezClose();
}

EZobject* object;

int setup(void) {
	// Perform setup here
	// e.g. configuring the window, setting up callback functions, and some object creation
	ezTitle("My Graphics Program!");
	ezSize(690, 420);

	ezSetKeyCallback(keyCallback);
	object = ezCreateRect(0, 0, 1, 1);

	return EZ_OK;
}

void draw(void) {
	// Draw a frame here
	ezDraw(object);
}

void cleanup(void) {
	// delete objects here
	//ezDelete(object);
}