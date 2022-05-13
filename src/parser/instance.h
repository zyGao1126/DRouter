#ifndef __INSTANCE__
#define __INSTANCE__

#include <vector>
#include <string>
#include <iostream>

class Pin 
{
public:
    std::string m_pinName;
    std::vector<std::vector<int>> m_portList;
public:
    Pin() {}
    Pin(std::string name): m_pinName(name) {}
};

class Cell 
{
public:
    std::string m_cellName;
    int m_cellSize[2];
    std::vector<Pin *> m_pinList;

public:
    Cell(): m_pinList(std::vector<Pin *>()) {}
    virtual ~Cell() {
        for (auto &p : m_pinList) {
            delete p;
            p = nullptr;
        }
    } 
};

class Inst: public Cell 
{
public:
    int m_id;
    int m_diarea[2];
public:
    Inst() {}
    Inst(int id): m_id(id) {};
    ~Inst() {}     
    void coorConversion(const int *instCoor, const std::string &direct, size_t indice);    
};

#endif