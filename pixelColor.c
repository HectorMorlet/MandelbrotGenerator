
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

#define ORANGE_R 255
#define ORANGE_G 0
#define ORANGE_B 0

#define BLUE_R 0
#define BLUE_G 0
#define BLUE_B 255


static int lerp(int start, int end, double t);



// -------------------------------------------- //
//   Color Generation                           //
// -------------------------------------------- //


unsigned char stepsToRed(int steps) {
	return lerp(ORANGE_R, BLUE_R, (double) steps / MAX_STEPS);
}


unsigned char stepsToGreen(int steps) {
	return lerp(ORANGE_G, BLUE_G, (double) steps / MAX_STEPS);
}


unsigned char stepsToBlue(int steps) {
	return lerp(ORANGE_B, BLUE_B, (double) steps / MAX_STEPS);
}



// -------------------------------------------- //
//   Linear Interpolation                       //
// -------------------------------------------- //


static int lerp(int start, int end, double t) {
	return start + (end - start) * t;
}
