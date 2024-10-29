#include "response.h"
#include <filesystem>
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

bool fileExists(const std::string &path) {
  return std::filesystem::exists(path);
}

std::string Response::findFileWithExtension(std::string basePath) {
  if (Response::ends_with(basePath, "/"))
    basePath.pop_back();

  if (fileExists(basePath))
    return basePath;

  if (fileExists(basePath + ".html")) {
    return basePath + ".html";
  }

  return "";
}

void Response::sendFile(const std::string &path) {
  const std::string actualPath = findFileWithExtension(path);
  std::ifstream file(actualPath);

  if (!file.is_open()) {
    code = "404 Not Found";
    send("File not found");
    return;
  }

  std::stringstream buffer;
  buffer << file.rdbuf();
  message = buffer.str();
  contentLength = message.size();

  if (ends_with(actualPath, ".html")) {
    contentType = "text/html";
  } else if (ends_with(actualPath, ".jpg") || ends_with(actualPath, ".jpeg")) {
    contentType = "image/jpeg";
  } else if (ends_with(actualPath, ".png")) {
    contentType = "image/png";
  } else if (ends_with(actualPath, ".css")) {
    contentType = "text/css";
  } else if (ends_with(actualPath, ".svg")) {
    contentType = "image/svg+xml";
  } else {
    contentType = "application/octet-stream";
  }

  headers.push_back("Connection: close");
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
