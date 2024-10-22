#include "WebSocket.h"
#include "WebSocketServer.h"
#include "server.h"
#include <cstdio>
#include <fstream>
#include <iostream>
#include <memory>

#define PORT 8000
int main(void) {
  Server server("./public");
  WebSocketServer wss(server);

  wss.onConnect([](std::shared_ptr<WebSocket> ws) -> void {
    std::cout << "client: " << ws->m_socket << " connected\n";

    ws->m_messageCallback = [&ws](WSMessage &msg) -> void {
      std::cout << msg.m_size << ": " << ws->m_socket << std::endl;

      std::ofstream file("./test.png", std::ios::binary);
      file.write(reinterpret_cast<const char *>(msg.m_msg), msg.m_size);
      file.close();

      if (file.good()) {
        std::cout << "PNG file written successfully!" << std::endl;
      } else {
        std::cerr << "Error: Failed to write PNG file." << std::endl;
      }
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
