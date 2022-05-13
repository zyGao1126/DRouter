#include "grid.h"
#include "../util/config.h"
#include "../util/utils.h"
#include <iostream>
#include <fstream>

using std::vector;
using std::string;

GridLayerBox::GridLayerBox(int layerId, int spacing, int width, int height) 
{
    this->m_layerId = layerId;
    this->m_gridSpacing = spacing;
    this->m_gridWidth = width;
    this->m_gridHeight = height;           

    gridBox = new int* [m_gridWidth];
    for (int i = 0; i < m_gridWidth; i++) {
        // initialize value 0
        gridBox[i] = new int[m_gridHeight] ();
    }
}

GridLayerBox::~GridLayerBox() 
{
    for (int i = 0; i < m_gridWidth; i++) 
        delete [] gridBox[i];
    delete [] gridBox;    
}

//init grid information according to port, obstacle
void GridLayerBox::initGridBox() 
{
    if (this->m_layerId != 1) {
        showGrid();
        return;
    }
        
    int startGridX, startGridY, endGridX, endGridY;
    for (size_t i = 0; i < instList.size(); i++) 
        for (size_t j = 0; j < instList[i]->m_pinList.size(); j++) 
            for (size_t k = 0; k < instList[i]->m_pinList[j]->m_portList.size(); k++) {
                vector<int> tmp = instList[i]->m_pinList[j]->m_portList[k];
                startGridX = tmp[0] / m_gridSpacing;
                startGridY = tmp[1] / m_gridSpacing;
                endGridX = tmp[2] / m_gridSpacing;
                endGridY = tmp[3] / m_gridSpacing;
                addObstacle(startGridX, startGridY, endGridX, endGridY, instList[i]->m_pinList[j]->m_pinName);
            }

    showGrid();
}

void GridLayerBox::addSpace(int x, int y) 
{
    if (y > 0)                                          
        gridBox[x][y - 1] = (gridBox[x][y - 1] == 0 ? SPACEFLAG : gridBox[x][y - 1]);
    if (x > 0)                                          
        gridBox[x - 1][y] = (gridBox[x - 1][y] == 0 ? SPACEFLAG : gridBox[x - 1][y]);               
    if (y < m_gridHeight - 1)                           
        gridBox[x][y + 1] = (gridBox[x][y + 1] == 0 ? SPACEFLAG : gridBox[x][y + 1]);                
    if (x < m_gridWidth - 1)                            
        gridBox[x + 1][y] = (gridBox[x + 1][y] == 0 ? SPACEFLAG : gridBox[x + 1][y]);                    
    if (x > 0 && y > 0)                                 
        gridBox[x - 1][y - 1] = (gridBox[x - 1][y - 1] == 0 ? SPACEFLAG : gridBox[x - 1][y - 1]);
    if (x > 0 && y < m_gridHeight - 1)                  
        gridBox[x - 1][y + 1] = (gridBox[ x- 1][y + 1] == 0 ? SPACEFLAG : gridBox[x - 1][y + 1]);                 
    if (x < m_gridWidth - 1 && y > 0)                   
        gridBox[x + 1][y - 1] = (gridBox[x + 1][y - 1] == 0 ? SPACEFLAG : gridBox[x + 1][y - 1]);
    if (x < m_gridWidth - 1 && y < m_gridHeight - 1)    
        gridBox[x + 1][y + 1] = (gridBox[x + 1][y + 1] == 0 ? SPACEFLAG : gridBox[x + 1][y + 1]);     
}

void GridLayerBox::addObstacle(int startX, int startY, int endX, int endY, const string& pinName) 
{
    if (startX > endX)
        std::swap(startX, endX);
    if (startY > endY)
        std::swap(startY, endY);

    for (int x = startX; x <= endX; x++)
        for (int y = startY; y <= endY; y++) {
            if (pinName == "OBS")
                gridBox[x][y] = OBSFLAG;
            else {
                gridBox[x][y] = PINFLAG;
                addSpace(x, y);
            }
    }
}

void GridLayerBox::showGrid() 
{
    int gridCount = 0;
    int obsCount = 0;
    int pinCount = 0;
    int spaceCount = 0;
    for (int i = 0; i < m_gridWidth; i++)
        for (int j = 0; j < m_gridHeight; j++) {
            gridCount++;
            if (gridBox[i][j] == SPACEFLAG)
                spaceCount++;
            else if (gridBox[i][j] == OBSFLAG)
                obsCount++;
            else if (gridBox[i][j] == PINFLAG)
                pinCount++;
        }
    
    log() << std::endl;
    log() << "Grid Layer: " << this->m_layerId << std::endl;
    log() << "Width: " << this->m_gridWidth << std::endl;
    log() << "Height: " << this->m_gridHeight << std::endl;
    log() << "Total Grid Amount: " << gridCount << std::endl;
    log() << "Total Obstacle Amount: " << obsCount << std::endl;
    log() << "Total Pin Amount: " << pinCount << std::endl;
    log() << "Total Pin-spacing Amount: " << spaceCount << std::endl;
    log() << std::endl;

#ifdef WRITEOUT
    std::ofstream OutFile("./result/gridBox_result.txt");
    for (int s = 0;s < this->m_gridWidth; s++) {
        for (int t = 0; t < this->m_gridHeight; t++) {
            OutFile << this->gridBox[s][t] << '\t';
        }
        OutFile << std::endl;
    }
    OutFile.close();    
#endif //WRITEOUT
}