#include "WebSocket.h"
#include <cstdint>
#include <iostream>
#include <netinet/in.h>
#include <unistd.h>

WebSocket::WebSocket(int socket, sockaddr_in address)
    : m_socket(socket), m_address(address) {

  loopThread = std::thread([this]() { this->loop(); });
  loopThread.detach();
}

void WebSocket::onMessage(std::function<void(uint8_t *, size_t)> callback) {
  std::cout << "created onMessage\n";
  m_messageCallback = callback;
  message = true;
}

void WebSocket::onClose(std::function<void(void)> callback) {
  std::cout << "created onClose\n";
  m_closeCallback = callback;
  close = true;
}

void WebSocket::send() {}

void WebSocket::loop() {
  uint8_t buf[2046];
  size_t bytes;

  while (!close) {
    if ((bytes = read(m_socket, buf, sizeof(buf))) == 0)
      continue;

    if (!message)
      continue;

    // TODO: get payload
    m_messageCallback(buf, bytes);
  }
}
