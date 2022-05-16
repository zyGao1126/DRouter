#ifndef MEMPOOL
#define MEMPOOL

template <typename T>
class MemoryPool 
{
private:
    //memory sub block
    struct MemBlock 
    {
        T Element;
        MemBlock *pNext;
        MemBlock *pBefore;
    };
    //memory block
    struct MemBlockSet 
    {
        MemBlock *curBlock; 
        MemBlockSet *pNext;
    };

private:
    // connect memory block set  
    MemBlockSet *blockSetList;
    // point to current free block
    MemBlock *freeBlockList;
    int blockNum; 
    int blockSetNum; 
    int usedNum;
    
public:
    MemoryPool(): blockSetList(nullptr), freeBlockList(nullptr), blockNum(0), blockSetNum(0), usedNum(0) {}
    ~MemoryPool() 
    {
        if (GetPoolSize() > 0 || IsCreated())
            Destroy();
    }
    bool Create(int num);
    void Destroy();
    T* Alloc();
    void Free(T *element);
    int GetPoolSize();
    bool IsCreated();
};

#endif