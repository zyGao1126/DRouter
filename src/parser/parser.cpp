#include "parser.h"
#include "../util/config.h"
#include "../util/utils.h"
#include "layer.h"
#include "instance.h"
#include "net.h"
#include "../mempool/mempool.hpp"
#include <getopt.h>

using std::string;
using std::vector;
using std::ifstream;
using std::ofstream;

void Parser::split(const string& s, vector<string>& v, const string& c)
{
    string::size_type pos1, pos2;
    size_t len = s.length();
    pos2 = s.find(c);
    if (s[0]=='\n') 
        pos1 = 1;
    else 
        pos1 = 0;

    while(string::npos != pos2) {
        if (pos2 == pos1 || s.substr(pos1, pos2 - pos1) == " ") {}
        else
            v.emplace_back(s.substr(pos1, pos2 - pos1));
        pos1 = pos2 + c.size();
        pos2 = s.find(c, pos1);
    }
    if(pos1 != len)
        v.emplace_back(s.substr(pos1));
}


void Parser::parseMetalInfo(ifstream &lefFile, string &line, vector<string> &strList) 
{
    MetalLayer *metalLayer = metalMemPool.alloc();
    metalLayer->m_id = stoi(strList[1].substr(5, strList[1].length() - 5));

    for (int i = 0; i < lefMetalLine; i++) {
        getline(lefFile, line, ';');
        strList.clear();
        this->split(line, strList, " ");

        if (strList[0] == "DIRECTION") {
            if (strList[1] == "HORIZONTAL")
                metalLayer->m_direct = metalLayer->layerDirect::HORIZONTAL;
            else if (strList[1] == "VERTICAL")
                metalLayer->m_direct = metalLayer->layerDirect::VERTICAL;     
        }
        else if (strList[0] == "MINWIDTH") {
            metalLayer->m_minWidth = (int)(stod(strList[1]) * MICRONS);
        }
        else if (strList[0] == "SPACING") {
            metalLayer->m_minSpace = (int)(stod(strList[1]) * MICRONS); 
        }
        else if (strList[0] == "AREA") {
            metalLayer->m_minArea = (int)(stod(strList[1]) * MICRONS);
        }
        else
            continue;
    }
    metalList.push_back(metalLayer);
}

void Parser::parseViaInfo(ifstream &lefFile, string &line, vector<string> &strList) 
{
    ViaLayer *viaLayer = viaMemPool.alloc();
    viaLayer->m_viaId = stoi(strList[1].substr(3, 1));

    for (int i = 0; i < lefViaLine; i++) {
        getline(lefFile, line, ';');
        strList.clear();
        this->split(line, strList, " ");
        if (strList[0] == "LAYER") {
            if (strList[1].find("VIA") == 0)
                viaLayer->m_layerId = stoi(strList[1].substr(3, 1));
            else if (strList[1].find("METAL") == 0)
                viaLayer->m_layerId = stoi(strList[1].substr(5, 1));
        }
        if (strList[0] == "RECT") {
            for (size_t j = 0; j < 4; j++)
                viaLayer->m_coor[j] = (int)(stod(strList[j + 1]) * MICRONS);
        }
    }
    viaList.push_back(viaLayer);
}

void Parser::parseCellInfo(ifstream &lefFile, string &line) 
{
    vector<string> strList;
    this->split(line, strList, " ");
    Cell *cell = cellMemPool.alloc();
    cell->m_cellName = strList[1];

    while (!lefFile.eof()) {
        getline(lefFile, line);
        if (line.find("END") == 0)
            break;
        else if (line.find("SIZE") != string::npos) {
            strList.clear();
            split(line, strList, " "); 
            cell->m_cellSize[0] = (int)(stod(strList[1]) * MICRONS);
            cell->m_cellSize[1] = (int)(stod(strList[3]) * MICRONS);
        }
        else if (line.find("PIN") != string::npos || line.find("OBS") != string::npos) {
            Pin *pin = new Pin();
            parsePinInfo(lefFile, line, pin);
            cell->m_pinList.push_back(pin);
        }
        else 
            continue;
    }
    cellList.push_back(cell);
}

void Parser::parsePinInfo(ifstream &lefFile, string &line, Pin *pin) 
{
    vector<string> strList;
    this->split(line, strList, " ");
    pin->m_pinName = strList[1];

    while (true) {
        getline(lefFile, line);
        if (line.find("END") != string::npos)
            break;
        else if (line.find("RECT") != string::npos && line.find("DIRECTION") == string::npos) {
            vector<int> rectCoor;
            strList.clear();
            this->split(line, strList, " ");
            for (int i = 0; i < 4; i++) 
                rectCoor.push_back(stod(strList[i + 1]) * MICRONS);
            pin->m_portList.push_back(rectCoor);
        }
        else 
            continue;
    }
}

void Parser::parseInstInfo(std::ifstream &defFile) 
{
    vector<string> strList;
    string instLine;
    getline(defFile, instLine, ';');
    split(instLine, strList, " ");
    int num = stoi(strList[1].substr(4));

    Inst *inst = instMemPool.alloc();
    inst->m_id = num;
    string cellName = strList[2];
    int instCoors[2] = {stoi(strList[6]), stoi(strList[7])};
    string direct = strList[9];
    size_t indice;

    for (indice = 0; indice < cellList.size(); indice++) {
        if (cellName == cellList[indice]->m_cellName) 
            break;
    }
    inst->m_cellName = cellName;  
    inst->coorConversion(instCoors, direct, indice);
    instList.push_back(inst);
}

void Parser::parseNetInfo(ifstream &defFile) 
{
    vector<string> strList;    
    string netLine;
    getline(defFile, netLine, ';');
    this->split(netLine, strList, " ");
    int netNum = stoi(strList[1].substr(3, strList[1].length() - 3));
    Net *net = netMemPool.alloc();
    net->m_netId = netNum;

    for (size_t j = 0; j < strList.size(); j++) {
        if (strList[j] == "(") {
            // ignore net with pin
            if (strList[j+1].find("PIN") == 0) {
                netList.push_back(net);
                return;
            }
            int instId = stoi(strList[j + 1].substr(4, strList[j + 1].length() - 4));
            string pinName = strList[j + 2];        
            net->getConnectedNode(instId, pinName);
        }
    }
    netList.push_back(net);
}

void Parser::parseLefFile(ifstream &lefFile) 
{
    string line;
    vector<string> strList;
    // initialize 10 metal layer
    metalMemPool.create(10);
    // initialize 10 via layer
    viaMemPool.create(10);
    // initialize 10 via layer
    cellMemPool.create(5);    

    while (!lefFile.eof()) {
        getline(lefFile, line);
        if (line.find("LAYER") == 0) {
            strList.clear();
            this->split(line, strList, " ");
            if (strList[1].find("METAL") == 0) {
                parseMetalInfo(lefFile, line, strList);
            }
            else 
                continue;
        }
        else if (line.find("VIA") == 0 && line.find("VIARULE") != 0) {
            strList.clear();
            this->split(line, strList, " ");            
            char lastChar =strList[1].back();
            if (isdigit(lastChar) != 0)
                parseViaInfo(lefFile, line, strList);
            else
                continue;
        }
        else if (line.find("MACRO") == 0) {
            parseCellInfo(lefFile, line);
        }
    }

    if (cellList.empty())
        log() << "PARSER WARNING: there are no cell to parse." << std::endl;
    if (viaList.empty())
        log() << "PARSER WARNING: there are no via layer to parse." << std::endl;
    if (metalList.empty())
        log() << "PARSER WARNING: there are no metal layer to parse." << std::endl;  
}


void Parser::parseDefFile(ifstream &defFile) 
{
    string line;
    while (!defFile.eof()) {
        getline(defFile, line);
        if (line.find("COMPONENTS") == 0)
            break;
    }
    int endLine = line.find(';'); 
    int compNum = stoi(line.substr(11, endLine - 11));
    // initialize instance mempool
    instMemPool.create(compNum);
    for (int i = 0; i < compNum; i++) {
        parseInstInfo(defFile);
    }
    while (!defFile.eof()) {
        getline(defFile, line);
        if (line.find("NETS") == 0)
            break;
    }
    endLine = line.find(';');
    // initial global net count
    netCount = stoi(line.substr(5, endLine - 5));
    // initialize net mempool
    netMemPool.create(netCount);    
    // initial global layer list
    layerList = new int[netCount] ();   
    // initial global final path
    finalPath = new vector<vector<int>> [netCount];     
    for (int i = 0; i < netCount; i++) {
        this->parseNetInfo(defFile);
    }   
    defFile.close();
}

void Parser::show() 
{
    log() << "Total MetalLayer Count: " << metalList.size() << std::endl;
    for (size_t i = 0; i < metalList.size(); i++) {
        log() << '\t' << "layer" << metalList[i]->m_id << ": " << "minwidth:" << metalList[i]->m_minWidth << '\t' \
              << "minspacing:" << metalList[i]->m_minSpace << '\t' << "minarea:" << metalList[i]->m_minArea << std::endl;
    }

    log() << "Total ViaLayer Count: " << viaList.size() << std::endl;
    for (size_t i = 0; i < viaList.size(); i++) {
        log() << '\t' << "via" << viaList[i]->m_viaId << " rectangle: " << '\t';
        std::cout << '(' << viaList[i]->m_coor[0] << ", " << viaList[i]->m_coor[1] << "), " \
              << '(' << viaList[i]->m_coor[2] << ", " << viaList[i]->m_coor[3] << ')' << std::endl;
    }

    log() << "Total Cell Count: " << cellList.size() << std::endl;
    for (size_t i = 0; i < cellList.size(); i++) {
        log() << '\t' << cellList[i]->m_cellName << ": " << "coordinate:(" << cellList[i]->m_cellSize[0] \
              << ", " << cellList[i]->m_cellSize[1] << ")" << std::endl;
    }

    log() << "Total Diarea: " << "(" << DiareaX << ", " << DiareaY << ")" << std::endl;
    log() << "Total Inst Count: " << instList.size() << std::endl;
    log() << "Total Net Count: " << netList.size() << std::endl;
}

void parseInputSpec(int argc, char *argv[]) 
{
    int opt;
    static struct option longOptions[] = {
        {"lef", required_argument, 0, 'l'},
        {"def", required_argument, 0, 'd'},
        {"result", required_argument, 0, 'r'},
    };
    int optionIndex = 0;
    Timer parserTimer;
    Parser parser;

    log() << std::endl;
    log() << "------ LAUNCH PARSER ------" << std::endl;
    log() << std::endl;
    
    while ((opt = getopt_long_only(argc, argv, "", longOptions, &optionIndex)) != -1) {
        string fileName = optarg; 
        switch (opt)
        {
            case 'l':
            {
                lefFileName = fileName;
                std::ifstream lefFile(fileName);
                if (!lefFile) {
                    printlog("open lefFile fail");
                    exit(1);
                }
                else {
                    parser.parseLefFile(lefFile);
                    break;
                }
            }
            case 'd':
            {
                defFileName = fileName;
                std::ifstream defFile(fileName);
                if (!defFile) {
                    printlog("open defFile fail");
                    exit(1);
                }
                else {
                    parser.parseDefFile(defFile);                
                    break;
                }
            }   
            case 'r':
            {
                resultFileName = fileName;
                break;
            }
            default:
                printlog("illegal input format, please inter correct files");
                exit(1);
        }
    }

    parser.show();
    log() << std::endl;
    log() << "------ FINISH PARSER ------" << std::endl;
}