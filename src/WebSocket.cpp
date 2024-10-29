#include "WebSocket.h"
#include "WSFrame.h"
#include "WSMessage.h"
#include <cstdint>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>

WebSocket::WebSocket(int socket, sockaddr_in address)
    : m_socket(socket), m_address(address) {}

void WebSocket::onMessage(std::function<void(WSMessage &)> callback) {
  m_messageCallback = callback;
}

void WebSocket::onClose(std::function<void(std::string)> callback) {
  m_closeCallback = callback;
}

void WebSocket::send(const std::string &payload) {
  WSFrame frame{};
  frame.m_opcode = TEXT;
  frame.encodePayload(reinterpret_cast<const uint8_t *>(payload.c_str()), payload.length());
  ::send(m_socket, frame.m_buffer, frame.m_len, 0);
}

void WebSocket::send(const uint8_t *payload, size_t len) {
  WSFrame frame{};
  frame.m_opcode = BINARY;
  frame.encodePayload(payload, len);
  ::send(m_socket, frame.m_buffer, frame.m_len, 0);
}

void WebSocket::send(const std::vector<uint8_t> &payload) {
  WSFrame frame{};
  frame.m_opcode = BINARY;
  frame.encodePayload(payload.data(), payload.size());
  ::send(m_socket, frame.m_buffer, frame.m_len, 0);
}

void WebSocket::send(const std::vector<char> &payload) {
  WSFrame frame{};
  frame.m_opcode = BINARY;
  frame.encodePayload(reinterpret_cast<const uint8_t *>(payload.data()), payload.size());
  ::send(m_socket, frame.m_buffer, frame.m_len, 0);
}

void WebSocket::loop() {
  uint8_t buf[4096];
  size_t bytes;
  std::vector<uint8_t> msgBuffer;
  WSMessage msg;

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
    std::cout << "socket: " << m_socket << std::endl;

    if (frame.m_opcode == CLOSE) {
      close = true;
      m_serverDisconnect(m_socket);
      m_closeCallback(std::string(reinterpret_cast<char *>(frame.m_payload),
                                  frame.m_payloadLen));
      break;
    }

    if (frame.m_opcode == CONTINUE) {
      msgBuffer.insert(msgBuffer.begin(), frame.m_payload,
                       frame.m_payload + frame.m_payloadLen);
      continue;
    }

    if (msgBuffer.size() > 0) {
      msg.m_msg = msgBuffer.data();
      msg.m_size = msgBuffer.size();
      msgBuffer.clear();
    } else {
      msg.m_msg = frame.m_payload;
      msg.m_size = frame.m_payloadLen;
    }

    msg.m_type = frame.m_opcode;
    m_messageCallback(msg);
  }
}
