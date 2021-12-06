all: 	semtest  \
	semtest3 \
	semtest3a \
	namedSem

semtest: semtest.c
	gcc -g -Wall semtest.c -o semtest -lpthread

semtest3: semtest3.c
	gcc -g -Wall semtest3.c -o semtest3 -lpthread

semtest3a: semtest3a.c
	gcc -g -Wall semtest3a.c -o semtest3a -lpthread

namedSem: namedSem.c
	gcc -g -Wall namedSem.c -o  namedSem -lpthread

clean:
	rm -f semtest namedSem  \
		semtest3 semtest3a
