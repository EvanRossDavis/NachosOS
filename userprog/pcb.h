#include "synch.h"
#include "thread.h"

#ifdef USER_PROGRAM
#include "machine.h"
#include "addrspace.h"
#endif

class Thread;

// CPU register state to be saved on context switch.  
// The SPARC and MIPS only need 10 registers, but the Snake needs 18.
// For simplicity, this is just the max over all architectures.
#define MachineStateSize 18 

// Process state
enum ProcessStatus { JUST_CREATED, RUNNING, READY, BLOCKED };

// external function, dummy routine whose sole job is to call Process::Print
extern void ProcessPrint(int arg);

//---------NEED GLOBAL LIST OF PROCESSES SOMEWHERE------
//-------create pcb manager------
class PCB {
    public:
        PCB(Thread *input);
        ~PCB();
        int getID();
        int getParentID();
        void setPid();

    private:
        int MAX_FILES;
        Thread *processThread;
        int processID;
        PCB *parent_process;
        int parentID;
        int childProcessID;
        int numProcesses;
        Addrspace *space;

        //List for child processes - pcbs
        List *children;

    // some of the private data for this class is listed above
    int* stack; 	 		// Bottom of the stack 
					// NULL if this is the main process
					// (If NULL, don't deallocate stack)
    ProcessStatus status;		// ready, running or blocked
    char* name;

    void StackAllocate(VoidFunctionPtr func, int arg);
    					// Allocate a stack for process.
					// Used internally by Fork()

extern "C" {
// First frame on process execution stack; 
//   	enable interrupts
//	call "func"
//	(when func returns, if ever) call ProcessFinish()
void ProcessRoot();

// Stop running oldProcess and start running newProcess
void SWITCH(PCB *oldProcess, PCB *newProcess);
}
};