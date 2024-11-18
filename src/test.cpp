#include "json.hpp"
#include "request.h"
#include "server.h"
#include <iostream>

using json = nlohmann::json;
int main(void) {
  Server wServer("./");

  wServer.get("/get", [](Request &req, Response &res) {
    json jsonResponse;
    jsonResponse["Stream"] = "192.168.1.1";
    jsonResponse["Port"] = "8080";
    res.json(jsonResponse.dump(4));
  });

  wServer.post("/post", [](Request &req, Response &res) {
    json jsonBody(req.body);
    std::cout << jsonBody << std::endl;
    res.json(jsonBody.dump(4));
  });

  wServer.startListen(8080, []() -> void {
    std::cout << "Server is running on http://localhost:8080" << std::endl;
  });
}
