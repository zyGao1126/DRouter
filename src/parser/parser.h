#ifndef __PARSER__
#define __PARSER__
#include <iostream>
#include <fstream>
#include <cstring>
#include <vector>
#include "instance.h"

// parse the input lef/def file
// write routing result to result.def
class Parser 
{
public:
    Parser() {}
    void parseLefFile(std::ifstream &lefFile);
    void parseDefFile(std::ifstream &defFile);
    void show();

private:
    void split(const std::string& s, std::vector<std::string>& v, const std::string& c);
    void parseMetalInfo(std::ifstream &lefFile, std::string &line, std::vector<std::string> &strList);
    void parseViaInfo(std::ifstream &lefFile, std::string &line, std::vector<std::string> &strList);
    void parseCellInfo(std::ifstream &lefFile, std::string &line);
    void parsePinInfo(std::ifstream &lefFile, std::string &line, Pin *pin);
    void parseInstInfo(std::ifstream &defFile);
    void parseNetInfo(std::ifstream &defFile);
};

void parseInputSpec(int argc, char *argv[]);
void writeReusltToDef();

#endif