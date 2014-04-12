
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

#define RED_LERP_START 
#define RED_LERP_END
#define BLUE_LERP_START
#define BLUE_LERP_END
#define GREEN_LERP_START
#define GREEN_LERP_END


static int lerp(start, end, time);

unsigned char stepsToRed(int steps){
   return lerp(RED_LERP_START, RED_LERP_END, steps/MAX_STEPS);
}


unsigned char stepsToBlue(int steps){
   return lerp(BLUE_LERP_START, BLUE_LERP_END, steps/MAX_STEPS);
}


unsigned char stepsToGreen(int steps){
   return lerp(GREEN_LERP_START, GREEN_LERP_END, steps/MAX_STEPS);
}

static int lerp(start, end, time) {
   return start+(end-start)*time;
}