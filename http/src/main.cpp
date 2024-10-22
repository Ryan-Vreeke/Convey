#include "WebSocket.h"
#include "WebSocketServer.h"
#include "server.h"
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <iostream>
#include <memory>

#define PORT 8000
int main(void) {
  Server server("./public");
  WebSocketServer wss(server);

  wss.onConnect([](std::shared_ptr<WebSocket> ws) -> void {
    std::cout << "client: " << ws->m_socket << " connected\n";

    ws->m_messageCallback = [&ws](uint8_t *message, size_t len) -> void {
      std::printf("%s\n", message);
      std::cout << len << ": " << ws->m_socket << std::endl;
    };

    ws->onClose([]() { std::cout << "client disconnected" << std::endl; });
  });

  server.startListen(PORT, []() -> void {
    std::cout << "Server is running on http://localhost:" << PORT << std::endl;
  });

  return 0;
}
