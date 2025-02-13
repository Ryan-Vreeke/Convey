#pragma once

#include "WSMessage.h"
#include <cstddef>
#include <cstdint>
#include <functional>
#include <netinet/in.h>
#include <thread>
namespace Convey {

class WebSocket {
public:
  const int m_socket;
  const sockaddr_in m_address;
  std::function<void(WSMessage &)> m_messageCallback;
  std::function<void(int)> m_serverDisconnect;
  std::function<void(std::string)> m_closeCallback;
  bool close = false;

  WebSocket(int socket, sockaddr_in address);
  void loop();

  void onMessage(std::function<void(WSMessage &)>);
  void onClose(std::function<void(std::string)>);
  void send(const std::string &);
  void send(const uint8_t *payload, size_t len);
  void send(const std::vector<uint8_t> &payload);
  void send(const std::vector<char> &payload);

private:
  std::thread loopThread;
};
} // namespace Convey
