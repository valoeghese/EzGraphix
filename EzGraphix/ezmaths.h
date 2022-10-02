//
// The mathematics utility header for EzGraphix.
// Contains mathematical functions useful for graphics that are not already found in <math.h>
// The implementation of these functions can be found in ezmaths.c
// 
// For the main library header, see ezgraphix.h
//
// Author: Mekal Covic
//

#pragma once

// Calculates the red, green, and blue values, as floating point numbers in the range [0,1]
// for a given hue, saturation, and value in the range [0,1]
void ezHSV(float h, float s, float v, float* r, float* g, float* b);
