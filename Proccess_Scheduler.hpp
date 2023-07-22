#ifndef PROCESS_SCHEDULER_HPP
#define PROCESS_SCHEDULER_HPP

#include <iostream>
#include <stdlib.h>
#include <stdlib.h>
#include <queue>
#include <map>
#include <vector>
#include "Semaphore.hpp"
#include <random>
#include <ctime>

#define TIMESLOT 3

using std::priority_queue;
using std::vector;

// Reversing the functionality of PQ (we need lowest priority to be highest)
typedef struct compare
{
    bool operator()(PCB *p1, PCB *p2)
    {
        return p1->get_priority() > p2->get_priority();
    }
} LessThan;

// keeps the stats of a priority group
typedef struct Statistics
{
    int counter;
    int overall_waiting_timeslots;
    int sem_waiting_timeslots;
} stats;

class Process_Sheduler
{
private:
    PCB *Current; // current Process PID that is running
    vector<PCB> processes;
    priority_queue<PCB *, vector<PCB *>, LessThan> ready_queue; // processes that are ready sorted by priority
    int number_of_processes;
    Semaphore **semaphores;
    double current_time;
    const float request_critical;
    const double timeslot;
    const int S;
    stats *statistics;

public:
    Process_Sheduler(vector<PCB> inc_processes, const float inc_request_critical, const int sem_count, int Max_processes, /*int S,*/ float inc_timeslot);
    ~Process_Sheduler();
    void check_incoming_PCB();
    const void print_PQ();
    const void Run_Scheduler();
    const void Run_Process(PCB *process);
    const void Run_InCritical(PCB *process);
    const void Print_stats();
    const void HandleWaiting(int sem_id, PCB *process);
};

#endif // PROCESS_SCHEDULER_HPP