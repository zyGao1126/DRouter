#ifndef __NET__
#define __NET__

#include <vector>
#include <string>

class Net 
{
public:
    int m_netId;
    std::vector<std::vector<int>> m_nodeList;

public:
    Net(int id): m_netId(id), m_nodeList(std::vector<std::vector<int>> ()) {}
    void getConnectedNode(const int instId, const std::string& pinName);
    void convertToGrid(std::vector<std::vector<int>> &portVec, int layerId);

private:
    void findBestPort(const std::vector<std::vector<int>> &portList);
};

#endif
