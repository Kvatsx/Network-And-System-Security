all: IRC

IRC: server.o client.o helper.o
	g++ -pthread -o server server.o helper.o -g -lssl -lcrypto -Wall
	g++ -pthread -o client client.o helper.o -g -lssl -lcrypto -Wall 

server:
	./server

client:
	./client 127.0.0.1

Perm:
	sudo chown fakeroot:fakeroot server
	sudo chown fakeroot:fakeroot client
	sudo chmod u+s server
	sudo chmod u+s client

clean:
	rm -f server.o helper.o server *~
	rm -f client.o helper.o client *~