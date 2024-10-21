#include "response.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

Response::Response(int clientSocket, sockaddr_in clientAddress)
    : clientSocket(clientSocket), clientAddress(clientAddress) {}

void Response::send(const std::string &body) {
  message = body;
  contentLength = body.size();
  sendResponse();
}

void Response::json(const std::string &jsonData) {
  contentType = "application/json";
  message = jsonData;
  contentLength = jsonData.size();

  sendResponse();
}

void Response::type(const std::string &type) { contentType = type; }

void Response::sendFile(const std::string &path) {
  std::ifstream file(path);
  if (!file.is_open()) {
    code = "404 Not Found";
    send("File not found");
    return;
  }

  std::stringstream buffer;
  buffer << file.rdbuf();
  message = buffer.str();
  contentLength = message.size();

  if (ends_with(path, ".html")) {
    contentType = "text/html";
  } else if (ends_with(path, ".jpg") || ends_with(path, ".jpeg")) {
    contentType = "image/jpeg";
  } else if (ends_with(path, ".png")) {
    contentType = "image/png";
  } else {
    contentType = "application/octet-stream";
  }

  headers.push_back("Connection: close\r\n");
  sendResponse();

  file.close();
}

void Response::sendResponse() {
  std::string headers = generateHeaders();
  std::string response = headers + message;

  ::send(clientSocket, response.c_str(), response.size(), 0);
}

std::string Response::generateHeaders() {
  std::ostringstream headerString;
  std::ostringstream header;
  headerString << version << " " << code << "\r\n";

  headers.push_back("Content-Type: " + contentType);
  headers.push_back("Content-Length: " + std::to_string(contentLength));

  for (const auto &h : headers) {
    headerString << h << "\r\n";
  }

  headerString << "\r\n";
  return headerString.str();
}

bool Response::ends_with(const std::string &str, const std::string &suffix) {
  return str.size() >= suffix.size() &&
         str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}
