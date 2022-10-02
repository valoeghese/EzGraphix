// Uncomment this to not start up a console
// Note: A console will still open when launching from Visual Studio, but the program will not interact with it
//#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")

#include "ezgraphix.h"
#include "ezmaths.h"
#include <math.h>

#define PI 3.141592f

EZobject* object;
EZobject* randomCircle;

void key(int key, int action)
{
	ezSetShouldClose();
}

void click(int button, int action) {
	printf("Action %d, Button %d\n", action, button);
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
	// Perform setup here
	// e.g. configuring the window, setting up callback functions, and some object creation
	const int width = 690;
	const int height = 420;
	
	ezTitle("My Graphics Program!");
	ezDisplaySize(width, height);

	ezSetKeyFunction(key);
	ezSetClickFunction(click);
	ezSetMouseMoveFunction(mouseMove);
	ezSetResizeFunction(resize);

	// create first object
	object = ezCreateRect(width / 3, height / 4);

	ezFilletRadius(object, 20.0);
	ezColour(object, 0.0f, 0.0f, 1.0f);
	ezMove(object, width / 3, 3 * height / 8);

	// create second object
	randomCircle = ezCreateCircle(30.0);
	ezColour(randomCircle, 0.6f, 0.6f, 0.6f);
	ezMove(randomCircle, width / 2, height / 2);

	// Setup went ok. Proceed with running the program!
	return EZ_OK;
}

float time = 0;

void draw(void)
{
	// Draw a frame here

	// rainbow colour effect!
	// Get R,G,B for the given HSV values, where the hue is dependent on time, and S and V are always 1
	float r, g, b;
	ezHSV(time / (2 * PI), 1.0f, 1.0f, &r, &g, &b);

	// this sets the background colour
	ezBackgroundColour(r, g, b);
	
	// draw objects
	ezDraw(object);
	ezDraw(randomCircle);

	// update the time & keep in the range [0,2pi]
	time += 0.01;
	if (time >= 2 * PI) {
		time = 0;
	}
}

void cleanup(void)
{
	// do anything you need to before the program exits here
	// for example deleting objects that haven't already been deleted
	ezDelete(object);
	ezDelete(randomCircle);
}