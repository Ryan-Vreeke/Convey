#pragma once

#include <unistd.h>

#include <string>
#include <unordered_map>
#include <vector>

#ifdef _WIN32  // Windows-specific headers
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#else  // Linux-specific headers
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/sendfile.h>
#include <sys/socket.h>
#include <unistd.h>
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
  std::string findFileWithExtension(std::string basePath);

 private:
  std::string message;
  std::string contentType = "text/html";  // Default content type
  std::string version = "HTTP/1.1";       // Default HTTP version
  size_t contentLength = 0;
  std::unordered_map<std::string, std::string> typeSet = {
      {"html", "text/html"},         {"jpg", "image/jpeg"},
      {"jpeg", "image/jpeg"},        {"png", "image/png"},
      {"css", "text/css"},           {"svg", "image/svg+xml"},
      {"ts", "video/MP2T"},          {"mp4", "video/mp4"},
      {"m3u8", "vnd.apple.mpegURL"}, {"js", "application/javascript"}};

  void sendResponse();
  std::string generateHeaders();
};
}  // namespace Convey
