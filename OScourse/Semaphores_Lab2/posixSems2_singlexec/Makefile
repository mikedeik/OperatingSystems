
ALL: semex

semex: semex.o semexworker.o
	gcc semex.o semexworker.o -o semex -lpthread

semex.o: semex.c
	gcc -g -Wall -c semex.c

semexworker.o: semexworker.c
	gcc -g -Wall -c semexworker.c

clean:
	rm -f semex semex.o semexworker.o
