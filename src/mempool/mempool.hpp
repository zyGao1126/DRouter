#include "mempool.h"

template <typename T>
bool memoryPool<T>::create(int num) 
{
    blockNum = num;
    blockSetNum = 1;

    blockSetList = new memBlockSet;
    blockSetList->pNext = nullptr;
    blockSetList->curBlock = new memBlock [blockNum];
    
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
void memoryPool<T>::destroy() 
{
    while (blockSetList != nullptr) {
        if (blockSetList->curBlock) {
            delete [] blockSetList->curBlock;
            blockSetList->curBlock = nullptr;
        }
        memBlockSet *tmp = blockSetList;
        blockSetList = blockSetList->pNext;
        delete tmp;
    }
    usedNum = 0;
    blockSetNum = 0;
}

template <typename T>
T* memoryPool<T>::alloc() 
{
    //cur blockset has used up
    if (freeBlockList == nullptr) {
        memBlockSet *pSet = new memBlockSet;
        pSet->pNext = blockSetList;
        pSet->curBlock = new memBlock [blockNum];
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

    memBlock *pmemBlock = freeBlockList;
    freeBlockList = freeBlockList->pNext;
    if (freeBlockList)
        freeBlockList->pBefore = nullptr;
    return &(pmemBlock->Element);
}

template <typename T>
void memoryPool<T>::freeMem(T *element) 
{
    memBlock *pmemBlock = (memBlock *)element;
    pmemBlock->pBefore = nullptr;
    pmemBlock->pNext = freeBlockList;
    if (freeBlockList)    
        freeBlockList->pBefore = pmemBlock;
    freeBlockList = pmemBlock;
    usedNum--;
}

template <typename T>
int memoryPool<T>::getPoolSize() {
    return blockSetNum * blockNum;
}

template <typename T>
bool memoryPool<T>::isCreated() {
    return blockSetList != nullptr;
}