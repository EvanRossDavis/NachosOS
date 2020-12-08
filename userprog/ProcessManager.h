#include "synch.h"
#include "thread.h"
#include "machine.h"
#include "addrspace.h"
#include "pcb.h"

const int SIZE = 100;

class ProcessManager{
    public:
        ProcessManager();
        ~ProcessManager();
        PCB findProcess(int id);
        void addProcess(PCB process);
        int removeProcess(PCB process);

    private:
        int id;
        PCB processList[SIZE];
};
