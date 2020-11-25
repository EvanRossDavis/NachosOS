// threadtest.cc 
//	Simple test case for the threads assignment.
//
//	Create two threads, and have them context switch
//	back and forth between themselves by calling Thread::Yield, 
//	to illustratethe inner workings of the thread system.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.
#include "copyright.h"
#include "system.h"
#include "synch.h"
#include "stdint.h"
#include "list.h"

#ifdef HW1_ELEVATOR
struct PersonThread{
	int id;
	int atFloor;
	int toFloor;
};

struct ElevatorThread{
	int numFloors;
	int currentFloor;
	int numPeopleIn;

};

int globalID = 0;   //Global ID
int pickUp[10] = {0,0,0,0,0,0,0,0,0,0}; //List of people waiting to be picked up
int dropOff[5] = {0,0,0,0,0};           //List of people waiting in the elevator

// Various locks
Lock enterLock = Lock::Lock("Entering Lock");
Lock exitLock = Lock::Lock("Exiting Lock");
Lock requestLock = Lock::Lock("Request Lock");

// Various CVs
Condition enterEle = Condition::Condition("Get picked up"); 
Condition exitEle = Condition::Condition("Leave Elevator");
Condition noRequests = Condition::Condition("When no one needs the elevator");

int gCurrentFloor = 1;  //Global current floor

int remove(int a[], int size, int element)  //Remove an element from a given array (and shift)
{
    int i;
    for(i = 0; i < size; i++)
    {
        if(a[i] == element)
        {
            for(i = i; i < size - 1; i++)
            {
                a[i] = a[i + 1];
            }
            a[i] = 0;
            return i;
        }
    }
    return -1; //Did not exist
}

int insert(int a[], int size, int element)  //Insert an element to a given array
{
    int i;
    for(i = 0; i < size; i++)
    {
        if(a[i] == 0)
        {
            a[i] = element;
            return 1;
        }
    }
    return -1; //No space in array
}

void personFunc(int person) //Function for the person thread behavoir
{
	struct PersonThread* p = (struct PersonThread*)person;
    printf("Person wants to go to floor %d from floor %d\n", p->id, p->atFloor, p->toFloor);

    enterLock.Acquire();
    requestLock.Acquire();                    
    insert(pickUp, 10, p->atFloor);         //Add itself to pick up list
    noRequests.Signal(&requestLock);        //Tell elevator it now has a request
    requestLock.Release();
    while(p->atFloor != gCurrentFloor)      //Waiting for elevator
    {
        enterEle.Wait(&enterLock);
    }
    printf("Person %d got into the elevator\n", p->id);
    remove(pickUp, 10, p->atFloor);         //Remove from pickUp list as its now in elevator
    insert(dropOff, 5, p->toFloor);         //Add itself to drop off list (enter elevator)

    enterEle.Signal(&enterLock);            //Tell elevator it got on
    enterLock.Release();                    
    exitLock.Acquire();   
    printf("%d got exit lock\n", p->id);                  
    while(p->toFloor != gCurrentFloor)      //Waiting for floor
    {
        exitEle.Wait(&exitLock);
    }
    remove(dropOff, 5, p->toFloor);         //Arrived at desired floor
    printf("Person %d got out of the elevator\n", p->id);
    exitEle.Signal(&exitLock);              //Tell elevator it got off
    exitLock.Release();
    //Terminate thread
}

void ArrivingGoingFromTo(int atFloor, int toFloor)  //Creates fork for person thread
{
	struct PersonThread* p = new PersonThread;
	p->id = globalID;
    globalID++;
	p->atFloor = atFloor;
	p->toFloor = toFloor;
	
	Thread* t = new Thread("Person thread");
    t->Fork(personFunc, (intptr_t)p);
}


void elevatorFunc(int numFloors)    //Function for the elevator thread behavoir
{
    struct ElevatorThread* e = new ElevatorThread;
    e->numFloors = numFloors;   //Top floor
    e->currentFloor = 1;    //Not really used -- gCurrentFloor used instead
    e->numPeopleIn = 0;     //Not really used -- dropOff[4] tells if full or not
    int up = 1;     //Used as a boolean for direction of elevator
    int i = 0;      //Used for index for arrays
    gCurrentFloor = e->currentFloor;
    printf("Elevator arrives on floor 1\n");

    while(1)
    {
    requestLock.Acquire();  
    while(pickUp[0] == 0 && dropOff[0] == 0)
    {
        noRequests.Wait(&requestLock);   
    }
    requestLock.Release();
    //Acquire both locks so no one can enter/exit elevator while moving
        enterLock.Acquire();
        exitLock.Acquire();

    for(int i = 0; i < 50; i++) //Move floors
    {
        //Wait 50 ticks
    }

    //Update current floor
    if(up == 1)     //If upwards direction
    {
        e->currentFloor++;
        gCurrentFloor++;
    }
    else            //If downwards direction
    {
        e->currentFloor--;
        gCurrentFloor--;   
    }
    printf("Elevator arrives on floor %d\n", gCurrentFloor);

    //Update direction
    if(e->currentFloor == e->numFloors) //At top floor, go down now
    {
        up = 0;
    }
    else if(e->currentFloor == 1)       //At bottom floor, go up now
    {
        up = 1;
    }

    //dropOff/pickUp people
    for(i = 0; i < 5; i++)
    {
    if(dropOff[i] == gCurrentFloor) //If SOMEONE wants to get off
    {
        exitEle.Broadcast(&exitLock);
        exitEle.Wait(&exitLock);
    }
    }

    for(i = 0; i < 10; i++)
    {
    if(pickUp[i] == gCurrentFloor && dropOff[4] == 0)   //If SOMEONE wants to get on AND not full
    {
        exitLock.Release();
        enterEle.Broadcast(&enterLock);
        enterEle.Wait(&enterLock);
    }
    }
    enterLock.Release();
    exitLock.Release();
    }
}


void Elevator(int numFloors)
{
	Thread* t = new Thread("Elevator thread");
	t->Fork(elevatorFunc, numFloors);
}
#endif

// testnum is set in main.cc
int testnum = 1, notDone = 0, SharedVariable;
//----------------------------------------------------------------------
// SimpleThread
// 	Loop 5 times, yielding the CPU to another ready thread 
//	each iteration.
//
//	"which" is simply a number identifying the thread, for debugging
//	purposes.
//----------------------------------------------------------------------

#ifdef HW1_LOCKS
Lock lock = Lock::Lock("lock");
#endif

#ifdef HW1_SEMAPHORES
Semaphore sem = Semaphore::Semaphore("sem", 1);
#endif 

void SimpleThread(int which) {
    int num, val;
    for (num = 0; num < 5; num++) {
        #ifdef HW1_SEMAPHORES
        sem.P();
        #endif 
        
        #ifdef HW1_LOCKS
        lock.Acquire();
        #endif
        
        val = SharedVariable;
        printf("*** thread %d sees value %d\n", which, val);
        currentThread->Yield();
        SharedVariable = val + 1;
        
        #ifdef HW1_SEMAPHORES
        sem.V();
        #endif
        
        #ifdef HW1_LOCKS
        lock.Release();
        #endif
        
        currentThread->Yield();
    }
    
    notDone--;
    while (notDone > 0)
        currentThread->Yield();
    
    val = SharedVariable;
    printf("Thread %d sees final value %d\n", which, val);
}

//----------------------------------------------------------------------
// ThreadTest1
// 	Set up a ping-pong between two threads, by forking a thread 
//	to call SimpleThread, and then calling SimpleThread ourselves.
//----------------------------------------------------------------------

void
ThreadTest1(int n)
{
    DEBUG('t', "Entering ThreadTest1");
    
    for (int i = 1; i <= n; i++)
    {
        Thread* t = new Thread("forked thread");
        t->Fork(SimpleThread, i);
    }
    
    SimpleThread(0);
}

//----------------------------------------------------------------------
// ThreadTest
// 	Invoke a test routine.
//----------------------------------------------------------------------

void
ThreadTest(int n)
{
    switch (testnum) {
    case 1:
    notDone = n+1;
    ThreadTest1(n);
    break;
    default:
	printf("No test specified.\n");
	break;
    }
}

