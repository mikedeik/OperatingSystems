#include <iostream>
#include <stdlib.h>
#include <ctime>
#include "PCB.hpp"
#include <unistd.h>

using namespace std;

PCB::PCB(int pid, int priority, double burst_time, double arrival_time, double critical_section_time) : pid(pid),
                                                                                                        priority(priority),
                                                                                                        access(false),
                                                                                                        burst_time(burst_time),
                                                                                                        remaining_time(burst_time),
                                                                                                        arrival_time(arrival_time),
                                                                                                        critical_section_time(critical_section_time),
                                                                                                        locked_sem(-1),
                                                                                                        process_state(NEW)
{
}
PCB::~PCB() {}

const int PCB::get_pid()
{
    return pid;
}

const int PCB::get_priority()
{
    return priority;
}
const double PCB::get_arrival()
{
    return arrival_time;
}

const int PCB::get_semaphore()
{
    return locked_sem;
}

const double PCB::get_remaining_time()
{
    return remaining_time;
}
const double PCB::get_burst_time()
{
    return burst_time;
}

const int PCB::get_state()
{
    return process_state;
}

const bool PCB::in_critical()
{
    return access;
}

const double PCB::get_critical_time()
{
    return critical_section_time;
}

void PCB::set_priority(int new_priority)
{
    priority = new_priority;
}

void PCB::set_state(state new_state)
{
    process_state = new_state;
}

void PCB::set_semaphore(int new_sem)
{
    locked_sem = new_sem;
}

void PCB::set_burst_time(float timestep)
{
    burst_time -= timestep;
}

void PCB::set_remaining_time(float timestep)
{
    remaining_time -= timestep;
}

void PCB::set_access(bool inc_access)
{
    access = inc_access;
}

const void PCB::print()
{
    cout << "Pid : " << pid << "\n";
    cout << "Priority : " << priority << "\n";
    cout << "Arrival : " << arrival_time << "\n";
    cout << "Remaining : " << remaining_time << "\n";
    cout << "Burst : " << burst_time << "\n";
    cout << "Semaphore : " << locked_sem << "\n";
    cout << "State : " << process_state << "\n";
}
