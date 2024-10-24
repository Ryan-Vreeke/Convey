#include "WebSocket.h"
#include "WebSocketServer.h"
#include "server.h"
#include <cstdint>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <memory>

#define PORT 8000
int main(void) {
  Server server("/home/ryan/Work/Convey/public/");
  WebSocketServer wss(server);

  std::ifstream file("/home/ryan/Pictures/DSC03528.png", std::ios::binary | std::ios::ate);
  std::streamsize fileSize = file.tellg();
  file.seekg(0, std::ios::beg);

  std::vector<uint8_t> buffer(fileSize);
  if (file.read(reinterpret_cast<char *>(buffer.data()), fileSize)) {
    std::cout << "file read successfull\n";
  }

  wss.onConnect([buffer](std::shared_ptr<WebSocket> ws) -> void {
    std::cout << "client: " << ws->m_socket << " connected\n";

    ws->m_messageCallback = [ws, buffer](WSMessage &msg) -> void {
      std::cout << "message len: " << msg.m_size << ",Msg: " << msg << std::endl;

      std::cout << "sending image!\n";
      std::cout << buffer.size() << std::endl;
      ws->send(buffer);
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
