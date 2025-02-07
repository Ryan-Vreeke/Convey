#include "response.h"

#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "stringUtils.hpp"

using namespace Convey;
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

bool fileExists(const std::string &path) { return std::filesystem::exists(path); }

std::string Response::findFileWithExtension(std::string basePath) {
  if (Utils::ends_with(basePath, "/")) basePath.pop_back();

  if (fileExists(basePath)) return basePath;

  if (fileExists(basePath + ".html")) {
    return basePath + ".html";
  }

  return "";
}

std::string getFileExtension(const std::string &filePath) {
  size_t dotPosition = filePath.find_last_of('.');
  if (dotPosition == std::string::npos || dotPosition == filePath.length() - 1) {
    return "";
  }
  return filePath.substr(dotPosition + 1);
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

  std::string filetype = getFileExtension(actualPath);
  if (typeSet.count(filetype)) {
    contentType = typeSet[filetype];
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
