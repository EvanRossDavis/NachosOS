#include "synch.h"
#include "thread.h"
#include "machine.h"
#include "addrspace.h"
#include "pcb.h"

class ProcessManager{
    public:
        ProcessManager();
        ~ProcessManager();
        PCB findProcess(int ID);
        void addProcess(PCB process);
        int removeProcess(PCB process);

    private:
        int SIZE = 100;
        PCB[] processList;
        int ID = 0;

}
