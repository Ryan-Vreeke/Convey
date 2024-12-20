#pragma once

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <functional>
#include <string>
#include <unordered_map>

#include "request.h"
#include "response.h"

typedef int SOCKET;
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1

class Server {
 public:
  sockaddr_in address;
  const SOCKET server_fd;
  std::string publicDir;
  static constexpr int BUF_SIZE = 4096;

  Server();
  Server(const std::string);
  ~Server();

  void get(std::string path, std::function<void(Request &, Response &)> func);
  void post(std::string path, std::function<void(Request &, Response &)> func);
  void startListen(int port, std::function<void(void)> listenLoop);

  void addWSObserver(std::function<void(Request &, Response &)>);

 private:
  bool running;
  std::unordered_map<std::string, std::function<void(Request &, Response &)>> getMap;
  std::unordered_map<std::string, std::function<void(Request &, Response &)>> postMap;
  std::function<void(Request &, Response &)> WSConnected;

  void cleanup(SOCKET socket);
  void sendFile(const std::string &path, SOCKET client);
  void acceptClient();
  bool contains(
      const std::unordered_map<std::string, std::function<void(Request &, Response &)>> &map,
      const std::string &key);
  void handleClient(Request, Response);
};
