#include "synch.h"
#include "bitmap.h"
#include "machine.h"

class MemoryManager {
    public:
        MemoryManager(int numTotalPages);
        ~MemoryManager();
        int getPage(); //call find to find a clear page
        void clearPage(int pageId);
    
    private:
        BitMap *virtMem; //Use the bitmap to simulate entire physical memory
                        // Indicate what memory is available by setting bits
        Lock *lock;

};