#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define MAX_STEPS 256
#define true 1
#define false 0
#define SET_EXCEED_VALUE 4.0


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
t
	return EXIT_SUCCESS;
}


