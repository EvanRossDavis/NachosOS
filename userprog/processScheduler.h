// The following class defines the scheduler/dispatcher abstraction -- 
// the data structures and operations needed to keep track of which 
// thread is running, and which threads are ready but not running.
#include "sync.h"
#include "pcb.h"
#include "list.h"

class ProcessScheduler {
  public:
    ProcessScheduler();			// Initialize list of ready processes 
    ~ProcessScheduler();			// De-allocate ready list

    void ReadyToRun(PCB* process);	// Process can be dispatched.
    PCB* FindNextToRun();		// Dequeue first process on the ready 
					// list, if any, and return process.
    void Run(PCB* nextProcess);	// Cause nextProcess to start running
    void Print();			// Print contents of ready list
    
  private:
    List *readyList;  		// queue of processes that are ready to run,
				            // but not running
    List *waitList;
};
