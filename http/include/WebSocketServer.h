#pragma once
#include "WebSocket.h"
#include "request.h"
#include "response.h"
#include "server.h"
#include <functional>
#include <vector>

extern std::string sha1(const std::string &input);
extern std::string base64_encode(const std::string &input);

class WebSocketServer {
public:
  const std::string magic = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";

  WebSocketServer(Server &server);

  void onConnect(std::function<void(WebSocket &)>);
  void onDisconnect(std::function<void(WebSocket &)>);

private:
  Server &m_server;
  std::vector<WebSocket> m_clients;
  std::function<void(WebSocket &)> m_clientConnect;
  std::function<void(WebSocket &)> m_clientDisconnect;

  void clientConnected(Request &, Response &);
};
