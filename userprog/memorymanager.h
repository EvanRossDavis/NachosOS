#include "sync.h"
#include "bitmap.h"

class MemoryManager {
    public:
        MemoryManager(int numTotalPages);
        ~MemoryManager();
        int getPage();
        void clearPage(int pageId);
    
    private:
        Bitmap *virtMem;
        Lock *lock;

};