#ifndef MEMPOOL
#define MEMPOOL

template <typename T>
class memoryPool 
{
private:
    //memory sub block
    struct memBlock 
    {
        T Element;
        memBlock *pNext;
        memBlock *pBefore;
    };
    //memory block
    struct memBlockSet 
    {
        memBlock *curBlock; 
        memBlockSet *pNext;
    };

private:
    // connect memory block set  
    memBlockSet *blockSetList;
    // point to current free block
    memBlock *freeBlockList;
    int blockNum; 
    int blockSetNum; 
    int usedNum;
    
public:
    memoryPool(): blockSetList(nullptr), freeBlockList(nullptr), blockNum(0), blockSetNum(0), usedNum(0) {}
    ~memoryPool() 
    {
        if (getPoolSize() > 0 || isCreated())
            destroy();
    }
    bool create(int num);
    void destroy();
    T* alloc();
    void freeMem(T *element);
    int getPoolSize();
    bool isCreated();
};

#endif