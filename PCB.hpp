#include <iostream>
#ifndef PCB_HPP
#define PCB_HPP

enum state {
    NEW,
    READY,
    RUNNING,
    TERMINATED,
    WAITING
};


class PCB
{
private:
    int pid;                         // process PID
    int priority;                    // process priority   
    bool access;                     // has access to critical section
    state process_state;             // Process state
    double burst_time;               // total time to finish
    double remaining_time;           // remaining time to finish CS    
    double arrival_time;             // time of arrival
    double critical_section_time;    // time that process requires in critical section
    int locked_sem;                  // Requested Semaphore (or locked)

public:
    PCB(int pid , int priority , double burst_time, double arrival_time, double critical_section_time);
    ~PCB();

    /* ======== GETERS ========= */ 
    const int get_pid();
    const int get_priority();
    const int get_state();
    const double get_arrival();
    const int get_semaphore();
    const bool in_critical();
    const double get_remaining_time();
    const double get_burst_time(); 
    const double get_critical_time();

    /* ======== SETERS ========= */
    void set_priority(int new_priority);
    void set_state(state new_state);
    void set_turnaround_time(float timestep);
    void set_burst_time(float timestep);
    void set_remaining_time(float timestep);
    void set_semaphore(int new_sem);
    void set_access(bool inc_access);

    // Print Pid and Priority (used for debugging)
    const void print();
    
};

#endif // PCB_H
