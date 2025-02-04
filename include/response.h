#pragma once

#include <string>
#include <unistd.h>
#include <vector>

#ifdef _WIN32 // Windows-specific headers
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#else // Linux-specific headers
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/sendfile.h>
#endif

namespace Convey {

class Response {
public:
  const int clientSocket;
  const sockaddr_in clientAddress;
  std::vector<std::string> headers;
  std::string code = "200 OK";

  Response(int clientSocket, sockaddr_in clientAddress);

  void send(const std::string &body);
  void json(const std::string &json);
  void type(const std::string &type);
  void sendFile(const std::string &path);

private:
  std::string message;
  std::string contentType = "text/html"; // Default content type
  std::string version = "HTTP/1.1";      // Default HTTP version
  size_t contentLength = 0;

  std::string findFileWithExtension(std::string basePath);

  void sendResponse();
  std::string generateHeaders();
};
} // namespace Convey
