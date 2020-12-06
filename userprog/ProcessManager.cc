#include "synch.h"
#include "thread.h"
#include "machine.h"
#include "addrspace.h"
#include "pcb.h"
#include "ProcessManager.h"

ProcessManager::ProcessManager()
{
    processList = new PCB[SIZE];
}

~Process::ProcessManager()
{
    delete processList;
}

PCB ProcessManager::findProcess(int ID)
{
    int i;
    PCB processNotFound;
    for(i = 0; i < SIZE; i++)
    {
        if(processList[i] == null)
        {
            return processNotFound;
        }    
        if(processList[i].getID() == ID)
        {
            return processList[i];
        }
    }
    return processNotFound;
}

void ProcessManager::addProcess(PCB process)
{
    process.setID(ID);
    for(i = 0; i < SIZE; i++)
    {
        if(processList[i] == null)
        {
            processList[i] = process;
        }    
    }
    ID++;
}

int ProcessManager::removeProcess(PCB process)
{    
    int ID = process.getID();
    for(i = 0; i < SIZE; i++)
    {
        if(processList[i].getID() == ID)
        {
            processList[i] = null;
            return 1;
        }
    }
    return -1;
}
