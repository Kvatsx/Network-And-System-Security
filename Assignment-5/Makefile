all: RAW_FILES

RAW_FILES: server.o client.o helper.o
	gcc -pthread -o server server.o helper.o -g 
	gcc -pthread -o client client.o helper.o -g

clean: 
	rm -f server.o helper.o server *~
	rm -f client.o helper.o client *~
