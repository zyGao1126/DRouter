#ifndef __CONFIG__
#define __CONFIG__

#include "../parser/layer.h"
#include "../parser/instance.h"
#include "../mempool/mempool.h"
#include "../parser/net.h"

#define MICRONS 2000
#define OBSFLAG 0xFFFF
#define PINFLAG 0xFFFE
#define SPACEFLAG -0xFFFF
#define RRRLIMIT 3

extern const int lefMetalLine;
extern const int lefViaLine;
extern const int gridSlack;
extern int DiareaX;
extern int DiareaY;
extern int netCount;
extern std::string defFileName;
extern std::string lefFileName;
extern std::string resultFileName;

extern MemoryPool<MetalLayer> metalMemPool;
extern MemoryPool<ViaLayer> viaMemPool;
extern MemoryPool<Cell> cellMemPool;
extern MemoryPool<Inst> instMemPool;
extern MemoryPool<Net> netMemPool;

extern std::vector<MetalLayer *> metalList;
extern std::vector<ViaLayer *> viaList;
extern std::vector<Cell *> cellList;
extern std::vector<Inst *> instList;
extern std::vector<Net *> netList;
extern std::vector<std::vector<int>> *finalPath;

extern int *layerList;

void deallocate();

#endif