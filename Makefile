CC = gcc

server: server.c client.c
	$(CC) server.c -o server
	$(CC) client.c -o client

