#include "Websocket.h"
#include <algorithm>
#include <cstdint>
#include <cstring>
#include <iostream>

#ifdef _WIN32
void initializeWinsock() {
  WSADATA wsaData;
  if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
    std::cerr << "WSAStartup failed." << std::endl;
    exit(EXIT_FAILURE);
  }
}

void cleanupWinsock() { WSACleanup(); }
#endif

Websocket::Websocket() { serverSocket = -1; }

Websocket::~Websocket() {
  stop();
#ifdef _WIN32
  cleanupWinsock();
#endif
}

bool Websocket::start(int port) {
#ifdef _WIN32
  initializeWinsock();
#endif
  // Create a TCP socket
  serverSocket = socket(AF_INET, SOCK_STREAM, 0);
  if (serverSocket < 0) {
    std::cerr << "Failed to create socket." << std::endl;
    return false;
  }

  sockaddr_in serverAddr;
  memset(&serverAddr, 0, sizeof(serverAddr));
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_addr.s_addr = INADDR_ANY;
  serverAddr.sin_port = htons(port);

  if (bind(serverSocket, (sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
    std::cerr << "Bind failed." << std::endl;
    CLOSE_SOCKET(serverSocket);
    return false;
  }

  // Listen for incoming connections
  if (listen(serverSocket, 5) < 0) {
    std::cerr << "Listen failed." << std::endl;
    CLOSE_SOCKET(serverSocket);
    return false;
  }

  return true;
}

void Websocket::stop() {
  if (serverSocket != -1) {
    CLOSE_SOCKET(serverSocket);
    serverSocket = -1;
  }
}

void Websocket::handleClient() {
  sockaddr_in clientAddr;
  socklen_t clientAddrLen = sizeof(clientAddr);
  SocketType clientSocket = accept(serverSocket, (sockaddr *)&clientAddr, &clientAddrLen);

  if (clientSocket < 0) {
    std::cerr << "Accept failed." << std::endl;
    return;
  }

  std::cout << "Client connected!" << std::endl;

  // Receive data from the client
  char buffer[1024];
  int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
  if (bytesReceived > 0) {
    std::cout << "Received: " << std::string(buffer, bytesReceived) << std::endl;
  }

  CLOSE_SOCKET(clientSocket);
}
