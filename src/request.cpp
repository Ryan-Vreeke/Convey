#include "request.h"

#include <sstream>
#include <stdexcept>

#include "stringUtils.hpp"

Request::Request(const std::string& rawRequest)
    : method(""), httpVersion(""), path(""), clientSocket(-1) {
  std::istringstream requestStream(rawRequest);
  std::string requestLine;

  // Parse the request line (first line)
  if (std::getline(requestStream, requestLine)) {
    auto requestLineParts = Utils::splitString(requestLine, " ");
    if (requestLineParts.size() < 3) {
      throw std::invalid_argument("Invalid HTTP request line.");
    }

    method = requestLineParts[0];

    if (Utils::contains(requestLineParts[1], "?")) {
      auto argsLine = Utils::splitString(requestLineParts[1], "?");
      path = argsLine[0];

      for (const auto& arg : Utils::splitString(argsLine[1], "&")) {
        auto pair = Utils::splitString(arg, "=");
        std::string key = pair[0];
        std::string value = pair[1];

        args[key] = value;
      }
    } else {
      path = requestLineParts[1];
    }

    httpVersion = requestLineParts[2];
  }

  std::string header;
  while (std::getline(requestStream, header) && header != "\r") {
    std::string key = Utils::splitString(header, ": ")[0];
    std::string value = Utils::splitString(header, ": ")[1];
    headers[key] = value;
  }

  // If a Content-Length header exists, parse the body
  auto contentLengthIt = headers.find("Content-Length");
  if (contentLengthIt != headers.end()) {
    size_t contentLength = std::stoi(contentLengthIt->second);
    body.resize(contentLength);
    requestStream.read(&body[0], contentLength);
  }

  if (!Utils::ends_with(path, "/")) path += "/";
}
