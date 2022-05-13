#include "net.h"
#include "../Util/config.h"
#include <limits.h>
#include <assert.h>

using std::vector;
using std::string;

//a method to find the most suitable port
void Net::findBestPort(const vector<vector<int>> &portList) 
{
    int index;
    int coorX, coorY, gridX, gridY, manhDis;
    int minDis = INT_MAX;

    for (size_t i = 0; i < portList.size(); i++) {
        coorX = (portList[i][0] + portList[i][2]) / 2;
        coorY = (portList[i][1] + portList[i][3]) / 2;
        gridX = (int)(coorX / metalList[0]->m_minSpace) * metalList[0]->m_minSpace + (metalList[0]->m_minSpace) / 2; 
        gridY = (int)(coorY / metalList[0]->m_minSpace) * metalList[0]->m_minSpace + (metalList[0]->m_minSpace) / 2; 
        manhDis = abs(coorX - gridX) + abs(coorY - gridY);
        if (manhDis < minDis) {
            index = i;
            minDis = manhDis;
        }
    }

    coorX = (portList[index][0] + portList[index][2]) / 2;
    coorY = (portList[index][1] + portList[index][3]) / 2;
    vector<int> bestPort = {coorX, coorY};
    m_nodeList.push_back(bestPort);
}

//parse the connection of each net and return the coordinates of the node
void Net::getConnectedNode(const int instId, const string& pinName) 
{
    size_t indice;
    for (indice = 0; indice < instList.size(); indice++) {
        if (instId == instList[indice]->m_id) 
            break;
    }
    for (size_t j = 0; j < instList[indice]->m_pinList.size(); j++) {
        if (pinName == instList[indice]->m_pinList[j]->m_pinName) {
            findBestPort(instList[indice]->m_pinList[j]->m_portList);
            break;
        }
    }
}

// convert coordinate into gridBox indice
void Net::convertToGrid(vector<vector<int>> &portVec, int layerId) 
{
    int spacing;
    for (size_t i = 0; i < metalList.size(); i++) {
        if (layerId == metalList[i]->m_id) {
            spacing = metalList[i]->m_minSpace;
            break;
        }
    }
    for (size_t i = 0; i < portVec.size(); i++) {
        assert(portVec[i].size() == 2);
        portVec[i][0] = (portVec[i][0] / spacing) + 1;
        portVec[i][1] = (portVec[i][1] / spacing) + 1;
    }      
}