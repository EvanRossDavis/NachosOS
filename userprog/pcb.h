#include "synch.h"

class Thread;

class PCB {
    public:
        PCB(Thread *input);
        ~PCB();
        int getID();
        void setPid();

    private:
        int MAX_FILES;
        Thread * processThread;
        int processID;
        PCB *parent_process;
        int numProcesses;
};