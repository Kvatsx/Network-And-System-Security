all: IRC

IRC: server.o client.o helper.o
	g++ -pthread -o server server.o helper.o -g -lcrypto -Wall
	g++ -pthread -o client client.o helper.o -g -lcrypto -Wall 

server:
	./server

client:
	./client 127.0.0.1

clean:
	rm -f server.o helper.o server *~
	rm -f client.o helper.o client *~