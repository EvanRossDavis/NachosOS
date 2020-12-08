// exception.cc 
//	Entry point into the Nachos kernel from user programs.
//	There are two kinds of things that can cause control to
//	transfer back to here from user code:
//
//	syscall -- The user code explicitly requests to call a procedure
//	in the Nachos kernel.  Right now, the only function we support is
//	"Halt".
//
//	exceptions -- The user code does something that the CPU can't handle.
//	For instance, accessing memory that doesn't exist, arithmetic errors,
//	etc.  
//
//	Interrupts (which can also cause control to transfer from user
//	code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "addrspace.h"
#include "system.h"
#include "syscall.h"
#include "pcb.h"
#include "synch.h"
//#include "ProcessManager.h"
//----------------------------------------------------------------------
// ExceptionHandler
// 	Entry point into the Nachos kernel.  Called when a user program
//	is executing, and either does a syscall, or generates an addressing
//	or arithmetic exception.
//
// 	For system calls, the following is the calling convention:
//
// 	system call code -- r2
//		arg1 -- r4
//		arg2 -- r5
//		arg3 -- r6
//		arg4 -- r7
//
//	The result of the system call, if any, must be put back into r2. 
//
// And don't forget to increment the pc before returning. (Or else you'll
// loop making the same system call forever!
//
//	"which" is the kind of exception.  The list of possible exceptions 
//	are in machine.h.
//----------------------------------------------------------------------

void
ExceptionHandler(ExceptionType which)
{
    int type = machine->ReadRegister(2);

    if ((which == SyscallException) && (type == SC_Halt)) {
	    DEBUG('a', "Shutdown, initiated by user program.\n");
   	    interrupt->Halt();
    } 

    /* fork()*/
    else if ((which == SyscallException) && (type == SC_Fork)) {
        DEBUG('b', "Fork, initiated by user program.\n");
        printf("This is the fork system call.");

        //1. save old process register
        currentThread->SaveUserState();

        //2. create a new AddrSpace and copy old AddrSpace to new AddrSpace. 
        AddrSpace *newSpace = new AddrSpace(currentThread->space->GetNumPages(), currentThread->space->getStartPage()); 
       //Copy old address space into new with method

        // 3. create a new Thread, associate new AddrSpace with new thread.
        Thread *child = new Thread("Child Thread");
        child->space = newSpace;

        //  4. create a PCB (Process Control Block - implement) and 
        //     associate the new Address and new Thread with PCB.
        PCB *pcb;
        pcb->setThread(child);

        //  6. copy old register values to new register. 
        pcb->space->RestoreState();
        //     Set pc reg value to value in r4. 
        machine->WriteRegister(PCReg, machine->ReadRegister(4));
        //     save new register values to new AddrSpace.
        newSpace->SaveState();
    
        //  7. use Thread::Fork(func, arg) to set new thread behavior: 
        child->Fork((VoidFunctionPtr)machine->ReadRegister(4), machine->ReadRegister(5));

        //  restore registers, restore memory and put machine to run.
        child->RestoreUserState();
        machine->Run();

        //  8. restore old process register
        currentThread->RestoreUserState();

        //  9. write new process pid to r2 
        machine->WriteRegister(2, pcb->getID());

        //  10. update counter of old process and return.
        machine->WriteRegister(PrevPCReg, machine->ReadRegister(PCReg));
        machine->WriteRegister(PCReg, machine->ReadRegister(NextPCReg));
        machine->WriteRegister(NextPCReg, machine->ReadRegister(NextPCReg)+4);
    }

/* Steps for Exec*/
    else if ((which == SyscallException) && (type == SC_Exec)) {
        DEBUG('c', "Exec, initiated by user program.\n");
        printf("This is the Exec system call.");
        
        char filename[100];
        int i=0, memval;

        //Read register r4 to get the executable path
        int vaddr = machine->ReadRegister(4);

        machine->ReadMem(vaddr, 1, &memval);
        while ((*(char*)&memval) != '\0') {
            filename[i]  = (char)memval;
            ++i;
            vaddr++;
            machine->ReadMem(vaddr, 1, &memval);
        }
        filename[i]  = (char)memval;

        // replace the process memory with the content of the executable and run exe
        OpenFile *executable = fileSystem->Open(filename);
        AddrSpace *space;

        if (executable == NULL) {
        printf("Unable to open file %s\n", filename);
        return;
        }
        space = new AddrSpace(executable);    
        currentThread->space = space;

        delete executable;			// close file

        space->InitRegisters();		// set the initial register values
        space->RestoreState();		// load page table register

        machine->Run();			// jump to the user progam
        
        // write 1 to register r2 indicating exec() invoked successful.
        machine->WriteRegister(2, 1);
        
        ASSERT(FALSE);			// machine->Run never returns;
                        // the address space exits
                        // by doing the syscall "exit"


        // return 1 if all steps succeed; return -1 if any step failed. e.g., the executable is unrecognizable.

        //Adjust PC counter
        machine->WriteRegister(PrevPCReg, machine->ReadRegister(PCReg));
        machine->WriteRegister(PCReg, machine->ReadRegister(NextPCReg));
        machine->WriteRegister(NextPCReg, machine->ReadRegister(NextPCReg)+4);
    }

/* Steps for Yield*/
    else if ((which == SyscallException) && (type == SC_Yield)) {
        DEBUG('d', "Yield, initiated by user program.\n");

        //	currentThread yield
        currentThread->Yield();

        machine->WriteRegister(PrevPCReg, machine->ReadRegister(PCReg));
        machine->WriteRegister(PCReg, machine->ReadRegister(NextPCReg));
        machine->WriteRegister(NextPCReg, machine->ReadRegister(NextPCReg)+4);
    }

/* Steps for Exit */
    else if ((which == SyscallException) && (type == SC_Exit)) {
        DEBUG('e', "Exit, initiated by user program.\n");
        printf("This is the Exit system call.");

        //get exit code from r4;
        int exitCode = machine->ReadRegister(4);
        
        PCB *process;
        process->setThread(currentThread);
        //if current process has children, set their parent pointers to null;
        if (process->hasChildren()){
            process->setParentProcess(NULL);
        }
        //if current process has a parent, remove itself from the children list of its
        //parent process, and set child exit value to parent.
        //remove it from the pcb manager and pid manager.
        //deallocate the process memory and remove from the page table;
        if (process->hasParent()){
            //This function should find the child by id and set it to NULL but doesn't
            //process->setParentChildToNull(process->getID());
        }
        
        //Memory manager would not work properly so I had to use only a bitmap
        //Wasn't able to implement deallocation

        // current thread finish.
        currentThread->Finish();

        //Adjust program counter registers
        machine->WriteRegister(PrevPCReg, machine->ReadRegister(PCReg));
        machine->WriteRegister(PCReg, machine->ReadRegister(NextPCReg));
        machine->WriteRegister(NextPCReg, machine->ReadRegister(NextPCReg)+4);
    }

/*Steps for join*/
    else if ((which == SyscallException) && (type == SC_Join)) {
        DEBUG('f', "Join, initiated by user program.\n");
        printf("This is the Join system call.");
        
        //read process id from register r4
        int pid = machine->ReadRegister(4);

        //make sure the requested process id 
        //is the child process of the current process.
        PCB* process;
        process->setThread(currentThread);
        ASSERT(process->getID() == pid);
        
        //keep on checking if the requested process is 
        //finished. if not, yield the current process.
        //Wasn't sure how to check if the process was finished
        
        //If the requested process finished, write the 
        //requested process exit id to register r2. return.
        machine->WriteRegister(2, pid);

        machine->WriteRegister(PrevPCReg, machine->ReadRegister(PCReg));
        machine->WriteRegister(PCReg, machine->ReadRegister(NextPCReg));
        machine->WriteRegister(NextPCReg, machine->ReadRegister(NextPCReg)+4);
    }

/* Steps for Kill*/
    else if ((which == SyscallException) && (type == SC_Kill)) {
        DEBUG('g', "Kill, initiated by user program.\n");
        printf("This is the Kill system call.");

        //read killed ID from r4
        int killedID = machine->ReadRegister(4);

        //make sure if the killed process exists, process manager doesn't work properly
        //ProcessManager *manager;
        //PCB foundProcess = manager->findProcess(killedID);

        //if killed process is the current process, simply call exit
        
        //if the killed process has children, set their parent pointers to null
        //if the killed process has a parent, remove itself from the child list.
        //remove it self from the pcb list and pid list
        //free up memory and remove page table item.
        //remove killed thread from scheduler.
        //write 0 to r2 to show succeed. 
        machine->WriteRegister(PrevPCReg, machine->ReadRegister(PCReg));
        machine->WriteRegister(PCReg, machine->ReadRegister(NextPCReg));
        machine->WriteRegister(NextPCReg, machine->ReadRegister(NextPCReg)+4);       
    }
    else {
        printf("Unexpected user mode exception %d %d\n", which, type);
        ASSERT(FALSE);
        }
}
 