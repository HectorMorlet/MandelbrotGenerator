
//
//  Mandelbrot pixelColor
//  By Hector and Ben
//  Task 2B
//


//  Program to give the color of a pixel given
//  the iterations taken for that pixel to be
//  included in the set


#include <stdio.h>
#include <stdlib.h>
#include "pixelColor.h"


#define MAX_STEPS 255
#define MIN_STEPS 1


// TODO here the other function that is supposed to be in here that
// Richard did in his demo

unsigned char stepsToRed(int steps){
   // Place holder return value
   return steps%30;
}


unsigned char stepsToBlue(int steps){
   // Place holder return value
   return steps%30;
}


unsigned char stepsToGreen(int steps){
   // Place holder return value
   return steps%30;
}