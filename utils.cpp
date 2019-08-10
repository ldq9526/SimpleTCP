#include "utils.h"
#include <cstdio>
#include <cerrno>

void startUp() {
#ifdef WINDOWS
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif
}

void cleanUp() {
#ifdef WINDOWS
	WSACleanup();
#endif
}

void printError(const char *e) {
#ifdef WINDOWS
	printf("Error: fail to %s\n", e);
#else
	perror(e);
#endif
}

void closeSocket(SOCKET fd) {
	if(fd != -1) {
#ifdef WINDOWS
		closesocket(fd);
#else
		close(fd);
#endif
	}
}

int getErrorCode() {
#ifdef WINDOWS
	return WSAGetLastError();
#else
	return errno;
#endif
}

/* set send and receive timeout, t seconds */
void setTimeOut(SOCKET fd, int t) {
#ifdef WINDOWS
	int timeout = t * 1000;
	setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, (const char *)&timeout, sizeof(int));
	setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, (const char *)&timeout, sizeof(int));
#else
	struct timeval timeout;
	timeout.tv_sec = t;
	timeout.tv_usec = 0;
	setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(struct timeval));
	setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(struct timeval));
#endif
}