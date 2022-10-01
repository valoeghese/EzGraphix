// Uncomment this to not start up a console
// Note: A console will still open when launching from Visual Studio, but the program will not interact with it
//#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")

#include "ezgraphix.h"

EZobject* object;

void key(int key, int action)
{
	ezClose();
}

void click(int button, int action) {
	printf("Action %d\n", action);
}

void mouseMove(double mouseX, double mouseY) {
	printf("Move %lf %lf\n", mouseX, mouseY);
}

void resize(int width, int height)
{
	ezMove(object, width / 3, 3 * height / 8);
	ezResize(object, width / 3, height / 4);
}

int setup(void)
{
	const int width = 690;
	const int height = 420;

	// Perform setup here
	// e.g. configuring the window, setting up callback functions, and some object creation
	ezTitle("My Graphics Program!");
	ezDisplaySize(width, height);

	ezSetKeyFunction(key);
	ezSetClickFunction(click);
	ezSetMouseMoveFunction(mouseMove);
	ezSetResizeFunction(resize);

	object = ezCreateRect(width / 3, height / 4);

	ezColour(object, 0.0f, 0.0f, 1.0f);
	ezMove(object, width / 3, 3 * height / 8);

	return EZ_OK;
}

void draw(void)
{
	// Draw a frame here
	ezDraw(object);
}

void cleanup(void)
{
	// delete objects here
	ezDelete(object);
}