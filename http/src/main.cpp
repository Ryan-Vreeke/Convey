#include "WebSocket.h"
#include "WebSocketServer.h"
#include "server.h"
#include <cstdio>
#include <iostream>
#include <memory>

#define PORT 8000
int main(void) {
  Server server("./public");
  WebSocketServer wss(server);

  wss.onConnect([](std::shared_ptr<WebSocket> ws) -> void {
    std::cout << "client: " << ws->m_socket << " connected\n";

    ws->m_messageCallback = [ws](WSMessage &msg) -> void {
      std::cout << msg.m_size << ": " << ws->m_socket << std::endl;
      std::cout << msg << std::endl;

      ws->send("hello");
    };

    ws->onClose([](std::string reason) {
      std::cout << "client disconnected" << std::endl;
    });
  });

  server.startListen(PORT, []() -> void {
    std::cout << "Server is running on http://localhost:" << PORT << std::endl;
  });

  return 0;
}
