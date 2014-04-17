
//
//  bitmap_server.c
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

#define ZOOM 4


static int createServer(int port);
static int waitForConnection(int server);

static void respondToClient(int socket, char *path);
int escapeSteps(double x, double y);


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
//   Server Response                            //
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



// -------------------------------------------- //
//   Server Response                            //
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


static void respondToClient(int socket, char *path) {
	writeBitmapHeader(socket);

	for (int y = -128; y < 128; y++) {
		for (int x = -128; x < 128; x++) {
			double actualX = x * exp2(-ZOOM);
			double actualY = y * exp2(-ZOOM);
			int escape = escapeSteps(actualX, actualY);

			if (escape == MAX_STEPS) {
				writePixel(socket, 0x00, 0x00, 0x00);
			} else {
				writePixel(socket, 0xff, 0xff, 0xff);
			}
		}
	}
}
