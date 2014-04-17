
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
#define ORANGE_R 255
#define ORANGE_G 100
#define ORANGE_B 0

#define BLUE_R 0
#define BLUE_G 149
#define BLUE_B 255

// Red and white palette
#define RED_R 183
#define RED_G 0
#define RED_B 0

#define WHITE_R 255
#define WHITE_G 255
#define WHITE_B 255


static int lerp(int start, int end, double t);



// -------------------------------------------- //
//   Color Generation                           //
// -------------------------------------------- //


unsigned char stepsToRed(int steps) {
	return lerp(WHITE_R, RED_R, (double) steps / MAX_STEPS);
}


unsigned char stepsToGreen(int steps) {
	return lerp(WHITE_G, RED_G, (double) steps / MAX_STEPS);
}


unsigned char stepsToBlue(int steps) {
	return lerp(WHITE_B, RED_B, (double) steps / MAX_STEPS);
}



// -------------------------------------------- //
//   Linear Interpolation                       //
// -------------------------------------------- //


static int lerp(int start, int end, double t) {
	return start + (end - start) * t;
}
