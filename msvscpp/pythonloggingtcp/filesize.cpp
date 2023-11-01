#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <nlohmann/json.hpp>
#include <fstream>
#include <string>
#include <iostream>
#include <chrono>


// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")


#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "9020"
#define HOST "localhost"

enum errorLevel { NOTSET=0, DEBUG=10, INFO=20, WARNING=30, ERRORR=40, CRITICAL=50};

int sendLog(
    std::string msg, 
    std::string name, 
    int levelno, 
    std::string filename, 
    std::string modulee, 
    std::string funcName,
    std::string pathname) 
{
    // get the path from json file
    using json = nlohmann::json;
    //std::ifstream f("unpickled.json");
    //json data2 = json::parse(f);
    //f.close();

    json data = json::parse(R"(
    {
    "name": "root",
    "msg": "empty",
    "args": null,
    "levelname": "INFO",
    "levelno": 20,
    "pathname": "C:\\Users\\MaisunIbnMonowar\\Documents\\code\\dektec_demod\\pythonScripts\\sender.py",
    "filename": "empty",
    "module": "empty",
    "exc_info": null,
    "exc_text": null,
    "stack_info": null,
    "lineno": 0,
    "funcName": "empty",
    "created": 0.0,
    "msecs": 0,
    "relativeCreated": 0,
    "thread": 0,
    "threadName": "empty",
    "processName": "empty",
    "process": 0
}
)");
  
    data["name"] = name;
    data["msg"] = msg;

    switch (levelno)
    {
    case CRITICAL:
        data["levelname"] = "CRITICAL";
        break;
    case ERRORR:
        data["levelname"] = "ERROR";
        break;
    case WARNING:
        data["levelname"] = "WARNING";
        break;
    case INFO:
        data["levelname"] = "INFO";
        break;
    case DEBUG:
        data["levelname"] = "DEBUG";
        break;
    default:
        data["levelname"] = "NOTSET";
        break;
    }
    data["filename"] = filename;
    data["module"] = modulee;
    data["funcName"] = funcName; 
    data["pathname"] = pathname;

    data["created"] = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    
    //int processs = data["process"];
    ///int threadd = data["thread"];
    //std::cout << processs + threadd;
    //std::string json_str = data.dump();

    //std::cout << data["name"] << std::endl;
    //std::cout << funcName << std::endl;
    //data["name"] = funcName;
    //std::cout << data["name"] << std::endl;

    // get the path form config
    //std::string pathRaw = data["data_location"].dump();

    WSADATA wsaData;
    SOCKET ConnectSocket = INVALID_SOCKET;
    struct addrinfo* result = NULL,
        * ptr = NULL,
        hints;
    const char* sendbuf = "this is a test";
    char recvbuf[DEFAULT_BUFLEN];
    int iResult;
    int recvbuflen = DEFAULT_BUFLEN;

    // Validate the parameters
//    if (argc != 2) {
  //      printf("usage: %s server-name\n", argv[0]);
    //    return 1;
    //}

    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed with error: %d\n", iResult);
        return 1;
    }

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    // Resolve the server address and port
    iResult = getaddrinfo(HOST, DEFAULT_PORT, &hints, &result);
    if (iResult != 0) {
        printf("getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();
        return 1;
    }

    // Attempt to connect to an address until one succeeds
    for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {

        // Create a SOCKET for connecting to server
        ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype,
            ptr->ai_protocol);
        if (ConnectSocket == INVALID_SOCKET) {
            printf("socket failed with error: %ld\n", WSAGetLastError());
            WSACleanup();
            return 1;
        }

        // Connect to server.
        iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (iResult == SOCKET_ERROR) {
            closesocket(ConnectSocket);
            ConnectSocket = INVALID_SOCKET;
            continue;
        }
        break;
    }

    freeaddrinfo(result);

    if (ConnectSocket == INVALID_SOCKET) {
        printf("Unable to connect to server!\n");
        WSACleanup();
        return 1;
    }

    // Send an initial buffer
    auto s1 = data.dump();
    sendbuf = s1.c_str(); // json data is now serialized
    int slen = (int)strlen(sendbuf);
    char clen[4];
    clen[3] = slen & 0x000000ff;
    clen[2] = (slen >> 8) & 0x000000ff;
    clen[1] = (slen >> 16) & 0x000000ff;
    clen[0] = (slen >> 24) & 0x000000ff;
    iResult = send(ConnectSocket, clen, 4, 0);
    iResult = send(ConnectSocket, sendbuf, (int)strlen(sendbuf), 0);
    if (iResult == SOCKET_ERROR) {
        printf("send failed with error: %d\n", WSAGetLastError());
        closesocket(ConnectSocket);
        WSACleanup();
        return 1;
    }

    printf("Bytes Sent: %ld\n", iResult);

    // shutdown the connection since no more data will be sent
    iResult = shutdown(ConnectSocket, SD_SEND);
    if (iResult == SOCKET_ERROR) {
        printf("shutdown failed with error: %d\n", WSAGetLastError());
        closesocket(ConnectSocket);
        WSACleanup();
        return 1;
    }

    // Receive until the peer closes the connection
    do {

        iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
        if (iResult > 0)
            printf("Bytes received: %d\n", iResult);
        else if (iResult == 0)
            printf("Connection closed\n");
        else
            printf("recv failed with error: %d\n", WSAGetLastError());

    } while (iResult > 0);

    // cleanup
    closesocket(ConnectSocket);
    WSACleanup();


    return 0;
}
int __cdecl main(int argc, char** argv)
{
    sendLog("test info", "root", INFO, "filename", "module", "funcName", "pathname");
    sendLog("test critical", "root", CRITICAL, "anotherFilename", "anothermodule", "aotherfunc", "anotherPath");

    return 0;
}