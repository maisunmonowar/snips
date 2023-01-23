#ifndef __U_H__
#define __U_H__

#include <comdef.h> // you will need this
#include <string>

class MySocketClass
{
public:
#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "7001"
#define INFO_BUFFER_SIZE 32767

	TCHAR infoBuf[INFO_BUFFER_SIZE] = {0};
	DWORD bufCharCount = INFO_BUFFER_SIZE;
	SOCKET ConnectSocket = INVALID_SOCKET;
	std::string termination_string = "terminate";
	char recvbuf[DEFAULT_BUFLEN] = {0};
	int recvbuflen = DEFAULT_BUFLEN;
	int iResult;
	enum udp_error_status
	{
		all_ok,
		connection_failed,
		wsa_startup_error,
		addrinfo_error
	};
	udp_error_status error_status_self = all_ok; // If the init wasn't successfull, this will change.

	MySocketClass();
	~MySocketClass();
	int sendData(char *ourMessage, int ourMessage_size);
	bool receiveSomething();

private:
	bool connectionActive = false;
};

#endif // !__UDP_CLIENT_H__#pragma once
