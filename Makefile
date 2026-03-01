CXX = g++
CFLAGS = -Wall -Werror -std=c++17

TARGET = loadbalancer
SOURCES = main.cpp Request.cpp RequestQueue.cpp WebServer.cpp LoadBalancer.cpp
OBJECTS = $(SOURCES:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(CFLAGS) -o $(TARGET) $(OBJECTS)

main.o: main.cpp LoadBalancer.h
	$(CXX) $(CFLAGS) -c main.cpp

Request.o: Request.cpp Request.h
	$(CXX) $(CFLAGS) -c Request.cpp

RequestQueue.o: RequestQueue.cpp RequestQueue.h Request.h
	$(CXX) $(CFLAGS) -c RequestQueue.cpp

WebServer.o: WebServer.cpp WebServer.h Request.h
	$(CXX) $(CFLAGS) -c WebServer.cpp

LoadBalancer.o: LoadBalancer.cpp LoadBalancer.h WebServer.h RequestQueue.h Request.h
	$(CXX) $(CFLAGS) -c LoadBalancer.cpp

clean:
	del /f /q $(TARGET).exe $(OBJECTS) 2>nul || true