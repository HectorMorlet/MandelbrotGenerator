
// mandelbrotTestingFile.c
// Mandelbrot parsing testing file
// Hector Morlet
// 13/4/2014
// 


#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>


#define MAX_URL_PARAM_LENGTH 30


static double parseX(char *path);
static double parseY(char *path);
static int parseZoom(char *path);


int main(int argc, char *argv[]) {
   printf("%lf\n", parseX("http://localhost:8080/tile_x-123_y-123_z-123.bmp"));
   assert(parseX("http://localhost:8080/tile_x-123_y-123_z-123.bmp") == -123);
   printf("%lf\n", parseY("http://localhost:8080/tile_x-123_y-123_z-123.bmp"));
   assert(parseY("http://localhost:8080/tile_x-123_y-123_z-123.bmp") == -123);
   printf("%d\n", parseZoom("http://localhost:8080/tile_x-123_y-123_z-123.bmp"));
   assert(parseZoom("http://localhost:8080/tile_x-123_y-123_z-123.bmp") == -123);

   return EXIT_SUCCESS;
}


static double parseX(char *path) {
   char *str = strchr(path, 'x');
   double x;

   if (!str) {
      x = 0.0;
   } else {
      str++;
      sscanf(str, "%lf", &x);
   }

   return x;
}


static double parseY(char *path) {
   char *str = strchr(path, 'y');
   double y;

   if (!str) {
      y = 0.0;
   } else {
      str++;
      sscanf(str, "%lf", &y);
   }

   return y;
}


static int parseZoom(char *path) {
   char *str = strchr(path, 'z');
   int zoom;

   if (!str) {
      zoom = 0;
   } else {
      str++;
      sscanf(str, "%d", &zoom);
   }

   return zoom;
}