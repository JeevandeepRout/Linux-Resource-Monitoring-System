CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -pthread

.PHONY: all client server test clean

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

test:
	$(CXX) $(CXXFLAGS) \
	tests/test_metrics.cpp \
	client/SystemMonitor.cpp \
	-o build/test_metrics

	$(CXX) $(CXXFLAGS) \
	tests/test_protocol.cpp \
	-o build/test_protocol

	@echo "Running metric test..."
	./build/test_metrics

	@echo "Running protocol test..."
	./build/test_protocol

clean:
	rm -f build/client build/server
	rm -f build/test_metrics build/test_protocol
