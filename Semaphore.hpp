#include <queue>
#include "PCB.hpp"
using std::queue;

class Semaphore
{
private:

    int sem_switch;
    queue <PCB*> pids;
    

    
public:
    Semaphore();
    ~Semaphore();
    int try_down();
    int try_up();

    // to use only on deadlock
    void force_up();
    
    PCB* get_front_process();
    void push(PCB* pid);
    void pop();
    
};
