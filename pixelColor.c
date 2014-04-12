
//
//  pixelColor.c
//  Mandelbrot Generator
//  Ben and Hector
//  9 April 2014
//


#include <stdio.h>
#include <stdlib.h>

#include "pixelColor.h"


#define MAX_STEPS 255
#define MIN_STEPS 1

#define RED_BLUE_VALUE
#define RED_ORANGE_VALUE
#define BLUE_BLUE_VALUE
#define BLUE_ORANGE_VALUE
#define GREEN_BLUE_VALUE
#define GREEN_ORANGE_VALUE


static int lerp(start, end, t);

unsigned char stepsToRed(int steps){
	return lerp(RED_BLUE_VALUE, RED_ORANGE_VALUE, steps/MAX_STEPS);
}


unsigned char stepsToBlue(int steps){
	return lerp(BLUE_BLUE_VALUE, BLUE_ORANGE_VALUE, steps/MAX_STEPS);
}


unsigned char stepsToGreen(int steps){
	return lerp(GREEN_BLUE_VALUE, GREEN_ORANGE_VALUE, steps/MAX_STEPS);
}


static int lerp(start, end, t) {
	return start + (end - start) * t;
}
