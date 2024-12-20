#include <iostream>
#include <string>

#include "json.hpp"
#include "server.h"

#define PORT 8080

using json = nlohmann::json;
int main(void) {
  Server wServer("public/");
  
  

  wServer.startListen(PORT, []() -> void {
    std::cout << "Server is running on http://localhost:" << PORT << std::endl;
  });
}
