// Uncomment this to not start up a console
// Note: A console will still open when launching from Visual Studio, but the program will not interact with it
//#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")

#include "ezgraphix.h"

float width = 690;
float height = 420;

EZobject* object;

void keyCallback(int key, int action) {
	ezClose();
}

void resizeCallback(int w, int h) {
	width = (float) w;
	height = (float) h;

	ezMove(object, width / 3, 3 * height / 8);
	ezResize(object, width / 3, height / 4);
	printf("Resize\n");
}

int setup(void) {
	// Perform setup here
	// e.g. configuring the window, setting up callback functions, and some object creation
	ezTitle("My Graphics Program!");
	ezDisplaySize(width, height);

	ezSetKeyCallback(keyCallback);
	ezSetResizeCallback(resizeCallback);

	object = ezCreateRect(width / 3, height / 4);

	ezColour(object, 0.0f, 0.0f, 1.0f);
	ezMove(object, width / 3, 3 * height / 8);

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