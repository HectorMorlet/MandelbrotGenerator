
//
//  pixelColor.c
//  Mandelbrot Generator
//  Ben and Hector
//  9 April 2014
//


#include <stdio.h>
#include <stdlib.h>

#include "pixelColor.h"


#define MAX_STEPS 256

#define COLOR_R 204 // Green: 152
#define COLOR_G 76  // Green: 213
#define COLOR_B 76  // Green: 140


static int lerp(int start, int end, double t);



// -------------------------------------------- //
//   Color Generation                           //
// -------------------------------------------- //


unsigned char stepsToRed(int steps) {
	for (int i = 1; i < 10; i++) {
		if (steps == i) {
			return i * 25 * COLOR_R / 255.0;
		}
	}

	if (steps >= 253) {
		return 0;
	} else if (steps <= 2) {
		return 0;
	}

	return lerp(COLOR_R, 0, (double) steps / MAX_STEPS);
}


unsigned char stepsToGreen(int steps) {
	for (int i = 1; i < 10; i++) {
		if (steps == i) {
			return i * 25 * COLOR_G / 255.0;
		}
	}

	if (steps >= 253) {
		return 0;
	} else if (steps <= 2) {
		return 0;
	}

	return lerp(COLOR_G, 0, (double) steps / MAX_STEPS);
}


unsigned char stepsToBlue(int steps) {
	for (int i = 1; i < 10; i++) {
		if (steps == i) {
			return i * 25 * COLOR_B / 255.0;
		}
	}

	if (steps >= 253) {
		return 0;
	} else if (steps <= 2) {
		return 0;
	}

	return lerp(COLOR_B, 0, (double) steps / MAX_STEPS);
}



// -------------------------------------------- //
//   Linear Interpolation                       //
// -------------------------------------------- //


static int lerp(int start, int end, double t) {
	return start + (end - start) * t;
}
