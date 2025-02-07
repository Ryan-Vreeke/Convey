#pragma once

#include <memory>

#include "Block.hpp"
#include "Templar.hpp"
#ifdef _WIN32
#include <windows.h>
#include <winsock2.h>  // Windows socket API
#include <ws2tcpip.h>
#else
#include <arpa/inet.h>  // Linux/Unix socket API
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#endif

#include <functional>
#include <string>
#include <unordered_map>

#include "request.h"
#include "response.h"

typedef int SOCKET;
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1

namespace Convey {
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
  void addTemplator(std::shared_ptr<Templar>, std::function<void(std::string &page_content, const std::string &page_name)>);

 private:
  bool running;
  bool templar = false;
  std::shared_ptr<Templar> templar_ptr;
  std::unordered_map<std::string, std::function<void(Request &, Response &)>> getMap;
  std::unordered_map<std::string, std::function<void(Request &, Response &)>> postMap;
  std::function<void(Request &, Response &)> WSConnected;
  std::function<void(std::string &page_content, const std::string &page_name)> templCB;

  void cleanup(SOCKET socket);
  void sendFile(const std::string &path, SOCKET client);
  void acceptClient();
  bool contains(
      const std::unordered_map<std::string, std::function<void(Request &, Response &)>> &map,
      const std::string &key);
  void handleClient(Request, Response);
};
}  // namespace Convey
