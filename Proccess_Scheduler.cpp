#include <iostream>
#include <stdlib.h>
#include <queue>
#include <vector>
#include "Proccess_Scheduler.hpp"
#include "utils.hpp"
#include <random>
#include <ctime>

using std::cout;
using std::priority_queue;
using std::queue;

/*==================== Constructor =============================*/

Process_Sheduler::Process_Sheduler(vector<PCB> inc_processes, const float inc_request_critical, const int sem_count, int Max_processes, float inc_timeslot) : Current(NULL),
                                                                                                                                                              timeslot(inc_timeslot),
                                                                                                                                                              current_time(0),
                                                                                                                                                              request_critical(inc_request_critical),
                                                                                                                                                              S(sem_count),
                                                                                                                                                              number_of_processes(Max_processes)
{
    srand((unsigned)time(0));

    processes = inc_processes;
    // alocate memory for Semaphores
    semaphores = new Semaphore *[S];
    for (int i = 0; i < S; i++)
    {
        semaphores[i] = new Semaphore();
    }
    // edw eixa lathos
    statistics = (stats *)malloc(sizeof(stats) * Max_processes);
    for (int i = 0; i < Max_processes; i++)
    {
        statistics[i].counter = 0;
        statistics[i].overall_waiting_timeslots = 0;
        statistics[i].sem_waiting_timeslots = 0;
    }
}

/*==================== Destructor =============================*/

Process_Sheduler::~Process_Sheduler()
{
    // release memory
    for (int i = 0; i < S; i++)
    {
        delete (semaphores[i]);
    }
    delete (semaphores);

    free(statistics);
}

/*==================== Check for Incoming Processes =============================*/

void Process_Sheduler::check_incoming_PCB()
{
    // for each process we check if there is any new incoming and add it to the PQ
    for (int position = 0; position < processes.size(); position++)
    {

        if (processes[position].get_arrival() <= current_time)
        {

            if (processes[position].get_state() == NEW)
            {

                // add the new process to our stats set it's state to ready and push it in the priority queue
                statistics[processes[position].get_priority() - 1].counter++;

                processes[position].set_state(READY);
                ready_queue.push(&processes[position]);
            }
        }
    }
}

/* ============== Handle a Waiting Process =================*/

const void Process_Sheduler::HandleWaiting(int sem_id, PCB *process)
{

    if (process->get_state() != WAITING)
    {
        /* code */
        semaphores[sem_id]->push(process);
        // the waiting process takes it's place
        process->set_state(WAITING);
        process->print();
    }

    if (process->get_pid() != semaphores[sem_id]->get_front_process()->get_pid())
    {
        // This process gets back in the queue
        cout << "Process " << processes[process->get_pid()].get_pid() << " has state " << processes[process->get_pid()].get_state() << "\n";
        getchar();
        ready_queue.push(process);
    }

    Current = semaphores[sem_id]->get_front_process();
    Current->print();
    // Set top process to RUNNING
    Current->set_state(RUNNING);
    // new code
    Current->set_access(true);
    Run_InCritical(Current);
    semaphores[sem_id]->pop();
}

/*==================== Run Process in Critical Section Function =============================*/

const void Process_Sheduler::Run_InCritical(PCB *process)
{

    process->set_remaining_time(timeslot);
    process->set_burst_time(timeslot);

    // If the process is done with Critical Section
    if (process->get_remaining_time() <= 0.0)
    {

        // If there is some error kill the process
        if (!semaphores[process->get_semaphore()]->try_up())
        {

            process->set_state(TERMINATED);
            semaphores[process->get_semaphore()]->force_up();
            number_of_processes--;
            Current = NULL;
            return;
        }
        // reInit critical section time
        process->set_access(false);
        process->set_remaining_time(process->get_critical_time());
    }
    // if process still has time in CS but burst time is over -- kill and force up semaphore
    if (process->get_burst_time() <= 0.0)
    {
        process->set_state(TERMINATED);
        if (process->get_remaining_time() <= 0.0)
        {
            semaphores[Current->get_semaphore()]->force_up();
        }
        number_of_processes--;
        return;
    }

    // process->set_state(READY);
    ready_queue.push(process);
}

/*==================== Run Process Function =============================*/

const void Process_Sheduler::Run_Process(PCB *process)
{

    double execution_time = timeslot;

    if (process->in_critical())
    {
        Run_InCritical(process);

        // return so we don't try another lock on semaphore
        return;
    }

    if (process->get_state() == WAITING)
    {
        cout << "ANY PROCESS WAITING?\n";
        HandleWaiting(process->get_semaphore(), process);
        return;
    }

    // if the process managed to try down a semaphore
    if (calculate_outcome(request_critical))
    {

        int random = rand() % S;
        process->set_semaphore(random);

        // If we manage to down semaphore Run in critical Section else push in the queue and pop front instead to run
        if (semaphores[random]->try_down())
        {
            process->set_access(true);
            Run_InCritical(process);
        }
        else
        {
            HandleWaiting(random, process);
        }

        return;
    }

    // Normal use of the CPU
    process->set_burst_time(timeslot);

    // If Process is done set to Terminated
    if (process->get_burst_time() <= 0.0)
    {
        process->set_state(TERMINATED);
        number_of_processes--;
        return;
    }

    process->set_state(READY);
    // in any other case push the process back in the queue
    ready_queue.push(process);
}

/*====================Run Scheduler Function =============================*/

const void Process_Sheduler::Run_Scheduler()
{

    // keep running until all processes are done (or killed)
    while (number_of_processes > 0)
    {
        check_incoming_PCB();

        current_time += timeslot;

        // If there is a process in the queue Run the process
        if (!ready_queue.empty())
        {

            Current = ready_queue.top();
            ready_queue.pop();
            Current->set_state(RUNNING);

            Run_Process(Current);
        }

        // Calculate th waiting times
        for (int c_process = 0; c_process < processes.size(); c_process++)
        {
            cout << "Process " << processes[c_process].get_pid() << " has state " << processes[c_process].get_state() << "\n";
            // if process is not the running process add 1 timeslot to their waiting
            if (processes[c_process].get_state() == READY || processes[c_process].get_state() == WAITING)
            {
                statistics[processes[c_process].get_priority() - 1].overall_waiting_timeslots++;

                // if a process is waiting for a semaphore add 1 timeslot to their sem waiting time
                if (processes[c_process].get_state() == WAITING)
                {
                    cout << "ANY WAITING PROCESS?\n";
                    getchar();
                    statistics[processes[c_process].get_priority() - 1].sem_waiting_timeslots++;
                }
            }
        }
    }
}

const void Process_Sheduler::Print_stats()
{
    for (int priority = 0; priority < 7; priority++)
    {
        float average_overall = 0.0;
        float average_sem = 0.0;

        if (statistics[priority].counter > 0)
        {
            average_overall = 1.0 * statistics[priority].overall_waiting_timeslots / statistics[priority].counter;
            average_sem = 1.0 * statistics[priority].sem_waiting_timeslots / statistics[priority].counter;

            cout << "For priority :" << priority + 1
                 << "\naverage overall waiting time " << average_overall
                 << "\naverage blocked waiting time " << average_sem << "\n";
        }
    }
}
