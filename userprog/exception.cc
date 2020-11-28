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
#include "system.h"
#include "syscall.h"

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

    /* fork()
 1. save old process register

 2. create a new AddrSpace and copy old AddrSpace to new AddrSpace. (Improve the current AddrSpace)

 3. create a new Thread, associate new AddrSpace with new thread.

 4. create a PCB (Process Control Block) and associate the new Address and new Thread with PCB. 

 5. complete PCB with information such as pid, ppid, etc.

 6. copy old register values to new register. Set pc reg value to value in r4. save new register values to new AddrSpace.

 7. use Thread::Fork(func, arg) to set new thread behavior: restore registers, restore memory and put machine to run.

 8. restore old process register

 9. write new process pid to r2 

 10. update counter of old process and return.

*/
    else if ((which == SyscallException) && (type == SC_Fork)) {
        DEBUG('b', "Fork, initiated by user program.\n");
    }

/* Steps for Exec
Read register r4 to get the executable path. 
replace the process memory with the content of the executable.
Init registers
write 1 to register r2 indicating exec() invoked successful.
return 1 if all steps succeed; return -1 if any step failed. e.g., the executable is unrecognizable.
*/

/* Steps for Yield
	currentThread yield
*/

/* Steps for Exit
get exit code from r4;
if current process has children, set their parent pointers to null;
if current process has a parent, remove itself from the children list of its
parent process, and set child exit value to parent.
remove it from the pcb manager and pid manager.
deallocate the process memory and remove from the page table;
current thread finish.
*/

/*Steps for join
read process id from register r4
make sure the requested process id is the child process of the current
process.
keep on checking if the requested process is finished. if not, yield the
current process.
If the requested process finished, write the requested process exit id to
register r2. return.
*/

/* Steps for Kill
read killed ID from r4
make sure if the killed process exists
if killed process is the current process, simply call exit
if the killed process has children, set their parent pointers to null
if the killed process has a parent, remove itself from the child list.
remove it self from the pcb list and pid list
free up memory and remove page table item.
remove killed thread from scheduler.
write 0 to r2 to show succeed.
*/
else {
	    printf("Unexpected user mode exception %d %d\n", which, type);
	    ASSERT(FALSE);
    }
}
 