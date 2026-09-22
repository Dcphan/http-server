CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -I./src

TARGET = server

SRC = \
	main.cpp \
	src/HttpServer.cpp \
	src/networking/socket.cpp \
	src/application/http_request.cpp \
	src/application/http_response.cpp \
	src/application/router.cpp

OBJ = $(SRC:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJ)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run
