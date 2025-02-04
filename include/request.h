#ifndef REQUEST_H
#define REQUEST_H

#ifdef _WIN32
#include <winsock2.h>
#else
#include <netinet/in.h>
#endif

#include <string>
#include <unordered_map>
#include <vector>

namespace Convey {
class Request {
public:
  std::string method;
  std::string httpVersion;
  std::string path;
  std::string host;
  std::string body;
  std::unordered_map<std::string, std::string> headers;
  std::unordered_map<std::string, std::string> args;

  sockaddr_in clientAddress;
  int clientSocket;

  Request(const std::string &);
};

};

#endif