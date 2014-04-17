

//
//  ascii.c
//  Mandelbrot ASCII
//  Ben Anderson
//  17 April 2014
//


//  Prints out the mandelbrot set at a set zoom level in ASCII


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>


#define true  1
#define false 0

#define MAX_STEPS        256
#define SET_EXCEED_VALUE 4.0
#define ZOOM             6


int escapeSteps(double x, double y) {
	int i = 0;
	int isInSet = true;

	double r = x;
	double s = y;

	while (i < MAX_STEPS && isInSet == true) {
		double currentR = r;
		double currentS = s;

		r = currentR * currentR - currentS * currentS + x;
		s = 2 * currentR * currentS + y;

		double check = r * r + s * s;
		if (check > SET_EXCEED_VALUE) {
			isInSet = false;
		} else {
			i++;
		}
	}

	return i;
}


int main(int argc, char *argv[]) {
	for (int y = -10; y < 10; y++) {
		for (int x = -50; x < 30; x++) {
			double actualX = x * exp2(-ZOOM);
			double actualY = y * exp2(-ZOOM);
			int escape = escapeSteps(actualX, actualY);

			if (escape == MAX_STEPS) {
				printf("*");
			} else {
				printf(" ");
			}
		}

		printf("\n");
	}

	return EXIT_SUCCESS;
}
