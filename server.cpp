#define MAX_BUFFER_SIZE 513

#include "utils.h"

#include <cstdio>
#include <cstring>
#include <iostream>
#include <string>
#include <unordered_map>
#include <thread>
#include <mutex>

const char * const SERVER_IP = "192.168.1.2";
const int SERVER_PORT = 2048;

/* clients socket and ip:port */
char ip[50];
std::unordered_map<SOCKET, std::string> clients;
std::mutex mutexClients;/* receive thread modify clients */

void clientHandler(SOCKET clientSocket) {
	int messageLength;
	char sendBuffer[MAX_BUFFER_SIZE], receiveBuffer[MAX_BUFFER_SIZE];
	while(true) {
		messageLength = recv(clientSocket, receiveBuffer, MAX_BUFFER_SIZE, 0);

		if(messageLength <= 0) {/* receive error */
			printError("recv");
			std::unique_lock<std::mutex> lock(mutexClients);
			clients.erase(clientSocket);
			closeSocket(clientSocket);
			break;
		}

		receiveBuffer[messageLength] = '\0';
		printf("%s > %s\n", clients[clientSocket].c_str(), receiveBuffer);
		std::string echo = std::string("Echo ") + receiveBuffer;
		sprintf(sendBuffer, "%s", echo.c_str());
		if(-1 == send(clientSocket, sendBuffer, echo.size(), 0)) {
			printError("send");
			std::unique_lock<std::mutex> lock(mutexClients);
			clients.erase(clientSocket);
			closeSocket(clientSocket);
			break;
		}
	}/* end while */
}

int main(int argc, char **argv) {
	startUp();

	/* create socket */
	SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(-1 == serverSocket) {
		printError("socket");
		return 0;
	}

	/* set server ip:port */
	struct sockaddr_in serverAddress;
	memset(&serverAddress, 0, sizeof(serverAddress));
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = inet_addr(SERVER_IP);
	serverAddress.sin_port = htons(SERVER_PORT);

	/* server bind ip:port */
	if(-1 == bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress))) {
		printError("bind");
		return 0;
	}

	/* listen to clients */
	printf("Server listening...\n");
	if(-1 == listen(serverSocket, 20)) {
		printError("listen");
		return 0;
	}

	/* receive from clients */
	struct sockaddr_in clientAddress;
	socklen_t clientAddressSize = sizeof(clientAddress);

	while(true) {
		SOCKET clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddress, &clientAddressSize);
		if(-1 == clientSocket) {
			printError("accept");
			break;
		}

		/* ip:port of clients */
		char *clientIp = inet_ntoa(clientAddress.sin_addr);
		int clientPort = ntohs(clientAddress.sin_port);
		sprintf(ip, "%s:%d", clientIp, clientPort);

		std::unique_lock<std::mutex> lock(mutexClients);
		clients[clientSocket] = std::string(ip);

		printf("Accept %s\n", ip);
		std::thread handler(clientHandler, clientSocket);
		handler.detach();
	}

	closeSocket(serverSocket);

	cleanUp();
	return 0;
}