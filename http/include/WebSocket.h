#pragma once

#include <cstddef>
#include <cstdint>
#include <functional>
#include <netinet/in.h>
#include <thread>

class WebSocket {
public:
  const int m_socket;
  const sockaddr_in m_address;
  std::function<void(uint8_t *, size_t)> m_messageCallback;
  std::function<void(void)> m_closeCallback;
  bool message = false;
  bool close = false;

  WebSocket(int socket, sockaddr_in address);

  void onMessage(std::function<void(uint8_t *, size_t)>);
  void onClose(std::function<void(void)>);
  void send();

private:
  std::thread loopThread;

  void loop();
};
