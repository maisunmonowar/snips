#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0600
#endif
#define WIN32_LEAN_AND_MEAN // do not move this line.

#include <iostream>
#include <chrono>
#include <thread>
#include <atomic>
#include <windows.h>		// include windows types
#include <winsock2.h>		// Winsock2 must be included before ws2tcpip
#include <ws2tcpip.h>		// for socket functions.
#include <stdlib.h>
#include <stdio.h>
#include <NetSh.h>
#include "u.h"

// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "Mswsock.lib")
#pragma comment(lib, "AdvApi32.lib")

MySocketClass::MySocketClass()
{
	printf("mySocketClass::Constructor\n");
	PCSTR hostname = "127.0.0.1";
	WSADATA wsaData;
	struct addrinfo *result = NULL,
					*ptr = NULL,
					hints;

	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0)
	{
		printf("mySocketClas::Constructor -> wsa startup error\n");
		error_status_self = wsa_startup_error;
		return;
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
		WSACleanup();
		return;
	}

	// Attempt to connect to an address until one succeeds
	for (ptr = result; ptr != NULL; ptr = ptr->ai_next)
	{
		// Create a SOCKET for connecting to server
		ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
		if (ConnectSocket == INVALID_SOCKET) {
			continue;
		}

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

	if (!connectionActive) {
		error_status_self = connection_failed;
		// leave WSA running; destructor will cleanup if needed
		return;
	}

	// setting timeout value (milliseconds)
	DWORD ti = 500; // 500 ms
	setsockopt(ConnectSocket, SOL_SOCKET, SO_RCVTIMEO, (const char *)&ti, sizeof(ti));
}

MySocketClass::~MySocketClass()
{
	printf("mySocketClas::destructor\n");
	if (connectionActive)
	{
		printf("mySocketClas::destructor-> connetction was active\n");
		// cleanup
		iResult = shutdown(ConnectSocket, SD_SEND);
		if (iResult == SOCKET_ERROR)
		{
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
	std::cout << "\tMySocketClass::receive" << std::endl;
	if (error_status_self != all_ok)
	{
		return true; // connection error exists. returning default response so caller continues.
	}

	int bytes = recv(ConnectSocket, recvbuf, recvbuflen - 1, 0);
	if (bytes == SOCKET_ERROR)
	{
		int err = WSAGetLastError();
		// timeout or other error: just return true to continue running
		if (err == WSAETIMEDOUT)
		{
			// no data available right now
			// std::cout << "recv timed out." << std::endl;
			return true;
		}
		std::cout << "recv failed with error: " << err << std::endl;
		return true;
	}
	if (bytes == 0)
	{
		// no data / remote closed - for UDP this is unusual; continue
		return true;
	}
	recvbuf[bytes] = '\0';
	std::cout << "Received: " << recvbuf << std::endl;

	// check for termination message
	size_t klen = termination_string.size();
	if ((size_t)bytes >= klen && std::string(recvbuf, klen) == termination_string)
	{
		std::cout << "\trecorder::receiveSomething -> self terminating. " << std::endl;
		time_to_quit = true;
		return false; // signal caller to terminate
	}
	else
	{
		std::cout << "\trecorder::receiveSomething -> continuing." << std::endl;
		return true; // stay alive
	}
}

int MySocketClass::sendData(char *ourMessage, int ourMessage_insize)
{

	std::cout << "\tmySocketClass::sendData" << std::endl;
	if (ConnectSocket == INVALID_SOCKET)
	{
		printf("sendDAta -> invalid socket\n");
		return -1;
	}

	if (error_status_self != all_ok)
	{
		return -2; // connection error exists. returning default response.
	}

	if (connectionActive)
	{
		int sent = send(ConnectSocket, ourMessage, ourMessage_insize, 0);
		if (sent == SOCKET_ERROR) {
			std::cout << "send failed with err: " << WSAGetLastError() << std::endl;
			return -4;
		}
		std::cout << "connection was active and message sent" << std::endl;
	}
	else
	{
		std::cout << "\tMySocketClass::senddata -> Not Sending the data cause connection not active." << std::endl;
		return -3;
	}
	return 0;
}

bool MySocketClass::should_i_quit(){
	return time_to_quit;
}