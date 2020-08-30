
//STD console files
#include "sdkddkver.h"
#include <conio.h>
#include <stdio.h>
#include <sstream>
#include <map>
#include <vector>

//socket header files
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <Windows.h>
#include <iostream>

#pragma comment(lib, "Ws2_32.lib")

#define SCK_VERSION2 0x0202
#define DEFAULT_PORT "27015"
#define DEFAULT_BUFLEN 4096

class ACCOUNTS;

class SERVER
{
public:
	SERVER();
	virtual ~SERVER();
	virtual int initialiseSocket();
	virtual int createSocket();
	virtual int bindSocket();
	virtual int listenSocket();
	virtual void sendMsg(std::string msg, SOCKET socket);
	virtual int connectMultipleClients();
	virtual int disconnectServer();

private:
	struct addrinfo* result, *ptr, hints;
	sockaddr_in client;
	int clientSize;
	SOCKET ListenSocket;
	SOCKET ClientSocket;
	WSADATA wsaData;
	int recvbuflen;
	char recvbuf[DEFAULT_BUFLEN];
	int iResult;
	std::vector<ACCOUNTS*> account;

};


