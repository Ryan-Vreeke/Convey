#include "request.h"
#include <iostream>
#include <sstream>
#include <stdexcept>

// Constructor implementation
Request::Request(const std::string &rawRequest)
    : method(""), httpVersion(""), path(""), clientSocket(-1) {
  std::istringstream requestStream(rawRequest);
  std::string requestLine;

  // Parse the request line (first line)
  if (std::getline(requestStream, requestLine)) {
    std::vector<std::string> requestLineParts = splitString(requestLine, " ");
    if (requestLineParts.size() < 3) {
      throw std::invalid_argument("Invalid HTTP request line.");
    }

    method = requestLineParts[0];
    path = requestLineParts[1];
    httpVersion = requestLineParts[2];
  }

  std::string header;
  while (std::getline(requestStream, header) && header != "\r") {
    std::string key = splitString(header, ": ")[0];
    std::string value = splitString(header, ": ")[1];
    headers[key] = value;
  }

  // If a Content-Length header exists, parse the body
  auto contentLengthIt = headers.find("Content-Length");
  if (contentLengthIt != headers.end()) {
    size_t contentLength = std::stoi(contentLengthIt->second);
    body.resize(contentLength);
    requestStream.read(&body[0], contentLength);
  }
}

std::vector<std::string> Request::splitString(const std::string &str,
                                              const std::string &delimiter) {
  std::vector<std::string> tokens;
  size_t start = 0;
  size_t end = str.find(delimiter);

  while (end != std::string::npos) {
    tokens.push_back(str.substr(start, end - start));
    start = end + delimiter.length();
    end = str.find(delimiter, start);
  }

  tokens.push_back(str.substr(start));

  return tokens;
}
