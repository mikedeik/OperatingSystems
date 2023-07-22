#include <iostream>
#include <stdlib.h>
#include <stdlib.h>
#include <queue>
#include "Proccess_Scheduler.hpp"
#include <unistd.h>
// #include "PCB.h"

#include "utils.hpp"

using namespace std;
using std::vector;

int main(int argc, char *argv[])
{

    if (argc != 8)
    {
        cerr << "Arguments should be:\n"
             << "1. median arrival time\n"
             << "2. median burst time\n"
             << "3. median critical section time\n"
             << "4. Max No of processes\n"
             << "5. probability of requesting critical section\n"
             << "6. Number of available Semaphores\n"
             << "7. Timeslot\n";
        return EXIT_FAILURE;
    }

    srand(time(0));
    int random;

    int priority;
    double burst_time;
    double arrival_time;
    double critical_time;

    double arrivals_l = atof(argv[1]);
    double bursts_l = atof(argv[2]);
    double critical_l = atof(argv[3]);
    const float request_critical = atof(argv[5]);
    int S = atoi(argv[6]);
    float timeslot = atof(argv[7]);

    if (request_critical > 1 || request_critical < 0)
    {
        return EXIT_FAILURE;
    }

    const int Max_Processes = atoi(argv[4]);

    vector<PCB> processes;
    double next_arrival = 0.0;

    for (int pid = 0; pid < Max_Processes; pid++)
    {
        priority = (rand() % 7) + 1;
        random = rand() % 10 + 2;

        burst_time = RandExp(random, bursts_l);
        arrival_time = RandExp(random, arrivals_l);
        critical_time = RandExp(random, critical_l);

        next_arrival += arrival_time;
        processes.push_back(PCB(pid, priority, burst_time, next_arrival, critical_time));
    }

    Process_Sheduler PS = Process_Sheduler(processes, request_critical, S, Max_Processes, timeslot);

    PS.Run_Scheduler();
    PS.Print_stats();

    return 0;
}