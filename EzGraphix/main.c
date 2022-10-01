// Uncomment this to not start up a console
// Note: A console will still open when launching from Visual Studio, but the program will not interact with it
//#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")

#include "ezgraphix.h"
#include <math.h>

#define PI 3.141592f

EZobject* object;

// Converts HSV to RGB
// http://www.easyrgb.com/en/math.php
void hsv(float h, float s, float v, float* r, float* g, float* b) {
	if (s == 0) {
		*r = v;
		*g = v;
		*b = v;
	}
	else {
		float var_h = h * 6;
		
		if (var_h == 6) var_h = 0; //H must be < 1
		int var_i = (int) (var_h);
		float var_1 = v * (1 - s);
		float var_2 = v * (1 - s * (var_h - var_i));
		float var_3 = v * (1 - s * (1 - (var_h - var_i)));

		float var_r;
		float var_g;
		float var_b;

		switch (var_i) {
		case 0:
			var_r = v; var_g = var_3; var_b = var_1;
			break;
		case 1:
			var_r = var_2; var_g = v; var_b = var_1;
			break;
		case 2:
			var_r = var_1; var_g = v; var_b = var_3;
			break;
		case 3:
			var_r = var_1; var_g = var_2; var_b = v;
			break;
		case 4:
			var_r = var_3; var_g = var_1; var_b = v;
			break;
		default:
			var_r = v; var_g = var_1; var_b = var_2;
			break;
		}

		*r = var_r;
		*g = var_g;
		*b = var_b;
	}
}

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

float time = 0;

void draw(void)
{
	float r, g, b;
	hsv(time / (2 * PI), 1.0f, 1.0f, &r, &g, &b);

	ezBackgroundColour(r, g, b);

	// Draw a frame here
	ezDraw(object);

	time += 0.01;
	if (time >= 2 * PI) {
		time = 0;
	}
}

void cleanup(void)
{
	// delete objects here
	ezDelete(object);
}