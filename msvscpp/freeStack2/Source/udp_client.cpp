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
#include "udp_client.h"

// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "Mswsock.lib")
#pragma comment(lib, "AdvApi32.lib")

MySocketClass::MySocketClass()
{
	PCSTR hostname = "localhost";
	WSADATA wsaData;
	struct addrinfo *result = NULL,
					*ptr = NULL,
					hints;

	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0)
	{
#ifdef _DUBUG
		std::cout << "mySocketClas::Constructor -> wsa startup error\n";
#endif
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
		connectionActive = true;
		break;
	}

	freeaddrinfo(result); // because you called getaddrinfo()

	// setting timeout value
	DWORD ti = 400; // time out value. unit unknown.
	setsockopt(ConnectSocket, SOL_SOCKET, SO_RCVTIMEO, (char *)&ti, sizeof(ti));
}

MySocketClass::~MySocketClass()
{
	if (connectionActive)
	{
		iResult = shutdown(ConnectSocket, SD_SEND);
		if (iResult == SOCKET_ERROR)
		{
#ifdef _DEBUG
			printf("mySocketClas::destructor -> shutdown failed with error: %d\n", WSAGetLastError());
#endif
			closesocket(ConnectSocket);
			WSACleanup();
		}
		closesocket(ConnectSocket);
		WSACleanup();
		connectionActive = false;
	}
}

bool MySocketClass::receiveSomething()
{
	if (error_status_self != all_ok)
	{
		return true; // connection error exists. returning default respose.
	}

	iResult = recv(ConnectSocket, recvbuf, recvbuflen, MSG_PEEK);
	if (iResult < 1)
	{
		//std::cout << "peeked." << std::endl;
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
		return true; // stay allive the process.
	}
}

int MySocketClass::sendData(char *ourMessage, int ourMessage_insize)
{
	if (ConnectSocket == INVALID_SOCKET)
	{
		return -1;
	}

	if (error_status_self != all_ok)
	{
		return -2; // connection error exists. returning default respose.
	}

	if (connectionActive)
	{
		send(ConnectSocket, ourMessage, ourMessage_insize, 0);
	}
	else
	{
#ifdef _DEBUG
		std::cout << "\tMySocketClass::senddata -> Not Sending the data cause connection not acitve." << std::endl;
#endif
		return -3;
	}
	return 0;
}