#include "system.h"
#include "bitmap.h"
#include "machine.h"
#include "memorymanager.h"

MemoryManager::MemoryManager(int numPages){
    virtMem = new BitMap(numPages);
}

MemoryManager::~MemoryManager(){
    delete MemoryManager;
}

int MemoryManager::getPage(){
    virtMem->Find();
}

void MemoryManager::clearPage(int page){
    virtMem->Clear();
}