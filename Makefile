all:
	g++ PCB.cpp Semaphore.cpp Proccess_Scheduler.cpp utils.cpp main.cpp -o PS.o

clear:
	rm *.o