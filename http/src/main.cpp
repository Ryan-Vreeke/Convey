#include "WebSocket.h"
#include "WebSocketServer.h"
#include "server.h"
#include <cstddef>
#include <cstdint>
#include <iostream>

#define PORT 8000
int main(void) {
  Server server("./public");
  WebSocketServer wss(server);

  wss.onConnect([](WebSocket &ws) -> void {
    std::cout << "client" << ws.m_address.sin_addr.s_addr << " connected\n";

    ws.onMessage([](uint8_t *message, size_t len) -> void {
      std::cout << "hello" << std::endl;
      std::cout << len << std::endl;

    });

    ws.onClose([]() { std::cout << "client disconnected" << std::endl; });
  });

  server.startListen(PORT, []() -> void {
    std::cout << "Server is running on http://localhost:" << PORT << std::endl;
  });

  return 0;
}
