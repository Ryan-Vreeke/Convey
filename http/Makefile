CC = g++
CFLAGS = -Wall -Iinclude -Isrc
SRC = src/main.cpp src/server.cpp src/request.cpp src/response.cpp src/WebSocketServer.cpp src/WebSocket.cpp src/WSFrame.cpp src/WSMessage.cpp
OBJ = build/server.o build/request.o build/response.o build/WebSocketServer.o build/WebSocket.o build/WSFrame.o build/WSMessage.o
CCLIB = -lssl -lcrypto

build/%.o: src/%.cpp
	$(CC) $(CFLAGS) -c $< -o $@

libconvey.a: $(OBJ)
	ar rcs lib/libconvey.a $(filter-out build/main.o, $(OBJ)) # Exclude main.o from the library

Server: libconvey.a
	$(CC) $(CFLAGS) src/main.cpp lib/libconvey.a $(CCLIB) -o build/server

run: clean Server
	./build/server

clean:
	rm -f build/server lib/libconvey.a $(OBJ)


# Server: clean
# 	$(CC) $(CFLAGS) $(SRC) $(CCLIB) -o build/server

