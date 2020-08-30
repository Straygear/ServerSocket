#include "SERVER.h"
#include "ACCOUNTS.h"

SERVER::SERVER(): result(0), ptr(0), iResult(0) {
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;


	ListenSocket = INVALID_SOCKET;
	ClientSocket = INVALID_SOCKET;

	recvbuf[DEFAULT_BUFLEN];
	recvbuflen = DEFAULT_BUFLEN;

	clientSize = sizeof(client);
}

SERVER::~SERVER(){}

int SERVER::initialiseSocket() {
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		std::cout << "WSAStartup failed: " << iResult << std::endl;
		return 1;
	}
	std::cout << "Socket Initialised!" << std::endl;
	return iResult;
}

int SERVER::createSocket() {
	iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
	if (iResult != 0) {
		std::cout << "getaddrinfo failed: " << iResult << std::endl;
		WSACleanup();
		return 1;
	}

	//create a SOCKET for the server to listen for client connections
	ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

	if (ListenSocket == INVALID_SOCKET) {
		std::cout << "Error at socket(): " << WSAGetLastError() << std::endl;
		freeaddrinfo(result);
		WSACleanup();
		return 1;
	}
	std::cout << "address is: " << result->ai_addr << std::endl;
	std::cout << "Socket created!" << std::endl;
	return iResult;
}

int SERVER::bindSocket() {
	//bind socket to localhost
	iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		std::cout << "bind failed with error " << WSAGetLastError() << std::endl;
		freeaddrinfo(result);
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}
	std::cout << "Socket binded!" << std::endl;
	return iResult;
}

int SERVER::listenSocket() {
	if (listen(ListenSocket, SOMAXCONN) == SOCKET_ERROR) {
		std::cout << "Listen failed with error: " << WSAGetLastError() << std::endl;
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}
	//wait for a connection
	std::cout << "Listning...." << std::endl;
	return iResult;
}

int SERVER::connectMultipleClients() {
	char host[NI_MAXHOST];
	char service[NI_MAXHOST];

	std::map<SOCKET, std::string> socketname;
	std::map<SOCKET, bool> sockMap;
	std::pair<SOCKET, bool> sockets;
	std::string name;

	fd_set master;
	FD_ZERO(&master);
	FD_SET(ListenSocket, &master);

	while (true) {
		fd_set copy = master;

		int socketCount = select(0, &copy, nullptr, nullptr, nullptr); //here is where the magic happens

		for (int i = 0; i < socketCount; i++) {
			SOCKET sock = copy.fd_array[i];
			if (sock == ListenSocket) {

				//accept new connection
				ClientSocket = accept(ListenSocket, (sockaddr*)&client, &clientSize);
				if (ClientSocket == INVALID_SOCKET) {
					std::cout << "accept failed: " << WSAGetLastError() << std::endl;
					closesocket(ListenSocket);
					WSACleanup();
					return 1;
				}
				std::cout << "connection accepted!" << std::endl;

				//add the new connection to the list of connected clients
				FD_SET(ClientSocket, &master);
				sockets.first = sock;
				sockets.second = false;
				sockMap.insert(sockets);

				//send a welcome message to the connected client
				std::ostringstream st;
				st << "Welcome SOCKET #" << ClientSocket << " to the chat server!\r\n" << "please enter your name: " << "\r\n";
				std::string msg = st.str();

				//TODO: broadcast we have a new connection
				for(int i = 0; i < master.fd_count; i++) {
			
					SOCKET broadSock = master.fd_array[i];
					if (broadSock != ListenSocket && broadSock != ClientSocket) {
						std::ostringstream br;
						br << "\r\n" << "SOCKET #" << ClientSocket << " has connected to the server!\r\n";
						std::string msgtwo = br.str();
						sendMsg(msgtwo, broadSock);
					}
				}
					ZeroMemory(host, NI_MAXHOST);
					ZeroMemory(service, NI_MAXHOST);
					if (getnameinfo((sockaddr*)&client, sizeof(client), host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0) {
						std::cout << host << " connected on port " << service << std::endl;
						sendMsg(msg, ClientSocket);
					}
					else {
						inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
						std::cout << host << " connected on host " << ntohs(client.sin_port) << std::endl;
						sendMsg(msg, ClientSocket);
					}
				}
			else {
				ZeroMemory(recvbuf, recvbuflen);
				//recieve message
				int bytesIn = recv(sock, recvbuf, recvbuflen, 0);
				if (bytesIn <= 0) {
					//drop the client
					closesocket(sock);
					FD_CLR(sock, &master);
				}
				if (!sockMap[sock]) {
					std::ostringstream sg;
					sg << recvbuf;
					//name = std::string(recvbuf, 0, bytesIn);
					name = sg.str();
					sockMap[sock] = true;
					socketname[sock] = name;
					
				}
				else {
					for (int i = 0; i < master.fd_count; i++) {
						SOCKET outSock = master.fd_array[i];
						//send message to other clients, and definitly NOT the listening socket
						if (outSock != ListenSocket && outSock != sock) {

							std::ostringstream ss;

							ss << socketname[sock] << ": " << recvbuf << "\n\r";
							std::string strOut = ss.str();

							sendMsg(strOut, outSock);
						}
					}
				}
			}
		}
	}
}

void SERVER::sendMsg(std::string msg, SOCKET socket) {
	int isendresult = send(socket, msg.c_str(), msg.size() + 1, 0);
	if (isendresult == SOCKET_ERROR) {
		std::cout << "send failed: " << WSAGetLastError() << std::endl;
		WSACleanup();
	}
}
//int SERVER::recieveSendData() {
//	do {
//		ZeroMemory(recvbuf, recvbuflen);
//
//		iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
//		if (iResult > 0) {
//			std::cout << "Bytes recieved: " << iResult << std::endl;
//			std::cout << "CLIENT> " << std::string(recvbuf, 0, iResult) << std::endl;
//
//			//Echo the buffer back to the sender
//			int isendresult = send(ClientSocket, recvbuf, iResult + 1, 0);
//			if (isendresult == SOCKET_ERROR) {
//				std::cout << "send failed: " << WSAGetLastError() << std::endl;
//				closesocket(ClientSocket);
//				WSACleanup();
//				return 1;
//			}
//			std::cout << "Bytes sent " << isendresult << std::endl;
//		}
//		else if (iResult == 0) {
//			std::cout << "Connection closing...\n" << std::endl;
//			return iResult;
//		}
//		else {
//			std::cout << "recv failed: " << WSAGetLastError() << std::endl;
//			closesocket(ClientSocket);
//			WSACleanup();
//			return 1;
//		}
//	} while (true);
//}

int SERVER::disconnectServer() {
	iResult = shutdown(ClientSocket, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		std::cout << "shutdown failed: " << WSAGetLastError << std::endl;
		closesocket(ClientSocket);
		WSAGetLastError();
		return 1;
	}
	std::cout << "shuting down" << std::endl;
	return iResult;
}