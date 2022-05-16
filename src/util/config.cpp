#include "config.h"
#include "../mempool/mempool.hpp"
#include <algorithm> 

using std::vector;
using std::string;

const int lefMetalLine = 7;
const int lefViaLine = 2;
const int gridSlack = 5;
int DiareaX = 0;
int DiareaY = 0;
int netCount;
string defFileName;
string lefFileName;
string resultFileName;

memoryPool<MetalLayer> metalMemPool;
memoryPool<ViaLayer> viaMemPool;
memoryPool<Cell> cellMemPool;
memoryPool<Inst> instMemPool;
memoryPool<Net> netMemPool;


vector<MetalLayer *> metalList;
vector<ViaLayer *> viaList;
vector<Cell *> cellList;
vector<Inst *> instList;
vector<Net *> netList;
vector<vector<int>> *finalPath;
int *layerList;

void deallocate() 
{
    // free correspond mempool
    metalMemPool.destroy();
    viaMemPool.destroy();
    cellMemPool.destroy();
    instMemPool.destroy();
    netMemPool.destroy();
    // free route path
    delete [] finalPath;  
    // free layer id
    delete [] layerList;
}