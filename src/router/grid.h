#ifndef __GRIDLAYER__
#define __GRIDLAYER__

#include <string>

class GridLayerBox {
public:
    int m_layerId;
    int m_gridSpacing;
    int m_gridWidth; 
    int m_gridHeight;
    int **gridBox;

public:
    GridLayerBox(): m_layerId(0), m_gridSpacing(0), m_gridWidth(0), m_gridHeight(0), gridBox(nullptr) {}
    GridLayerBox(int layerId, int spacing, int width, int height);
    ~GridLayerBox();
    void initGridBox();

private:
    void addObstacle(int startX, int startY, int endX, int endY, const std::string& pinName);
    void addSpace(int x, int y);
    void showGrid();
};


#endif