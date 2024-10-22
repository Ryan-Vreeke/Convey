#include "WebSocketServer.h"
#include <memory>
#include <openssl/bio.h>
#include <openssl/buffer.h>
#include <openssl/evp.h>
#include <openssl/sha.h>

WebSocketServer::WebSocketServer(Server &server) : m_server(server) {
  server.addWSObserver(std::bind(&WebSocketServer::clientConnected, this,
                                 std::placeholders::_1, std::placeholders::_2));
}

void WebSocketServer::clientConnected(Request &req, Response &res) {
  std::string key = req.headers["Sec-WebSocket-Key"];
  key.erase(key.end() - 1);
  std::string shaResult = sha1(key + magic);
  std::string acceptKey = base64_encode(shaResult);

  res.code = "101 Switching Protocols";
  res.headers.push_back("Sec-WebSocket-Accept: " + acceptKey);
  res.headers.push_back("Upgrade: websocket");
  res.headers.push_back("Connection: Upgrade");

  res.send("");

  auto ws = std::make_shared<WebSocket>(res.clientSocket, res.clientAddress);
  ws->m_serverDisconnect = [this](int i) { onDisconnect(i); };
  m_clients[ws->m_socket] = ws;

  m_clientConnect(ws);

  std::thread wsLoop([ws]() { ws->loop(); });
  wsLoop.detach();
}

void WebSocketServer::onConnect( std::function<void(std::shared_ptr<WebSocket>)> callback) {
  m_clientConnect = callback;
}

void WebSocketServer::onDisconnect(int wSocket) {
  m_clients.erase(wSocket);
  close(wSocket);
}

std::string sha1(const std::string &input) {
  unsigned char hash[SHA_DIGEST_LENGTH];
  SHA1(reinterpret_cast<const unsigned char *>(input.c_str()), input.length(),
       hash);
  return std::string(reinterpret_cast<char *>(hash), SHA_DIGEST_LENGTH);
}

std::string base64_encode(const std::string &input) {
  BIO *bio, *b64;
  BUF_MEM *bufferPtr;

  b64 = BIO_new(BIO_f_base64());
  bio = BIO_new(BIO_s_mem());
  bio = BIO_push(b64, bio);

  BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL);
  BIO_write(bio, input.data(), input.size());
  BIO_flush(bio);
  BIO_get_mem_ptr(bio, &bufferPtr);

  std::string encoded_data(bufferPtr->data, bufferPtr->length);
  BIO_free_all(bio);

  return encoded_data;
}
