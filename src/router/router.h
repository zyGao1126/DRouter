#ifndef __ROUTER__
#define __ROUTER__

#include "../parser/net.h"
#include "../util/config.h"
#include "../util/utils.h"
#include "grid.h"
#include <list>
#include <cmath>

struct netState 
{
    std::vector<int> netRouted; //store the id of wired net
    std::vector<int> netUnroute; //store th id of unwired net due to resource limitation     
};

struct Point 
{
public:
    int x, y;
    int F, G, H;
    Point *parent;
    Point() {}
    Point(int _x, int _y): x(_x), y(_y), F(0), G(0), H(0), parent(nullptr) {}
    ~Point() {}
};

class AStar 
{
public:
    AStar(): m_openList(), m_closeList() {}
    ~AStar();
    void InitAstar(std::vector<std::vector<int>> &maze);
    Point* GetPath(Point &startPoint, Point &endPoint, GridLayerBox *gridLayer, const int netId);

private:
    Point *getLeastFpoint();
    bool IsPointLegal(int nextX, int nextY, int curX, int curY, GridLayerBox *gridLayer, int netId);
    Point *isInList(const std::list<Point *> &list, const int x, const int y) const; 
    Point *isInList(const std::list<Point *> &list, const Point* point) const; 
    int CornerCost(Point* nextP, int endX, int endY);

private:
    inline int calcH(const Point *point, const Point *end) {
        return (abs(point->x - end->x) + abs(point->y - end->y)) * 10;
    }
    inline int calcG(const Point *nextP, int offsetX, int offsetY) {
        int parentG = nextP->parent == nullptr ? 0 : nextP->parent->G;
        int extraG = sqrt(pow(offsetX, 2) + pow(offsetY, 2)) * 10; //constant
        return parentG + extraG;
    }    

private:
	std::list<Point *> m_openList;  
	std::list<Point *> m_closeList;
};

class Router 
{
public:
    void runRoutingFlow();

private:
    void routeNets(GridLayerBox *gridLayer, std::vector<int>& netToRoute);
    void findPath(GridLayerBox *gridLayer, struct netState* netste, const std::vector<int> &netToRoute, 
                  const std::vector<std::vector<std::vector<int>>> &nodeList);
    // void multiFindPath(GridLayerBox *gridLayer, struct netState* netste, const std::vector<int> &netToRoute, 
    //                    const std::vector<std::vector<std::vector<int>>> &nodeList);    
    bool astarRoute(GridLayerBox *gridLayer, const std::vector<std::vector<int>> &nodeList, std::vector<std::vector<int>> &path, const int netId);
    bool singleNetRoute(int startX, int startY, int endX, int endY, GridLayerBox *gridLayer, 
                        std::vector<std::vector<int>> &pathList, const int netId);    
    void updateGridBox(int idx, int x, int y, GridLayerBox *gridLayer);
    void routeOnGridBox(int idx, int x, int y, GridLayerBox *gridLayer);
    bool checkLegality(GridLayerBox *gridLayer, const int netId);
    static bool disCmp(const Net* a, const Net* b);

private:
    inline int calManhDis(int x0, int y0, int x1, int y1) {
        return (abs(x0 - x1) + abs(y0 - y1)) * 10;
    }
    inline void signGrid1(int **grid, int idx, int x, int y) {
        grid[x][y] = (grid[x][y] == 0 || grid[x][y] == SPACEFLAG) ? -idx : grid[x][y];
    }
    inline void signGrid2(int **grid, int idx, int x, int y) {
        grid[x][y] = (grid[x][y] <= 0) ? SPACEFLAG : grid[x][y];
    }
    inline bool checkGrid1(int **grid, int x, int y) {
        return (grid[x][y] > 0 || grid[x][y + 1] > 0 || grid[x + 1][y] > 0 || grid[x + 1][y + 1] > 0);
    }
    inline bool checkGrid2(int **grid, int x, int y) {
        return (grid[x][y] > 0 || grid[x][y - 1] > 0 || grid[x][y + 1] > 0 || grid[x + 1][y - 1] > 0 || grid[x + 1][y] > 0 || grid[x + 1][y + 1] > 0);
    }    
    inline bool checkGrid3(int **grid, int x, int y) {
        return (grid[x][y] > 0 || grid[x - 1][y] > 0 || grid[x][y + 1] > 0 || grid[x - 1][y + 1] > 0 || grid[x + 1][y] > 0 || grid[x + 1][y + 1] > 0);
    }    
    inline bool checkGrid4(int **grid, int x, int y) {
        return (grid[x][y] > 0 || grid[x - 1][y - 1] > 0 || grid[x - 1][y] > 0 || grid[x - 1][y + 1] > 0 || 
                grid[x][y - 1] > 0 || grid[x][y + 1] > 0 || grid[x + 1][y - 1] > 0 || grid[x + 1][y] > 0 || grid[x + 1][y + 1] > 0);
    }     
};


#endif