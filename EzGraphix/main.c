// Uncomment this to not start up a console
// Note: A console will still open when launching from Visual Studio, but the program will not interact with it
//#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")

#include <stdlib.h>
#include "ezgraphix.h"

#define WIDTH 690
#define HEIGHT 420

void keyCallback(int key, int action) {
	ezClose();
}

EZobject* object;

int setup(void) {
	// Perform setup here
	// e.g. configuring the window, setting up callback functions, and some object creation
	ezTitle("My Graphics Program!");
	ezSize(WIDTH, HEIGHT);

	ezSetKeyCallback(keyCallback);
	object = ezCreateRect(WIDTH / 3, 0, 2 * WIDTH / 3, HEIGHT / 4);
	// TODO, transformations (ezMoveRect...). Should ezCreateRect just be [width, height]? Then use position as move (vec2 uniform). Or have it take initial pos rather than assume 0.

	ezColour(object, 0.0f, 0.0f, 1.0f);

	return EZ_OK;
}

void draw(void) {
	// Draw a frame here
	ezDraw(object);
}

void cleanup(void) {
	// delete objects here
	ezDelete(object);
}