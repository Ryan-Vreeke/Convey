#include "WebSocket.h"
#include "WSFrame.h"
#include <cstdint>
#include <iostream>
#include <netinet/in.h>
#include <unistd.h>
#include <vector>

WebSocket::WebSocket(int socket, sockaddr_in address)
    : m_socket(socket), m_address(address) {}

void WebSocket::onMessage(std::function<void(uint8_t *, size_t)> callback) {
  m_messageCallback = callback;
}

void WebSocket::onClose(std::function<void(std::string)> callback) {
  m_closeCallback = callback;
}

void WebSocket::send() {}

void WebSocket::loop() {
  uint8_t buf[4096];
  size_t bytes;

  while (!close) {
    if ((bytes = read(m_socket, buf, sizeof(buf))) == 0) {
      close = true;
      m_serverDisconnect(m_socket);
      m_closeCallback("0 Bytes RECV");
      break;
    }

    if (!m_messageCallback) {
      continue;
    }

    WSFrame frame(buf, bytes, m_socket);

    if (frame.m_opcode == CLOSE) {
      close = true;
      m_serverDisconnect(m_socket);
      m_closeCallback(std::string(reinterpret_cast<char *>(frame.m_payload), frame.m_payloadLen));
      break;
    }

    std::cout << frame.m_payloadLen;
    m_messageCallback(frame.m_payload, frame.m_payloadLen);
  }
}
