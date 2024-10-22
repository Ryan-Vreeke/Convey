CC = g++
CFLAGS = -Wall -g -Iinclude
SRC = src/main.cpp src/server.cpp src/request.cpp src/response.cpp src/WebSocketServer.cpp src/WebSocket.cpp src/WSFrame.cpp src/WSMessage.cpp
CCLIB = -lssl -lcrypto

Server: clean
	$(CC) $(CFLAGS) $(SRC) $(CCLIB) -o build/server

run: clean Server
	./build/server

clean:
	rm -f server
