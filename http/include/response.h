#pragma once
#include <fcntl.h>
#include <netinet/in.h>
#include <string>
#include <sys/sendfile.h>
#include <unistd.h>
#include <vector>

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

  static bool ends_with(const std::string &, const std::string &);
private:
  std::string message;
  std::string contentType = "text/html"; // Default content type
  std::string version = "HTTP/1.1";      // Default HTTP version
  size_t contentLength = 0;


  void sendResponse();
  std::string generateHeaders();
};
