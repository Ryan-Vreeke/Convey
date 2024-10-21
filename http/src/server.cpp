#include "server.h"
#include <cassert>
#include <functional>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>

Server::Server(const std::string &publicPath)
    : server_fd(socket(AF_INET, SOCK_STREAM, 0)), publicDir(publicPath) {
  if (server_fd == INVALID_SOCKET) {
    std::cerr << "Socket creation failed. \n";
    cleanup(server_fd);
    return;
  }
}

Server::Server() : server_fd(socket(AF_INET, SOCK_STREAM, 0)), publicDir("./") {
  if (server_fd == INVALID_SOCKET) {
    std::cerr << "Socket creation failed. \n";
    cleanup(server_fd);
    return;
  }
}

Server::~Server() {
  running = false;
  cleanup(server_fd);
}

void Server::get(std::string path,
                 std::function<void(Request &, Response &)> func) {

  if (!Response::ends_with(path, "/"))
    path += "/";
  getMap[path] = func;
}

void Server::post(std::string path,
                  std::function<void(Request &, Response &)> func) {
  postMap[path] = func;
}

void Server::acceptClient() {
  char buffer[BUF_SIZE];
  while (running) {
    sockaddr_in client_address;
    socklen_t client_len = sizeof(client_address);
    SOCKET client_fd =
        accept(server_fd, (sockaddr *)&client_address, &client_len);

    if (client_fd == INVALID_SOCKET) {
      std::cerr << "Accept failed.\n";
    }

    read(client_fd, buffer, sizeof(buffer));

    Request request(buffer);
    Response response(client_fd, client_address);

    std::thread clientThread(
        [this, request, response]() { this->handleClient(request, response); });
    clientThread.detach();
  }
}

bool Server::contains(
    const std::unordered_map<std::string,
                             std::function<void(Request &, Response &)>> &map,
    const std::string &key) {
  return map.find(key) != map.end();
}

void Server::startListen(int port, std::function<void(void)> listenLoop) {
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(port);

  if (bind(server_fd, (sockaddr *)&address, sizeof(address)) == SOCKET_ERROR) {
    std::cerr << "Bind failed.\n";
    cleanup(server_fd);
    return;
  }

  if (listen(server_fd, 3) == SOCKET_ERROR) {
    std::cerr << "Listen failed.\n";
    cleanup(server_fd);
    return;
  }

  running = true;

  listenLoop();
  acceptClient();
}

void Server::cleanup(SOCKET sock) {
  if (sock == INVALID_SOCKET)
    return;
  close(sock);
}

void Server::handleClient(Request request, Response response) {
//TODO: make sure that the upgrade is a websocket
  if (request.headers["Upgrade"] != "") {
    std::cout << request.headers["Upgrade"] << std::endl;
    WSConnected(request, response);
    return;
  }

  if (!Response::ends_with(request.path, "/"))
    request.path += "/";

  if (request.method == "GET" && contains(getMap, request.path)) {
    getMap[request.path](request, response);
  } else if (request.method == "POST" && contains(postMap, request.path)) {
    postMap[request.path](request, response);
  } else {
    std::string filePath = publicDir + request.path;
    if (request.path == "/")
      filePath = publicDir + "/index.html";
    response.sendFile(filePath);
  }

  close(response.clientSocket);
}

void Server::addWSObserver(
    std::function<void(Request &, Response &)> callback) {
  WSConnected = callback;
}
