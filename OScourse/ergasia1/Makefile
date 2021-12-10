all: server client

server: server.c
	gcc -g -Wall -o server server.c -lpthread

client: client.c
	gcc -g -Wall -o client client.c -lpthread

clear:
	-rm *.o