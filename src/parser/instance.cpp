#include "instance.h"
#include "../util/config.h"
#include <iostream>

using std::vector;

//coordinate conversion and change pin_list 
void Inst::coorConversion(const int *instCoor, const std::string &direct, size_t indice) 
{
    Cell *tmp = cellList[indice];
    if (direct.find("N") == 0 || direct.find("S") == 0 || direct.find("FN") == 0 || direct.find("FS") == 0) {
        this->m_diarea[0] = instCoor[0] + tmp->m_cellSize[0];
        this->m_diarea[1] = instCoor[1] + tmp->m_cellSize[1];
    }
    else {
        this->m_diarea[0] = instCoor[0] + tmp->m_cellSize[1];
        this->m_diarea[1] = instCoor[1] + tmp->m_cellSize[0];
    }
    // update chip boundary
    DiareaX = (DiareaX < m_diarea[0] ? m_diarea[0] : DiareaX);
    DiareaY = (DiareaY < m_diarea[1] ? m_diarea[1] : DiareaY);    

    for (size_t i = 0; i < (tmp->m_pinList).size(); i++) {
        // initialize pin list
        Pin *instPin = new Pin(tmp->m_pinList[i]->m_pinName);
        this->m_pinList.push_back(instPin);

        for (size_t j = 0; j < (tmp->m_pinList[i]->m_portList).size(); j++) {
            vector<int> cellPort = tmp->m_pinList[i]->m_portList[j];

            vector<int> instPort(4, 0);
            if (direct.find("N") == 0) {
                instPort[0] = instCoor[0] + cellPort[0];
                instPort[1] = instCoor[1] + cellPort[1];
                instPort[2] = instCoor[0] + cellPort[2];
                instPort[3] = instCoor[1] + cellPort[3];
            }
            else if (direct.find("S") == 0) {
                instPort[0] = instCoor[0] + (tmp->m_cellSize[0] - cellPort[0]);
                instPort[1] = instCoor[1] + (tmp->m_cellSize[1] - cellPort[1]);
                instPort[2] = instCoor[0] + (tmp->m_cellSize[0] - cellPort[2]);
                instPort[3] = instCoor[1] + (tmp->m_cellSize[1] - cellPort[3]);    
            }
            else if (direct.find("W") == 0) {
                instPort[0] = instCoor[0] - cellPort[1];
                instPort[1] = instCoor[1] + cellPort[0];
                instPort[2] = instCoor[0] - cellPort[3];
                instPort[3] = instCoor[1] + cellPort[2];
            }
            else if (direct.find("E") == 0) {
                instPort[0] = instCoor[0] + cellPort[1];
                instPort[1] = instCoor[1] - cellPort[0];
                instPort[2] = instCoor[0] + cellPort[3];
                instPort[3] = instCoor[1] - cellPort[2];                   
            }
            else if (direct.find("FN") == 0) {
                instPort[0] = instCoor[0] + (tmp->m_cellSize[0] - cellPort[0]);
                instPort[1] = instCoor[1] + cellPort[1];
                instPort[2] = instCoor[0] + (tmp->m_cellSize[0] - cellPort[2]);
                instPort[3] = instCoor[1] + cellPort[3];                  
            }
            else if (direct.find("FS") == 0) {
                instPort[0] = instCoor[0] + cellPort[0];
                instPort[1] = instCoor[1] + (tmp->m_cellSize[1] - cellPort[1]);
                instPort[2] = instCoor[0] + cellPort[2];
                instPort[3] = instCoor[1] + (tmp->m_cellSize[1] - cellPort[3]);                
            }
            else if (direct.find("FW") == 0) {
                instPort[0] = instCoor[0] + cellPort[1];
                instPort[1] = instCoor[1] + cellPort[0];
                instPort[2] = instCoor[0] + cellPort[3];
                instPort[3] = instCoor[1] + cellPort[2];              
            }
            else if (direct.find("FE") == 0) {
                instPort[0] = instCoor[0] - cellPort[1];
                instPort[1] = instCoor[1] - cellPort[0];
                instPort[2] = instCoor[0] - cellPort[3];
                instPort[3] = instCoor[1] - cellPort[2];            
            }
            else {
                std::cout << "illegal inst direction!" << std::endl;
                exit(1);
            }
            instPin->m_portList.push_back(instPort);         
        }
    }
}
