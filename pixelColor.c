
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

#define COLOR_R 116
#define COLOR_G 130
#define COLOR_B 214


static int lerp(int start, int end, double t);



// -------------------------------------------- //
//   Color Generation                           //
// -------------------------------------------- //


unsigned char stepsToRed(int steps) {
	for (int i = 1; i < 10; i++) {
		if (steps == i) {
			return i * 25 * COLOR_R % 255;
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
			return i * 25 * COLOR_G % 255;
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
			return i * 25 * COLOR_B % 255;
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
