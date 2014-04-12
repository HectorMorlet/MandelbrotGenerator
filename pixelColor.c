
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


// TODO here the other function that is supposed to be in here that
// Richard did in his demo


unsigned char stepsToRed(int steps){
   // Place holder return value
   return steps % 30;
}


unsigned char stepsToBlue(int steps){
   // Place holder return value
   return steps % 30;
}


unsigned char stepsToGreen(int steps){
   // Place holder return value
   return steps % 30;
}
