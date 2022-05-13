#ifndef __LAYER__
#define __LAYER__

#include <iostream>

typedef struct sMetalLayer 
{
public:
    enum layerDirect {HORIZONTAL = 0, VERTICAL = 1};
public:
    int m_id;
    layerDirect m_direct;
    int m_minWidth;
    int m_minSpace;
    int m_minArea;
} MetalLayer;

typedef struct sViaLayer 
{
    int m_viaId;
    int m_layerId;
    int m_coor[4];
} ViaLayer; 

#endif