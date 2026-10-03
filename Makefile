CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -pthread

.PHONY: all client server clean

all: client server

client:
	$(CXX) $(CXXFLAGS) \
	client/main.cpp \
	client/SystemMonitor.cpp \
	client/NetworkClient.cpp \
	client/ClientIdentity.cpp \
	-o build/client

server:
	$(CXX) $(CXXFLAGS) \
	server/main.cpp \
	server/Server.cpp \
	server/ClientRegistry.cpp \
	server/AlertManager.cpp \
	server/Dashboard.cpp \
	-o build/server

clean:
	rm -f build/client build/server
