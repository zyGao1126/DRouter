#include "config.h"
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

vector<MetalLayer *> metalList;
vector<ViaLayer *> viaList;
vector<Cell *> cellList;
vector<Inst *> instList;
vector<Net *> netList;
vector<vector<int>> *finalPath;
int *layerList;

void deallocate() 
{
    // std::for_each(metalList.begin(), metalList.end(), [] (MetalLayer*& iter) {delete iter; iter = nullptr; });
    // std::for_each(viaList.begin(), viaList.end(), [] (ViaLayer*& iter) {delete iter; iter = nullptr; });
    // std::for_each(netList.begin(), netList.end(), [] (Net*& iter) {delete iter; iter = nullptr; });
    // std::for_each(instList.begin(), instList.end(), [] (Inst*& iter) {delete iter; iter = nullptr; });
    // std::for_each(cellList.begin(), cellList.end(), [] (Cell*& iter) {delete iter; iter = nullptr; });

    // free metal layers
    for (auto & metal : metalList) {
        delete metal;
        metal = nullptr;
    }    
    // free via layers
    for (auto & via : viaList) {
        delete via;
        via = nullptr;
    }    
    // free nets
    for (auto & net : netList) {
        delete net;
        net = nullptr;
    }      
    // free insts
    for (auto & inst : instList) {
        delete inst;
        inst = nullptr;
    }     
    // free cells
    for (auto & cell : cellList) {
        delete cell;
        cell = nullptr;
    }   
    // free route path
    delete [] finalPath;  
    // free layer id
    delete [] layerList;
    
}