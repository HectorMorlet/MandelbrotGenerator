
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

// Orange and blue palette
#define ORANGE_R 152
#define ORANGE_G 213
#define ORANGE_B 140

#define BLUE_R 0
#define BLUE_G 149
#define BLUE_B 255

// Red and white palette
#define RED_R 255
#define RED_G 0
#define RED_B 0

#define WHITE_R 0
#define WHITE_G 0
#define WHITE_B 0


static int lerp(int start, int end, double t);



// -------------------------------------------- //
//   Color Generation                           //
// -------------------------------------------- //


unsigned char stepsToRed(int steps) {
	for (int i = 1; i < 10; i++) {
		if (steps == i) {
			return i * 25 * ORANGE_R / 255.0;
		}
	}

	if (steps >= 253) {
		return 0;
	}

	return lerp(ORANGE_R, 0, (double) steps / MAX_STEPS);
}


unsigned char stepsToGreen(int steps) {
	for (int i = 1; i < 10; i++) {
		if (steps == i) {
			return i * 25 * ORANGE_G / 255.0;
		}
	}

	if (steps >= 253) {
		return 0;
	}

	return lerp(ORANGE_G, 0, (double) steps / MAX_STEPS);
}


unsigned char stepsToBlue(int steps) {
	for (int i = 1; i < 10; i++) {
		if (steps == i) {
			return i * 25 * ORANGE_B / 255.0;
		}
	}

	if (steps >= 253) {
		return 0;
	}

	return lerp(ORANGE_B, 0, (double) steps / MAX_STEPS);
}



// -------------------------------------------- //
//   Linear Interpolation                       //
// -------------------------------------------- //


static int lerp(int start, int end, double t) {
	return start + (end - start) * t;
}
