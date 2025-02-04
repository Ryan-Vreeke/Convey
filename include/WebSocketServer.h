#pragma once
#include <cstdint>
#include <functional>
#include <memory>
#include <unordered_map>
#include <vector>

#include "Convey.h"
#include "WebSocket.h"
#include "request.h"
#include "response.h"

namespace Convey {

class WebSocketServer {
 public:
  const std::string magic = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";

  WebSocketServer(Server &server);

  void onConnect(std::function<void(std::shared_ptr<WebSocket>)> callback);
  void onDisconnect(int wSocket);

 private:
  Server &m_server;
  std::unordered_map<int, std::shared_ptr<WebSocket>> m_clients;
  std::function<void(std::shared_ptr<WebSocket>)> m_clientConnect;
  std::function<void(std::shared_ptr<WebSocket>)> m_clientDisconnect;

  void clientConnected(Request &, Response &);

  std::string sha1(const std::string &input);
  std::string base64_encode(const std::string &input);
  std::string base64_encode(const std::vector<uint8_t> &input);
};
};  // namespace Convey
