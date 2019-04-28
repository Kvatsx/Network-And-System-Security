all: RAW_FILES

RAW_FILES: client.o utils.o
	# gcc -pthread -o server server.o helper.o -g 
	gcc -pthread -o client client.o utils.o -g

clean: 
	# rm -f server.o helper.o server *~
	rm -f client.o utils.o client *~
