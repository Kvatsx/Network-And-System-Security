all: RAW_FILES

RAW_FILES: server.o client.o helper.o
	gcc -lpthread -o server server.o helper.o -g 
	gcc -lpthread -o client client.o helper.o -g

clean: 
	rm -f server.o helper.o server *~
	rm -f client.o helper.o client *~
