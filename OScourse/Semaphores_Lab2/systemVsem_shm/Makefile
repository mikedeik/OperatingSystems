all: shareMem1 \
	shareMem2 \
	systemV-semaphores \
	sem-server \
	sem-client

shareMem1: shareMem1.c
	gcc -g -Wall shareMem1.c -o shareMem1

shareMem2: shareMem2.c
	gcc -g -Wall shareMem2.c -o shareMem2

systemV-semaphores: systemV-semaphores.c
	gcc -g -Wall systemV-semaphores.c -o systemV-semaphores

sem-server: sem-server.c
	gcc -g -Wall sem-server.c -o sem-server

sem-client: sem-client.c
	gcc -g -Wall sem-client.c -o sem-client


clean:
	rm -f shareMem1 shareMem2 systemV-semaphores \
		sem-client sem-server

