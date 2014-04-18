
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
	unsigned char color = lerp(COLOR_R, 0, (double) steps / MAX_STEPS);

	for (int i = 1; i < 10; i++) {
		if (steps == i) {
			color = i * 25 * COLOR_R % 255;
		}
	}

	if (steps >= 253) {
		color = 0;
	} else if (steps <= 2) {
		color = 0;
	}

	return color;
}


unsigned char stepsToGreen(int steps) {
	unsigned char color = lerp(COLOR_G, 0, (double) steps / MAX_STEPS);

	for (int i = 1; i < 10; i++) {
		if (steps == i) {
			color = i * 25 * COLOR_G % 255;
		}
	}

	if (steps >= 253) {
		color = 0;
	} else if (steps <= 2) {
		color = 0;
	}

	return color;
}


unsigned char stepsToBlue(int steps) {
	unsigned char color = lerp(COLOR_B, 0, (double) steps / MAX_STEPS);

	for (int i = 1; i < 10; i++) {
		if (steps == i) {
			color = i * 25 * COLOR_B % 255;
		}
	}

	if (steps >= 253) {
		color = 0;
	} else if (steps <= 2) {
		color = 0;
	}

	return color;
}



// -------------------------------------------- //
//   Linear Interpolation                       //
// -------------------------------------------- //


static int lerp(int start, int end, double t) {
	return start + (end - start) * t;
}
