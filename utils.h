#ifndef UTILS_H
#define UTILS_H

#ifdef WINDOWS
#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")
typedef int socklen_t;
#else
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <arpa/inet.h>
typedef int SOCKET;
#endif

/* initialize DLL call */
void startUp();

/* clean up DLL */
void cleanUp();

/* print error information */
void printError(const char *e);

/* close socket */
void closeSocket(SOCKET fd);

/* get error code */
int getErrorCode();

/* set send and receive timeout: t(seconds) */
void setTimeOut(SOCKET fd, int t);

#endif