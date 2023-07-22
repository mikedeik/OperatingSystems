
#include <iostream>
#include "Semaphore.hpp"




Semaphore::Semaphore(): 
sem_switch(1)
{}

Semaphore::~Semaphore(){}

// returns if we managed to down the Semaphore
int Semaphore::try_down()
{

    if (sem_switch)
    {
        sem_switch--;        
        return 1;
    }

    return 0;
    
}
// returns if we managed to up the Semaphore
int Semaphore::try_up()
{
    if (!sem_switch)
    {
        
        sem_switch++;
        return 1;
    }

    return 0;   
}

// returns first process waiting
PCB* Semaphore::get_front_process()
{
    return pids.front();
}

// adds process to Semaphore queue
void Semaphore::push(PCB* process)
{
    pids.push(process);
}

// pops process from queue
void Semaphore::pop()
{
    pids.pop();
}

// force release semaphore in case of Deadlock
void Semaphore::force_up()
{
    sem_switch++;
}

