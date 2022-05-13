#ifndef __GENERATER__
#define __GENERATER__

#include <iostream>
#include <fstream>
#include <string>
#include "../parser/net.h"

class Generator
{
public:
    void writeHeader(std::ofstream &resultFile);
    void writePathToDef(std::ofstream &resultFile, int netId);

private:
    static bool noCmp(const Net* a, const Net* b); 
    void addVia(std::ofstream &resultFile, const int layerId, const int netId);
    void addiniNode(std::ofstream &resultFile, const int layerId, const int netId);
    void convertCoor(int gridX, int gridY, int *arrCoor, int layerId);
    void addPath(std::ofstream &resultFile, const int layerId, const int netId); 
};

#endif