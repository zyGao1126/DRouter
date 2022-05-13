#include "generator.h"
#include "../util/config.h"
#include "../util/utils.h"
#include <algorithm>

using std::string;
using std::vector;
using std::ifstream;
using std::ofstream;

void Generator::writeHeader(ofstream &resultFile) 
{
    resultFile << "NONDEFAULTRULES 1 ;" << '\n';
    resultFile << "- DEFAULT_METAL1_400" << '\n';

    for (size_t i = 0; i < metalList.size(); i++) {
        resultFile << "  + LAYER METAL" << i+1 << '\n';
        resultFile << "    WIDTH " << metalList[i]->m_minWidth;
        if (i == metalList.size() - 1)
            resultFile << " ;" << std::endl;
        else
            resultFile << '\n';
    }
}

bool Generator::noCmp(const Net *net1, const Net *net2) {
    return net1->m_netId < net2->m_netId;
}

void Generator::convertCoor(int gridX, int gridY, int *arrCoor, int layerId) 
{
    int gridWidth = metalList[layerId - 1]->m_minSpace; 
    arrCoor[0] = gridX * gridWidth - (gridWidth / 2);
    arrCoor[1] = gridY * gridWidth - (gridWidth / 2);
}

void Generator::addVia(ofstream &resultFile, const int layerId, const int netId) 
{
    int arr[2];
    for (int i = 1; i < layerId; i++)
        for (size_t j = 0; j < netList[netId - 1]->m_nodeList.size(); j++) {
            arr[0] = netList[netId - 1]->m_nodeList[j][0];
            arr[1] = netList[netId - 1]->m_nodeList[j][1];
            if (i == 1 && j == 0) 
                resultFile << " + ROUTED METAL" << i << " ( " << arr[0] << " " << arr[1] << " )" << " via" << i << '\n';
            else
                resultFile << "   NEW METAL" << i << " ( " << arr[0] << " " << arr[1] << " )" << " via" << i << '\n';
        }
}

void Generator::addiniNode(ofstream &resultFile, const int layerId, const int netId)
{
    int arr[2];
    for (int i = 2; i < layerId; i++) 
        for (size_t j = 0; j < netList[netId - 1]->m_nodeList.size(); j++) {
            arr[0] = netList[netId - 1]->m_nodeList[j][0];
            arr[1] = netList[netId - 1]->m_nodeList[j][1];
            resultFile << "   NEW METAL" << i << " ( " << arr[0] << " " << arr[1] << " )";
            resultFile << " ( " << "*" << " " << arr[1] << " )" << '\n';
        }
}

void Generator::addPath(ofstream &resultFile, const int layerId, const int netId) 
{
    int startX = finalPath[netId - 1][0][0];
    int startY = finalPath[netId - 1][0][1];
    int curX = finalPath[netId - 1][1][0];
    int curY = finalPath[netId - 1][1][1];
    // horizontal: 0; vertical: 1; default: -1
    int direct = -1;

    int tmpCoor[2];
    convertCoor(curX, curY, tmpCoor, layerId);
    resultFile << " ( " << tmpCoor[0] << " " << tmpCoor[1] << " )";

    for (size_t i = 2; i < finalPath[netId - 1].size(); i++) {
        int nextX = finalPath[netId - 1][i][0];
        int nextY = finalPath[netId - 1][i][1];
        
        if (direct == -1) {
            direct = (abs(nextX - curX) == 1 ? 0 : direct);
            direct = (abs(nextY - curY) == 1 ? 1 : direct); 
        }
        // vertival
        if (abs(nextX - curX) == 1 && direct == 1) {
            direct = 0;
            convertCoor(curX, curY, tmpCoor, layerId);
            resultFile << " ( " << "*" << " " << tmpCoor[1] << " )" << '\n';
            resultFile << "   NEW METAL" << layerId << " ( " << tmpCoor[0] << " " << tmpCoor[1] << " )";
        }
        // horizontal
        if (abs(nextY - curY) == 1 && direct == 0) {
            direct = 1;
            convertCoor(curX, curY, tmpCoor, layerId);
            resultFile << " ( " << tmpCoor[0] << " " << "*" << " )" << '\n';
            resultFile << "   NEW METAL" << layerId << " ( " << tmpCoor[0] << " " << tmpCoor[1] << " )";            
        }
        if (nextX == startX && nextY == startY) {
            if (startX == curX) {
                convertCoor(nextX, nextY, tmpCoor, layerId);
                resultFile << " ( " << "*" << " " << tmpCoor[1] << " )" << '\n';
            }
            if (startY == curY) {
                convertCoor(nextX, nextY, tmpCoor, layerId);
                resultFile << " ( " << tmpCoor[0] << " " << "*" << " )" << '\n';
            }
            if (i == finalPath[netId - 1].size() - 1) {
                resultFile << ";" << '\n';
                break;
            }
            else {
                startX = finalPath[netId - 1][i + 1][0];
                startY = finalPath[netId - 1][i + 1][1];
                curX = finalPath[netId - 1][i + 2][0];
                curY = finalPath[netId - 1][i + 2][1];
                nextX = finalPath[netId - 1][i + 2][0];
                nextY = finalPath[netId - 1][i + 2][1];
                direct = -1;
                i += 2;
                convertCoor(curX, curY, tmpCoor, layerId);
                resultFile << "   NEW METAL" << layerId << " ( " << tmpCoor[0] << " " << tmpCoor[1] << " )";
            }            
        }
        curX = nextX;
        curY = nextY;
    }
}

void Generator::writePathToDef(ofstream &resultFile, int netId) 
{
    if (finalPath[netId - 1].size() == 0) {
        resultFile << ";" << '\n';
        return;
    }
    int layerId = layerList[netId - 1];
    // change net list as net-id order
    sort(netList.begin(), netList.end(), this->noCmp);

    if (layerId == 1) {
        resultFile << " + ROUTED METAL" << layerId;
    }
    if (layerId > 1) {
        addVia(resultFile, layerId, netId);
        addiniNode(resultFile, layerId, netId);
        resultFile << "   NEW METAL" << layerId;
    }
    addPath(resultFile, layerId, netId);
}

void writeReusltToDef() 
{
    ifstream defFile(defFileName);
    ofstream resultFile(resultFileName);
    string line;
    Generator generator;

    while (!defFile.eof()) {
        getline(defFile, line);
        if (line.find("DESIGN") == 0) {
            resultFile << "DESIGN result ;" << '\n';
            continue;
        }
        if (line.find("COMPONENTS") == 0) {
            generator.writeHeader(resultFile);
        }
        resultFile << line << '\n';
        if (line.find("NET") == 0) 
            break;
    }

    for (int i = 0; i < netCount; i++) {
        getline(defFile, line, ';');
        resultFile << line;
        generator.writePathToDef(resultFile , i + 1);
    }
    while (!defFile.eof()) {
        getline(defFile, line);
        resultFile << line << '\n';
    }

    defFile.close();
    resultFile.close();
}