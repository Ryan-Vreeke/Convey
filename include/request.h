#pragma once

#include <netinet/in.h>
#include <string>
#include <unordered_map>
#include <vector>

class Request {
public:
  std::string method;
  std::string httpVersion;
  std::unordered_map<std::string, std::string> headers;
  std::string path;
  std::string host;
  std::string body;

  sockaddr_in clientAddress;
  int clientSocket;

  Request(const std::string &);

private:
  std::vector<std::string> splitString(const std::string &,
                                       const std::string &);
};
