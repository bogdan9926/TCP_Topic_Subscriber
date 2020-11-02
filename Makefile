CC=g++ -std=c++11 -Wall -Wextra

all: build

build: server subscriber

server_functions.o: server_functions.cpp
	$(CC) $^ -c

server.o: server.cpp
	$(CC) $^ -c

server: server_functions.o server.o
	$(CC) $^ -o server


subscriber: subscriber.o
	$(CC) $^ -o subscriber


subscriber.o: subscriber.cpp
	$(CC) $^ -c

clean:
	rm *.o server subscriber
