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
//enum ProcessStatus { JUST_CREATED, RUNNING, READY, BLOCKED };

// external function, dummy routine whose sole job is to call Process::Print
extern void ProcessPrint(int arg);

class PCB {
    public:
        PCB();
        ~PCB();
        void addToProcessList(); //adds current process to process list for the manager
        int getID() {return processID;}
        int getParentID() {return parentID;}
        int getStatus() {return status;}
        void setPid();
        void setStatus();
        bool hasChildren() {if(numChildren != 0) return true;}
        void addChild();
        void setParentToNull();
        void setThread(Thread* thread){processThread = thread;}
        AddrSpace *space;

    private:
        int MAX_FILES;
        Thread *processThread;
        int processID;
        PCB *parent_process;
        int parentID;
        int childProcessID;
        int numProcesses;
        int numChildren;
        int status;

        //List for child processes - pcbs
        List *children;

    
    // some of the private data for this class is listed above
    int* stack; 	 		// Bottom of the stack 
					// NULL if this is the main process
					// (If NULL, don't deallocate stack)
    //ProcessStatus status;		// ready, running or blocked
    char* name;

    void StackAllocate(VoidFunctionPtr func, int arg);
    					// Allocate a stack for process.
					// Used internally by Fork()
};