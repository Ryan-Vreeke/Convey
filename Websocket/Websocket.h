#pragma once

#ifdef _WIN32
    #define _WIN32_WINNT 0x0601
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
#else
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    #include <cstring>  
#endif

#include <cstdint>
#include <string>

#ifdef _WIN32
    void initializeWinsock();
    void cleanupWinsock();
    typedef SOCKET SocketType;
    #define CLOSE_SOCKET closesocket
#else
    typedef int SocketType;
    #define CLOSE_SOCKET close
#endif

class Websocket {
public:
  Websocket();
  ~Websocket();
  Websocket &operator=(Websocket &); 
  Websocket &operator=(Websocket &&);

  bool start(int port);
  void stop();
  void handleClient();

private:
  SocketType serverSocket;
};
