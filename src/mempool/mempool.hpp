#include "mempool.h"

template <typename T>
bool MemoryPool<T>::Create(int num) 
{
    blockNum = num;
    blockSetNum = 1;

    blockSetList = new MemBlockSet;
    blockSetList->pNext = nullptr;
    blockSetList->curBlock = new MemBlock [blockNum];
    
    for (int i = 0; i < blockNum; i++) {
        if (i > 0) {
            blockSetList->curBlock[i].pBefore = &(blockSetList->curBlock[i-1]);
            blockSetList->curBlock[i-1].pNext = &(blockSetList->curBlock[i]);
        }
        blockSetList->curBlock[0].pBefore = nullptr;
    }
    blockSetList->curBlock[blockNum - 1].pNext = nullptr;

    freeBlockList = blockSetList->curBlock;
    usedNum = 0;
    return true;
}

template <typename T>
void MemoryPool<T>::Destroy() 
{
    while (blockSetList != nullptr) {
        if (blockSetList->curBlock) {
            delete [] blockSetList->curBlock;
            blockSetList->curBlock = nullptr;
        }
        MemBlockSet *tmp = blockSetList;
        blockSetList = blockSetList->pNext;
        delete tmp;
    }
    usedNum = 0;
    blockSetNum = 0;
}

template <typename T>
T* MemoryPool<T>::Alloc() 
{
    //cur blockset has used up
    if (freeBlockList == nullptr) {
        MemBlockSet *pSet = new MemBlockSet;
        pSet->pNext = blockSetList;
        pSet->curBlock = new MemBlock [blockNum];
        for (int i = 0; i < blockNum; i++) {
            if (i > 0) {
                pSet->curBlock[i].pBefore = &(pSet->curBlock[i-1]);
                pSet->curBlock[i - 1].pNext = &(pSet->curBlock[i]);
            }
            pSet->curBlock[0].pBefore = nullptr;
        }
        pSet->curBlock[blockNum - 1].pNext = nullptr;
        freeBlockList = pSet->curBlock;
        blockSetList = pSet;
        blockSetNum++;
    }

    MemBlock *pMemBlock = freeBlockList;
    freeBlockList = freeBlockList->pNext;
    if (freeBlockList)
        freeBlockList->pBefore = nullptr;
    return &(pMemBlock->Element);
}

template <typename T>
void MemoryPool<T>::Free(T *element) 
{
    MemBlock *pMemBlock = (MemBlock *)element;
    pMemBlock->pBefore = nullptr;
    pMemBlock->pNext = freeBlockList;
    if (freeBlockList)    
        freeBlockList->pBefore = pMemBlock;
    freeBlockList = pMemBlock;
    usedNum--;
}

template <typename T>
int MemoryPool<T>::GetPoolSize() {
    return blockSetNum * blockNum;
}

template <typename T>
bool MemoryPool<T>::IsCreated() {
    return blockSetList != nullptr;
}