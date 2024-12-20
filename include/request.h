#pragma once

#include <netinet/in.h>

#include <string>
#include <unordered_map>
#include <vector>

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
