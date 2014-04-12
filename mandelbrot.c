
//
//  mandelbrot.c
//  Mandelbrot Generator
//  Ben and Hector
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


#define true  1
#define false 0

#define PORT                8080
#define REQUEST_BUFFER_SIZE 1000

#define IMAGE_REQUEST_TYPE  0
#define VIEWER_REQUEST_TYPE 1

#define MAX_STEPS        256
#define SET_EXCEED_VALUE 4.0

#define FRACTAL_WIDTH  512
#define FRACTAL_HEIGHT 512


typedef byte unsigned char;


static int createServer(int port);
static int waitForConnection(int server);

static void respondToClient(int socket, char *path);

static int determineRequestTypeForPath(char *path);
static void serveBitmap(int socket, char *path);
static void serveFractalViewer(int socket, char *path);

static double parseX(char *path);
static double parseY(char *path);
static int parseZoom(char *path);

static void writeBitmapHeader(int socket);
static void writePixel(int socket, byte r, byte g, byte b);
static void writeFractal(int socket, double startX, double startY,
	int zoom);


void runTests(void);
void testPathParsing(void);
void testDetermineRequestType(void);



// -------------------------------------------- //
//   Server                                     //
// -------------------------------------------- //


int main(int argc, char *argv[]) {
	runTests();

	printf("Starting server...\n");

	int server = createServer(PORT);

	int running = 1;
	while (running) {
		// Wait for a connection
		int client = waitForConnection(server);

		// Read the client's request
		char request[REQUEST_BUFFER_SIZE];
		int byteCount = read(client, request, sizeof(request) - 1);

		// Ensure there was no error
		if (byteCount < 0) {
			printf("Failed to read request from client!\n");
		} else {
			// Extract the path from the request
			char *pathStart = strchr(request, ' ') + 1;
			char *pathEnd = strchr(pathStart, ' ') - 1;
			int length = pathEnd - pathStart + 1;
			char path[length + 1];

			strncpy(path, pathStart, length);
			path[length] = '\0';

			// Respond to the client
			printf("Responding to request for: %s\n", path);
			respondToClient(client, path);
		}

		close(client);
	}

	close(server);

	return EXIT_SUCCESS;
}


static int createServer(int port) {
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
	int type = VIEWER_REQUEST_TYPE;
	char *extention = strrchr(path, '.');

	if (!extention) {
		type = VIEWER_REQUEST_TYPE;
	} else {
		extention++;

		if (strcmp(extention, "bmp") == 0) {
			type = IMAGE_REQUEST_TYPE;
		} else {
			type = VIEWER_REQUEST_TYPE;
		}
	}

	return type;
}


static double parseX(char *path) {
	char *xString = strchr(path, 'x');
	int x;

	if (!x) {
		x = 0;
	} else {
		xString++;

		char *rightOfString = strchr(x, '_');
		strncat(x, xString, xString - rightOfString);
	}

	return x;
}


static double parseY(char *path) {
	char *yString = strchr(path, 'y');
	int y;

	if (!y) {
		y = 0;
	} else {
		yString++;

		char *rightOfString = strchr(y, '_');
		strncat(y, yString, yString - rightOfString);
	}

	return y;
}


static int parseZoom(char *path) {
	char *yString = strchr(path, 'z');
	int z;

	if (!z) {
		z = 0;
	} else {
		zString++;

		char *rightOfString = strchr(z, '.');
		strncat(z, zString, zString - rightOfString);
	}

	return z;
}



// -------------------------------------------- //
//   Server Response                            //
// -------------------------------------------- //


static void respondToClient(int socket, char *path) {
	int requestType = determineRequestTypeForPath(path);
	if (requestType == VIEWER_REQUEST_TYPE) {
		serveFractalViewer(socket, path);
	} else if (requestType == IMAGE_REQUEST_TYPE) {
		serveBitmap(socket, path);
	}
}


static void serveBitmap(int socket, char *path) {
	double startX = parseX(path) * exp2(-zoom);
	double startY = parseY(path) * exp2(-zoom);
	int zoom = parseZoom(path);

	writeFractal(socket, startX, startY, zoom);
}


static void serveFractalViewer(int socket, char *path) {
	char *message;

	message =
		"HTTP/1.0 200 Found\n"
		"Content-Type: text/html\n"
		"\n";
	write(socket, message, strlen(message));

	message =
		"<!DOCTYPE html>\n"
		"<script src=\"https://almondbread.cse.unsw.edu.au/tiles.js\">\n"
		"</script>\n"
		"\n";
	write(socket, message, strlen(message));
}



// -------------------------------------------- //
//   Mandelbrot Generation                      //
// -------------------------------------------- //


static void writeBitmapHeader(int socket) {
	int success;

	byte header[] = {
		0x42, 0x4d, 0x5a, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x36, 0x00, 0x00, 0x00, 0x28, 0x00,
		0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x03, 0x00,
		0x00, 0x00, 0x01, 0x00, 0x18, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x24, 0x00, 0x00, 0x00, 0x13, 0x0b,
		0x00, 0x00, 0x13, 0x0b, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	}

	success = write(socket, header, sizeof(header));
	assert(success >= 0);
}


static void writePixel(int socket, byte r, byte g, byte b) {
	int success;

	success = write(socket, r, sizeof(r));
	assert(success >= 0);

	success = write(socket, g, sizeof(g));
	assert(success >= 0);

	success = write(socket, b, sizeof(b));
	assert(success >= 0);
}


static void writeFractal(int socket, double startX, double startY,
		int zoom) {
	writeBitmapHeader(socket);

	int x = -(FRACTAL_WIDTH / 2);
	int y = -(FRACTAL_HEIGHT / 2);
	while (x < FRACTAL_WIDTH) {
		while (y < FRACTAL_HEIGHT) {
			double actualX = x * exp2(-zoom);
			double actualY = y * exp2(-zoom);
			int steps = escapeSteps(actualX + startX, actualY + startY);

			byte red = (byte) stepsToRed(steps);
			byte green = (byte) stepsToGreen(steps);
			byte blue = (byte) stepsToBlue(steps);

			writePixel(socket, red, green, blue)
		}
	}
}


int escapeSteps(double x, double y) {
	int i = 0;
	int isInSet = true;

	double r = x;
	double s = y;
	while (i < MAX_STEPS && isInSet) {
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



// -------------------------------------------- //
//   Unit Tests                                 //
// -------------------------------------------- //


void runTests(void) {
	testDetermineRequestType();
	testPathParsing();

	printf("All tests passed!\n");
	printf("We are awesome!\n");
}


void testPathParsing(void) {
	assert(parseX("http://localhost:[port]/tile_x-12.12414_y[y]_z[zoom level].bmp") == )
	assert(parseX("http://localhost:[port]/tile_x-123_y[y]_z[zoom level].bmp") == )
	assert(parseX("http://localhost:[port]/tile_x134.4124_y[y]_z[zoom level].bmp") == )
	assert(parseX("http://localhost:[port]/tile_x235_y[y]_z[zoom level].bmp") == )

	assert(parseY("http://localhost:[port]/tile_x-[x]_y235235.234234_z[zoom level].bmp") == )
	assert(parseY("http://localhost:[port]/tile_x-[x]_y234234_z[zoom level].bmp") == )
	assert(parseY("http://localhost:[port]/tile_x-[x]_y-234234_z[zoom level].bmp") == )
	assert(parseY("http://localhost:[port]/tile_x-[x]_y-234234.234234_z[zoom level].bmp") == )

	assert(parseZoom("http://localhost:[port]/tile_x-[x]_y[y]_z234234234.bmp") == )
	assert(parseZoom("http://localhost:[port]/tile_x-[x]_y[y]_z234234234.234234.bmp") == )
	assert(parseZoom("http://localhost:[port]/tile_x-[x]_y[y]_z-2343223434.bmp") == )
	assert(parseZoom("http://localhost:[port]/tile_x-[x]_y[y]_z-23423324.234324.bmp") == )
}


void testDetermineRequestType(void) {
	assert(determineRequestTypeForPath("/hello") == VIEWER_REQUEST_TYPE);
	assert(determineRequestTypeForPath("/test/owiejf") == VIEWER_REQUEST_TYPE);
	assert(determineRequestTypeForPath("/test.bmp") == IMAGE_REQUEST_TYPE);
	assert(determineRequestTypeForPath("/meh/meh2.bmp") == IMAGE_REQUEST_TYPE);
	assert(determineRequestTypeForPath("/index.bmp") == IMAGE_REQUEST_TYPE);
}
