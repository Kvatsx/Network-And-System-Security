all:
	gcc -pthread shell_server.c -o shell_server
	gcc -pthread client.c -o client
	# gcc -pthread MyServer.c -o MyServer
	gcc ls.c -o ls

s: 
	./shell_server

c:
	./client 127.0.0.1

clean:
	# rm MyServer
	rm shell_server
	rm client
	rm ls
