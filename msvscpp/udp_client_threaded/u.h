#ifndef __U_H__
#define __U_H__

// Ensure the Windows SDK exposes the required types in ws2tcpip.h
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0600
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <atomic>
#include <comdef.h> // you will need this
#include <condition_variable>
#include <mutex>
#include <queue>
#include <string>
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>

class MySocketClass {
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
  enum udp_error_status {
    all_ok,
    connection_failed,
    wsa_startup_error,
    addrinfo_error
  };
  udp_error_status error_status_self =
      all_ok; // If the init wasn't successfull, this will change.

  MySocketClass();
  ~MySocketClass();
  int sendData(char *ourMessage, int ourMessage_size);
  // returns true to continue, false to indicate termination message received
  bool receiveSomething();

  // Run method to be executed in udp_client thread.
  // It consumes messages from the queue, sends them and checks server replies.
  void run(std::queue<std::string> &q, std::mutex &m,
           std::condition_variable &cv, std::atomic<bool> &finished);

private:
  bool connectionActive = false;
};

#endif // __U_H__
