
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

#include <unistd.h>
#include <netinet/in.h>

#include "mandelbrot.h"


#define PORT                8080
#define REQUEST_BUFFER_SIZE 1000

#define IMAGE_REQUEST_TYPE  0
#define VIEWER_REQUEST_TYPE 1


static int createServer(int port);
static int waitForConnection(int server);

static void respondToClient(int socket, char *path);

static int determineRequestTypeForPath(char *path);
static void serveBitmap(int socket, char *path);
static void serveFractalViewer(int socket, char *path);



// -------------------------------------------- //
//   Server                                     //
// -------------------------------------------- //


int main(int argc, char *argv[]) {
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


int createServer(int port) {
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


int waitForConnection(int server) {
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


int determineRequestTypeForPath(char *path) {
	char *extention = strrchr(path, '.');

	if (!extention) {
		return VIEWER_REQUEST_TYPE;
	} else {
		extention++;

		if (strcmp(extention, "bmp") == 0) {
			return IMAGE_REQUEST_TYPE;
		} else {
			return VIEWER_REQUEST_TYPE;
		}
	}
}


void respondToClient(int socket, char *path) {
	int requestType = determineRequestTypeForPath(path);
	if (requestType == VIEWER_REQUEST_TYPE) {
		serveFractalViewer(socket, path);
	} else if (requestType == IMAGE_REQUEST_TYPE) {
		serveBitmap(socket, path);
	}
}


static void serveBitmap(int socket, char *path) {
	int success;

	char *header = "HTTP/1.0 200 OK\r\n"
					"Content-Type: image/bmp\r\n"
					"\r\n";

	success = write(socket, header, strlen(header));
	assert(success >= 0);

	unsigned char image[] = {
		0x42,0x4d,0x5a,0x00,0x00,0x00,0x00,0x00,
		0x00,0x00,0x36,0x00,0x00,0x00,0x28,0x00,
		0x00,0x00,0x03,0x00,0x00,0x00,0x03,0x00,
		0x00,0x00,0x01,0x00,0x18,0x00,0x00,0x00,
		0x00,0x00,0x24,0x00,0x00,0x00,0x13,0x0b,
		0x00,0x00,0x13,0x0b,0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x00,0x00,0x00,0x07,0x07,
		0xff,0x07,0x07,0x07,0x07,0x07,0xff,0x00,
		0x00,0x0e,0x07,0x07,0x07,0x66,0x07,0x07,
		0x07,0x07,0x07,0x00,0x00,0x0d,0x07,0x07,
		0x07,0x07,0x07,0x07,0xff,0xff,0xff,0x00,
		0x00,0x0d
	};

	success = write(socket, image, sizeof(image));
	assert(success >= 0);
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


int escapeSteps(double x, double y) {
	return 0;
}
