#include <iostream>
#include "Websocket.h"

int main()
{
  std::cout << "testing" << std::endl;

  Websocket w1{};
  w1.start(8000);
  w1.handleClient();
  w1.stop();

  return 0;
}
