all: IRC

IRC: server.o client.o helper.o
	g++ -o server server.o helper.o -g 
	g++ -o client client.o helper.o -g 

server:
	./server

client:
	./client 127.0.0.1

clean:
	rm -f server.o helper.o server *~
	rm -f client.o helper.o client *~