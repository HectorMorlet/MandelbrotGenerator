
//
//  mandelbrot.c
//  Mandelbrot Generator
//  Ben Anderson and Hector Morlet
//  9 April 2014
//


#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <math.h>

#include <unistd.h>
#include <netinet/in.h>

#include "mandelbrot.h"
#include "pixelColor.h"


#define TRUE                1
#define FALSE               0

#define PORT                8080
#define REQUEST_BUFFER_SIZE 2048

#define IMAGE_REQUEST_TYPE  0
#define VIEWER_REQUEST_TYPE 1

#define MAX_STEPS           256
#define SET_EXCEED_VALUE    4.0

#define FRACTAL_WIDTH       512
#define FRACTAL_HEIGHT      512


static int createServer(int port);
static int waitForConnection(int server);

static void respondToClient(int socket, char *path);

static int determineRequestTypeForPath(char *path);
static void serveBitmap(int socket, char *path);
static void serveFractalViewer(int socket);

static double parseX(char *path);
static double parseY(char *path);
static int parseZoom(char *path);

static void writeBitmapHeader(int socket);
static void writePixel(int socket, unsigned char r, unsigned char g,
	unsigned char b);
static void writeFractal(int socket, double startX, double startY,
	int zoom);



int main(int argc, char *argv[]) {
	printf("Starting server...\n");

	int server = createServer(PORT);

	int running = TRUE;
	while (running) {
		// Wait for a connection
		int client = waitForConnection(server);

		// Read the client's request
		char request[REQUEST_BUFFER_SIZE];
		long byteCount = read(client, request, sizeof(request) - 1);

		// Ensure there was no error
		if (byteCount < 0) {
			printf("Failed to read request from client!\n");
		} else {
			printf("Started main if statement...\n");

			printf("Request:\n%s\n", request);

			// Extract the path from the request
			char *pathStart = strchr(request, ' ');
			if (pathStart) {
				pathStart++;
				char *pathEnd = strchr(pathStart, ' ') - 1;

				long length = pathEnd - pathStart + 1;
				char path[length + 1];

				strncpy(path, pathStart, length);
				path[length] = '\0';
				printf("Path extracted from request.\n");

				// Respond to the client
				printf("Responding to request for: %s\n", path);
				respondToClient(client, path);
			}
		}

		printf("Closing client connection...\n\n------------------\n\n------------------\n\n");
		close(client);
	}

	printf("Closing server...\n");
	close(server);

	return EXIT_SUCCESS;
}



// -------------------------------------------- //
//   Server                                     //
// -------------------------------------------- //


static int createServer(int port) {
	printf("Creating server...\n");

	// Create a TCP socket
	int server = socket(AF_INET, SOCK_STREAM, 0);

	// Ensure there's no error
	assert(server >= 0);

	// Construct the server's own receive host/port
	struct sockaddr_in serverAddress;
	bzero((char *) &serverAddress, sizeof(serverAddress));
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = INADDR_ANY;
	serverAddress.sin_port = htons(port);

	// Allow the socket to reuse the port, in case it's already in use
	int option = 1;
	setsockopt(server,
		SOL_SOCKET,
		SO_REUSEADDR,
		&option,
		sizeof(option));

	// Bind socket to receive address
	int success = bind(server,
		(struct sockaddr *) &serverAddress,
		sizeof(serverAddress));
	assert(success >= 0);

	return server;
}


static int waitForConnection(int server) {
	printf("Waiting for connection...\n");

	// Listen for incomming data
	const int maxBacklog = 10;
	listen(server, maxBacklog);

	// Read the client's length
	struct sockaddr_in clientAddress;
	socklen_t clientLength = sizeof(clientAddress);

	// Accept the incomming connection
	int client = accept(server,
		(struct sockaddr *) &clientAddress,
		&clientLength);

	// Ensure there wasn't an error
	assert(client >= 0);

	return client;
}



// -------------------------------------------- //
//   Request URL Parsing                        //
// -------------------------------------------- //


static int determineRequestTypeForPath(char *path) {
	printf("Determining request type...\n");

	int type = VIEWER_REQUEST_TYPE;
	char *extension = strrchr(path, '.');

	// If there was no extension found
	if (!extension) {
		type = VIEWER_REQUEST_TYPE;
	} else {
		// If the extension was .bmp, then it was an image request
		if (strcmp(extension, ".bmp") == 0) {
			type = IMAGE_REQUEST_TYPE;
		} else {
			type = VIEWER_REQUEST_TYPE;
		}
	}

	return type;
}


static double parseX(char *path) {
	printf("Parsing X...\n");

	double x;

	// Get the start of the string we want to turn into a float
	char *str = strchr(path, 'x');

	if (!str) {
		x = 0.0;
	} else {
		str++;

		// Retrieve the float value from the string
		sscanf(str, "%lf", &x);
	}

	return x;
}


static double parseY(char *path) {
	printf("Parsing Y...\n");

	double y;

	// Get the start of the string
	char *str = strchr(path, 'y');

	if (!str) {
		y = 0.0;
	} else {
		str++;

		// Retrieve the value
		sscanf(str, "%lf", &y);
	}

	return y;
}


static int parseZoom(char *path) {
	printf("Parsing zoom...\n");

	int zoom;

	// Retrieve the start of the string
	char *str = strchr(path, 'z');

	if (!str) {
		zoom = 0;
	} else {
		str++;

		// Retrieve the value
		sscanf(str, "%d", &zoom);
	}

	return zoom;
}



// -------------------------------------------- //
//   Server Response                            //
// -------------------------------------------- //


static void respondToClient(int socket, char *path) {
	printf("Responding to client...\n");

	int requestType = determineRequestTypeForPath(path);

	// Serve different things depending on the request type
	if (requestType == VIEWER_REQUEST_TYPE) {
		serveFractalViewer(socket);
	} else if (requestType == IMAGE_REQUEST_TYPE) {
		serveBitmap(socket, path);
	}
}


static void serveBitmap(int socket, char *path) {
	printf("Serving bitmap...\n");

	// Fetch the X, Y, and zoom from the URL (path)
	int zoom = parseZoom(path);
	double startX = parseX(path);
	double startY = parseY(path);

	// Send the HTTP header
	char *message =
		"HTTP/1.0 200 Found\r\n"
		"Content-Type: image/bmp\r\n"
		"\r\n";
	int success = write(socket, message, strlen(message));
	assert(success >= 0);

	// Write the fractal to the socket
	writeFractal(socket, startX, startY, zoom);
}


static void serveFractalViewer(int socket) {
	printf("Serving fractal viewer...\n");

	long success;
	char *message;

	// Send the HTTP header
	message =
		"HTTP/1.0 200 Found\r\n"
		"Content-Type: text/html\r\n"
		"\r\n";
	success = write(socket, message, strlen(message));
	assert(success >= 0);

	// Send the HTML viewer
	message =
		"<!DOCTYPE html>\n"
		"<script src=\"https://almondbread.cse.unsw.edu.au/tiles.js\"></script>\n"
		"\n";
	success = write(socket, message, strlen(message));
	assert(success >= 0);
}



// -------------------------------------------- //
//   Mandelbrot Generation                      //
// -------------------------------------------- //


static void writeBitmapHeader(int socket) {
	printf("Writing bitmap header...\n");

	long success;

	// The bitmap header, preconfigured for a 512 by 512 image
	// The file size, pixel array size, and image width/height are
	// already configured
	unsigned char header[] = {
		0x42, 0x4D, // Format ID
		0x00, 0x00, 0x04, 0x00, // File size of the BMP in bytes CHANGABLE
		0x00, 0x00, 0x00, 0x00, // Application specific
		0x36, 0x00, 0x00, 0x00, // Position in the file where the pixel data starts
		0x28, 0x00, 0x00, 0x00, // Size of the DIB header
		0x00, 0x02, 0x00, 0x00, // Image width in pixels CHANGABLE
		0x00, 0x02, 0x00, 0x00, // Image height in pixels CHANGABLE
		0x01, 0x00, // Number of color planes
		0x18, 0x00, // Number of bits per pixel
		0x00, 0x00, 0x00, 0x00, // Compression used
		0x00, 0x00, 0x0C, 0x00, // Size of the pixel array data CHANGABLE
		0x13, 0x0B, 0x00, 0x00, // Horizontal resolution
		0x13, 0x0B, 0x00, 0x00, // Vertical resolution
		0x00, 0x00, 0x00, 0x00, // Number of colors in the palette
		0x00, 0x00, 0x00, 0x00 // Number of important colors
	};

	success = write(socket, header, sizeof(header));
	assert(success >= 0);
}


static void writePixel(int socket, unsigned char r, unsigned char g,
		unsigned char b) {
	long success;

	unsigned char color[] = {
		b, g, r
	};

	// Write a single pixel to the socket (3 bytes)
	success = write(socket, color, sizeof(color));
	assert(success >= 0);
}


static void writeFractal(int socket, double startX, double startY,
		int zoom) {
	printf("Writing fractal...\n");

	// Send the bitmap header
	writeBitmapHeader(socket);

	// The center of the image should be (0, 0), so start the x and y
	// at -256, and finish them at 256, leaving 0 to be in the center
	// and having a 512 by 512 image
	int y = -(FRACTAL_HEIGHT / 2);
	int x = -(FRACTAL_WIDTH / 2);

	// Pre-calculate the zoom, for quicker run time in the loop
	double actualZoom = exp2(-zoom);

	// Loop over each y row
	while (y < FRACTAL_HEIGHT / 2) {
		// Reset the X to be -256
		x = -(FRACTAL_WIDTH / 2);

		// Loop over each x column
		while (x < FRACTAL_WIDTH / 2) {
			// Transform the pixel into "math world" by multiplying
			// by the zoom, then translate it by the starting x and y
			double actualX = x * actualZoom + startX;
			double actualY = y * actualZoom + startY;

			// Calculate the escape steps
			int escape = escapeSteps(actualX, actualY);

			// Write the pixel to the socket
			writePixel(socket, stepsToRed(escape), stepsToGreen(escape),
				stepsToBlue(escape));

			x++;
		}

		y++;
	}
}



// -------------------------------------------- //
//   Mandelbrot Generation                      //
// -------------------------------------------- //


int escapeSteps(double x, double y) {
	// Method adapted from:
	// http://www.cs.princeton.edu/~wayne/mandel/mandel.html
	// The math is explained there, including formulas

	int i = 0;
	int isInSet = TRUE;

	double r = x;
	double s = y;

	while (i < MAX_STEPS && isInSet == TRUE) {
		// Check if the r and s values are still in the Mandelbrot set
		double check = r * r + s * s;
		if (check > SET_EXCEED_VALUE) {
			i++;
			isInSet = FALSE;
		} else {
			// Save the current r and s values for use in the calculations
			double currentR = r;
			double currentS = s;

			// Calculate the new r and s values
			r = currentR * currentR - currentS * currentS + x;
			s = 2 * currentR * currentS + y;

			i++;
		}
	}

	return i;
}
