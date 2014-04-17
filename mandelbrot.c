
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

#define MAX_STEPS            256
#define SET_EXCEED_VALUE     4.0
#define MAX_URL_PARAM_LENGTH 30

#define FRACTAL_WIDTH  512
#define FRACTAL_HEIGHT 512


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

	int running = true;
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



// -------------------------------------------- //
//   Server                                     //
// -------------------------------------------- //


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
		if (strcmp(extention, ".bmp") == 0) {
			type = IMAGE_REQUEST_TYPE;
		} else {
			type = VIEWER_REQUEST_TYPE;
		}
	}

	return type;
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



// -------------------------------------------- //
//   Server Response                            //
// -------------------------------------------- //


static void respondToClient(int socket, char *path) {
	int requestType = determineRequestTypeForPath(path);

	if (requestType == VIEWER_REQUEST_TYPE) {
		serveFractalViewer(socket);
	} else if (requestType == IMAGE_REQUEST_TYPE) {
		serveBitmap(socket, path);
	}
}


static void serveBitmap(int socket, char *path) {
	int zoom = parseZoom(path);
	double startX = parseX(path);
	double startY = parseY(path);

	printf("%d, %lf, %lf\n", zoom, startX, startY);

	writeFractal(socket, startX, startY, zoom);
}


static void serveFractalViewer(int socket) {
	int success;
	char *message;

	message =
		"HTTP/1.0 200 Found\r\n"
		"Content-Type: text/html\r\n"
		"\r\n";
	success = write(socket, message, strlen(message));
	assert(success >= 0);

	message =
		"<!DOCTYPE html>\n"
		"<html>\n"
		"<body>\n"
		"<script src=\"tiles.js\"></script>\n"
		"</body>\n"
		"</html>\n"
		"\n";
	success = write(socket, message, strlen(message));
	assert(success >= 0);
}



// -------------------------------------------- //
//   Mandelbrot Generation                      //
// -------------------------------------------- //


static void writeBitmapHeader(int socket) {
	int success;

	unsigned char header[] = {
		0x42, 0x4D, 0x5A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x36, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00, 0x00, 0x02,
		0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x01, 0x00, 0x18, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x24, 0x00, 0x00, 0x00, 0x13, 0x0B,
		0x00, 0x00, 0x13, 0x0B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00
	};

	success = write(socket, header, sizeof(header));
	assert(success >= 0);
}


static void writePixel(int socket, unsigned char r, unsigned char g,
		unsigned char b) {
	int success;

	unsigned char color[] = {
		b, g, r
	};

	success = write(socket, color, sizeof(color));
	assert(success >= 0);
}


static void writeFractal(int socket, double startX, double startY,
		int zoom) {
	writeBitmapHeader(socket);

	int y = -(FRACTAL_HEIGHT / 2);
	int x = -(FRACTAL_WIDTH / 2);

	while (y < FRACTAL_HEIGHT / 2) {
		x = -(FRACTAL_WIDTH / 2);
		while (x < FRACTAL_WIDTH / 2) {
			double actualX = x * exp2(-zoom) + startX;
			double actualY = y * exp2(-zoom) + startY;
			int escape = escapeSteps(actualX, actualY);

			if (escape == MAX_STEPS) {
				writePixel(socket, 0x00, 0x00, 0x00);
			} else {
				writePixel(socket, 0xff, 0xff, 0xff);
			}

			x++;
		}

		y++;
	}
}



// -------------------------------------------- //
//   Mandelbrot Generation                      //
// -------------------------------------------- //


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
