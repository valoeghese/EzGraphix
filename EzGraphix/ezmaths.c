#include "ezmaths.h"

// Converts HSV to RGB
// http://www.easyrgb.com/en/math.php
void ezHSV(float h, float s, float v, float* r, float* g, float* b) {
	if (s == 0) {
		*r = v;
		*g = v;
		*b = v;
	} else {
		float var_h = h * 6;

		if (var_h == 6) var_h = 0; //H must be < 1
		int var_i = (int)(var_h);
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