#include <iostream>
#include <chrono>
#include <thread>
#include <atomic>
#define WIN32_LEAN_AND_MEAN // do not move this line. 
#include <windows.h>		// or this one
#include <winsock2.h>		// The sequence is important
#include <ws2tcpip.h>		// for socket functions. 
#include <stdlib.h>			
#include <stdio.h>
#include <NetSh.h>
#include "u.h"

// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

MySocketClass::MySocketClass()
{
	printf("mySocketClass::Constructor\n");
	PCSTR hostname = "127.0.0.1";
	WSADATA wsaData;
	struct addrinfo* result = NULL,
		* ptr = NULL,
		hints;


	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0)
	{
		printf("mySocketClas::Constructor -> wsa startup error\n");
		error_status_self = wsa_startup_error;
	}
	ZeroMemory(&hints, sizeof(hints)); // init memory to zero. 
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_protocol = IPPROTO_UDP;

	// Resolve the server address and port
	auto a = getaddrinfo(hostname, DEFAULT_PORT, &hints, &result);
	if (a != 0)
	{
		error_status_self = addrinfo_error;
	}

	// Attempt to connect to an address until one succeeds
	for (ptr = result; ptr != NULL; ptr = ptr->ai_next)
	{
		// Create a SOCKET for connecting to server
		ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);

		// Connect to server.
		iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
		if (iResult == SOCKET_ERROR)
		{
			closesocket(ConnectSocket);
			ConnectSocket = INVALID_SOCKET;
			continue;
		}
		printf("mySocketClas::Constructor -> conncetion success\n");
		connectionActive = true;
		break;
	}

	freeaddrinfo(result); // because you called getaddrinfo()

	// setting timeout value
	struct timeval tv;
	tv.tv_usec = 500000; // half sec
	DWORD ti = 400; // time out value. unit unknown.
	//const char* t = &ti;
	setsockopt(		ConnectSocket,	SOL_SOCKET,		SO_RCVTIMEO,		(char *)&ti,		sizeof(ti)	);
}

MySocketClass::~MySocketClass()
{
	printf("mySocketClas::destructor\n");
	if (connectionActive)
	{
		printf("mySocketClas::destructor-> connetction was active\n");
		// cleanup
		iResult = shutdown(ConnectSocket, SD_SEND);
		if (iResult == SOCKET_ERROR) {
			printf("mySocketClas::destructor -> shutdown failed with error: %d\n", WSAGetLastError());
					closesocket(ConnectSocket);
				WSACleanup();
		}
		closesocket(ConnectSocket);
		WSACleanup();
		connectionActive = false;
		printf("mySocketClas::destructor->cleanup done. \n");
	}
}

bool MySocketClass::receiveSomething()
{
	if (error_status_self != all_ok)
	{
		return true; // connection error exists. returning default respose.
	}
	std::cout << "\tMySocketClass::receive" << std::endl;
	//iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
	iResult = recv(ConnectSocket, recvbuf, recvbuflen, MSG_PEEK);
	if (iResult < 1)
	{
		std::cout << "peeked." <<std::endl;
		return true;
	}
	iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
	if (iResult < 9)
	{
#ifdef _DEBUG
		std::cout << "less than 9 bytes on rx buffer." << std::endl;
#endif
		return true;
	}
	std::cout << recvbuf << std::endl;
	std::string killCode = "terminate";
	std::string commandReceived = std::string(recvbuf, 9);
	if (0 == killCode.compare(commandReceived))
	{
#ifdef _DEBUG
		std::cout << "\trecorder::receiveSomething -> self terminating. " << std::endl;
#endif
		return false; // time to terminate the process
	}
	else
	{
		std::cout << "\trecorder::receiveSomething -> Gammabarimasu. \n";
		return true; // stay allive the process.  
	}
}

int MySocketClass::sendData(char* ourMessage, int ourMessage_insize)
{
	
		std::cout << "\tmySocketClass::sendData" << std::endl;
	if (ConnectSocket == INVALID_SOCKET)
	{
		printf("sendDAta -> invalid sockt\n"); return -1;
	}
	else
	{
		if (error_status_self != all_ok)
		{
			return -2; // connection error exists. returning default respose.
		}
	}
	if (connectionActive)
	{
		send(ConnectSocket, ourMessage, ourMessage_insize, 0);
		std::cout << "ocnnection was aactive and message sent" << std::endl;
	}
	else
	{
		std::cout << "\tMySocketClass::senddata -> Not Sending the data cause connection not acitve." << std::endl;
		return -3;
	}
	return 0;
}
