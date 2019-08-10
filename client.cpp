#define MAX_BUFFER_SIZE 513

#include "utils.h"

#include <cstdio>
#include <cstring>
#include <iostream>
#include <unordered_map>
#include <string>
#include <thread>
#include <mutex>

const char * const SERVER_IP = "192.168.1.2";
const int SERVER_PORT = 2048;

/* thread related variables */
bool running = true;/* running or not */
std::mutex mutexRunning;/* receive thread modify running */
std::thread receiveThread;/* keep receive */

/* receive and send buffers */
char sendBuffer[MAX_BUFFER_SIZE], receiveBuffer[MAX_BUFFER_SIZE];

void receiveHandler(SOCKET serverSocket) {
	while(running) {
		int messageLength = recv(serverSocket, receiveBuffer, MAX_BUFFER_SIZE, 0);
		if(messageLength == 0) {
			std::unique_lock<std::mutex> lock(mutexRunning);
			if(running)
				running = false;
			printf("%s:%d closes\n", SERVER_IP, SERVER_PORT);
			break;
		} else if(messageLength < 0) {
			int errorCode = getErrorCode();

			/* timeout */
#ifdef WINDOWS
			if(errorCode == WSAETIMEDOUT)
				continue;
#else
			if(errorCode == EINTR || errorCode == EWOULDBLOCK || errorCode == EAGAIN)
				continue;
#endif

			/* error */
			std::unique_lock<std::mutex> lock(mutexRunning);
			if(running)
				running = false;
			printError("recv");
			break;
		}
		receiveBuffer[messageLength] = '\0';
		printf("%s:%d > %s\n", SERVER_IP, SERVER_PORT, receiveBuffer);
	}
}

int main(int argc, char **argv) {
	startUp();

	SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(-1 == serverSocket) {
		printError("socket");
		return 0;
	}
	setTimeOut(serverSocket, 3);

	struct sockaddr_in serverAddress;
	memset(&serverAddress, 0, sizeof(serverAddress));
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = inet_addr(SERVER_IP);
	serverAddress.sin_port = htons(SERVER_PORT);

	if(-1 == connect(serverSocket, (struct sockaddr*)&serverAddress, sizeof(struct sockaddr))) {
		printError("connect");
		return 0;
	}

	printf("Connect %s:%d\n", SERVER_IP, SERVER_PORT);
	receiveThread = std::thread(receiveHandler, serverSocket);

	std::string line;
	while(running) {
		std::getline(std::cin, line);
		if(line.empty())
			continue;

		if(line == "exit") {
			std::unique_lock<std::mutex> lock(mutexRunning);
			if(running)
				running = false;
			break;
		}

		sprintf(sendBuffer, "%s", line.c_str());
		int messageLength = send(serverSocket, sendBuffer, line.size(), 0);
		if(messageLength < 0) {
			int errorCode = getErrorCode();

			bool timeout = false;
#ifdef WINDOWS
			if(errorCode == WSAETIMEDOUT)
				timeout = true;
#else
			if(errorCode == EINTR || errorCode == EWOULDBLOCK || errorCode == EAGAIN)
				timeout = true;
#endif

			if(!timeout) {
				std::unique_lock<std::mutex> lock(mutexRunning);
				if(running)
					running = false;
				printError("send");
			}
		}
	}

	closeSocket(serverSocket);

	receiveThread.join();

	cleanUp();

	return 0;
}