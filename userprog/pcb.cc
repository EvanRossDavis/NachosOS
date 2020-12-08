#include "system.h"
#include "pcb.h"
#include "machine.h"
#include "addrspace.h"
#include "thread.h"


PCB::PCB(Thread *input){
    processThread = input;
    status = JUST_CREATED;
    children = NULL;
    space = intput->space;
    //space = new AddrSpace()

}

// PCB::setStatus(int num){
//     if (num == 1) status = JUST_CREATED;
//     if (num == 2) status = RUNNING;
//     if (num == 3) status = READY;
//     if (num = 4) status = BLOCKED;

// }

// void PCB::setParentToNull(){
//     parentProcess = NULL;
// }

void PCB::addChild(Thread *thread){
    children->Append(thread);
}