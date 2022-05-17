#include "router.h"
#include <assert.h>
#include <algorithm>
// #include <thread>

using std::vector;
using std::list;

AStar::~AStar() 
{
    list<Point *>::iterator ite;
    for (ite = m_openList.begin(); ite != m_openList.end(); ++ite) {
        Point *tmp = *ite;
        delete tmp;
        tmp = nullptr;
    }
    for (ite = m_closeList.begin(); ite != m_closeList.end(); ++ite) {
        Point *tmp = *ite;
        delete tmp;
        tmp = nullptr;
    }
    m_openList.clear();
    m_closeList.clear();
}

Point* AStar::getLeastFpoint() {
	if (!m_openList.empty()) {
		auto resPoint = m_openList.front();
		for (auto &point : m_openList) {
		    if (point->F < resPoint->F)
			    resPoint = point;
        }
        return resPoint;
	}
	return nullptr;
}

bool AStar::IsPointLegal(int nextX, int nextY, int curX, int curY, GridLayerBox *gridLayer, int netId) 
{

    int **grid = gridLayer->gridBox;
    // determinate boundary
    if (nextX < 0 || nextX >= gridLayer->m_gridWidth || nextY < 0 || nextY > gridLayer->m_gridHeight)
        return false;
    // determinate obstacle
    if (grid[nextX][nextY] != -netId && grid[nextX][nextY] != 0)
        return false;
    // two nodes are diagonal and their common adjacent nodes are obstacles
    if (nextX != curX && nextY != curY && ((grid[nextX][curY] != -netId && grid[nextX][curY] != 0) || (grid[curX][nextY] != -netId && grid[curX][nextY] != 0)))
        return false;

    return true;
}

Point* AStar::isInList(const std::list<Point *> &pList, const int x, const int y) const {
    for (auto p : pList) {
	    if (p->x == x && p->y == y)
		    return p;
    }
    return nullptr;
}

Point* AStar::isInList(const std::list<Point *> &pList, const Point *point) const {
    for (auto p : pList) {
	    if (p->x == point->x && p->y == point->y)
		    return p;
    }
    return nullptr;
}

// add constraint of inflection point
int AStar::CornerCost(Point* nextP, int endX, int endY) 
{
    if (nextP->parent == nullptr)
        return 0;
    if (nextP->x == nextP->parent->x || nextP->y == nextP->parent->y)
        return 0;
    else if (nextP->x == endX || nextP->y == endY)
        return 1;
    else
        return 2;
}

Point* AStar::GetPath(Point &startP, Point &endP, GridLayerBox *gridLayer, const int netId) 
{
    if (startP.x == endP.x && startP.y == endP.y)
        return &endP;
    // start point
    Point *p = new Point(startP.x, startP.y);
    m_openList.push_back(p);
    // correspond to top, left, right, down
    int offset[4][2] = {{-1,0}, {0,-1}, {0,1}, {1,0}}; 
    
    while (!m_openList.empty()) {
        Point* curP = getLeastFpoint();
        m_openList.remove(curP);
        m_closeList.push_back(curP);
        // since wire can only be vertical, only 4 points(top, down, left ,right) can be chosen
        for (int i = 0; i < 4; i++) {
            int nextX = curP->x + offset[i][0];
            int nextY = curP->y + offset[i][1];
            // is legal and not in close list
            if (IsPointLegal(nextX, nextY, curP->x, curP->y, gridLayer, netId) && !isInList(m_closeList, nextX, nextY)) {
                Point *nextP = new Point(nextX, nextY);
                int G = calcG(curP, offset[i][0], offset[i][1]);
                int H = calcH(nextP, &endP);
                int Extra = CornerCost(nextP, endP.x, endP.y);

                Point *p = isInList(m_openList, nextP);
                // nextP is not in inList, thus add to inList
                if (!p) {
                    m_openList.push_back(nextP);
                    nextP->parent = curP;
                    nextP->G = G;
                    nextP->H = H;
                    nextP->F = nextP->G + nextP->H + Extra;             
                    nextP = nullptr;     
                }
                // inList has a Point which is same as nextP, update p and delete nextP 
                else {
                    int tmpF = G + H + Extra;
                    // find better point, update it 
                    if (tmpF < p->F) {
                        p->parent = curP;
                        p->F = tmpF;
                    }
                    delete nextP;
                }
            }
            Point *resP = isInList(m_openList, &endP);
            if (resP)
                return resP;
        }
    }
    return nullptr;
}

bool Router::disCmp(const Net *a, const Net *b) 
{
    int dis1 = 0;
    int dis2 = 0;  
 
    for (size_t i = 1; i < a->m_nodeList.size(); i++) 
        dis1 += (abs(a->m_nodeList[i][0] - a->m_nodeList[i - 1][0]) + abs(a->m_nodeList[i][1] - a->m_nodeList[i - 1][1]));
    for (size_t i = 1; i < b->m_nodeList.size(); i++)
        dis2 += (abs(b->m_nodeList[i][0] - b->m_nodeList[i - 1][0]) + abs(b->m_nodeList[i][1] - b->m_nodeList[i - 1][1]));
    
    return (dis1 < dis2); 
}

void Router::updateGridBox(int idx, int x, int y, GridLayerBox *gridLayer) 
{
    int **grid = gridLayer->gridBox;
    int w = gridLayer->m_gridWidth;
    int h = gridLayer->m_gridHeight;
    grid[x][y] = -idx;

    //for different adjacent grid box, do different process
    if (x > 0)                  signGrid1(grid, idx, x-1, y);
    if (x < w-1)                signGrid1(grid, idx, x+1, y);
    if (x > 0 && y > 1)         signGrid1(grid, idx, x-1, y-2);
    if (x > 0 && y < h-2)       signGrid1(grid, idx, x-1, y+2);  
    if (x > 0 && y < h-3)       signGrid1(grid, idx, x-1, y+3);  
    if (x < w-1 && y > 1)       signGrid1(grid, idx, x+1, y-2);
    if (x < w-1 && y < h-2)     signGrid1(grid, idx, x+1, y+2);    
    if (x < w-1 && y < h-3)     signGrid1(grid, idx, x+1, y+3);
    if (y > 0)                  signGrid1(grid, idx, x, y-1);
    if (y < h-1)                signGrid1(grid, idx, x, y+1);   
    if (y > 1)                  signGrid1(grid, idx, x, y-2);       
    if (y < h-2)                signGrid1(grid, idx, x, y+2);         
    if (y < h-3)                signGrid1(grid, idx, x, y+3);    
    if (x > 1)                  signGrid1(grid, idx, x-2, y);      
    if (x < w-2)                signGrid1(grid, idx, x+2, y);       
    if (x > 1 && y > 0)         signGrid1(grid, idx, x-2, y-1);
    if (x < w-2 && y > 0)       signGrid1(grid, idx, x+2, y-1);
    if (x > 1 && y < h-1)       signGrid1(grid, idx, x-2, y+1);
    if (x < w-2 && y < h-1)     signGrid1(grid, idx, x+2, y+1);         

    if (x > 0 && y > 0)         signGrid2(grid, idx, x-1, y-1);
    if (x > 0 && y < h-1)       signGrid2(grid, idx, x-1, y+1);
    if (x < w-1 && y > 0)       signGrid2(grid, idx, x+1, y-1);
    if (x < w-1 && y < h-1)     signGrid2(grid, idx, x+1, y+1);                   
}


bool Router::singleNetRoute(int startX, int startY, int endX, int endY, GridLayerBox *gridLayer, vector<vector<int>> &path, const int netId) 
{
    AStar astar;
    Point startP(startX, startY);
    Point endP(endX, endY);
    Point *resP = astar.GetPath(startP, endP, gridLayer, netId);
    if (resP == nullptr) 
        return false;    
    while (resP != nullptr) {
        path.push_back({resP->x, resP->y});
        resP = resP->parent;
    }
    return true;
}

bool Router::astarRoute(GridLayerBox *gridLayer, const std::vector<std::vector<int>> &nodeList, 
                        vector<vector<int>> &path, const int netId) 
{
    assert(nodeList.size() >= 2);
    int startX = nodeList[0][0];
    int startY = nodeList[0][1];
    int endX = nodeList[1][0];
    int endY = nodeList[1][1];

    for (size_t i = 1; i < nodeList.size(); i++) {
        // path_list.push_back({startX, startY});
        bool flag = singleNetRoute(startX, startY, endX, endY, gridLayer, path, netId);
        if (!flag) {
            path.clear();
            log() << '\t' << "Net " << netId << " failed! [no sufficient resource]" << std::endl;
            return false;
        }
        if (i == nodeList.size() - 1)
            break;
        startX = path[path.size() - 1][0]; //nodeList[i-1][0]
        startY = path[path.size() - 1][1]; //nodeList[i-1][1]
        endX = nodeList[i+1][0];
        endY = nodeList[i+1][1];
        // since save path is in reverse order
        for (size_t j = 0; j < path.size() - 1; j++) {
            if (calManhDis(path[j][0], path[j][1], endX, endY) < calManhDis(startX, startY, endX, endY)) {
                startX = path[j][0];
                startY = path[j][1];
            }
        }
    }
    return true;
}

bool Router::checkLegality(GridLayerBox *gridLayer, const int netId) 
{
    bool isLegal = true;
    if (finalPath[netId - 1].size() == 0)
        return false;
    int x, y;
    int w = gridLayer->m_gridWidth;
    int h = gridLayer->m_gridHeight;
    int **grid = gridLayer->gridBox;

    for (size_t i = 0; i < finalPath[netId - 1].size(); i++) {
        x = finalPath[netId - 1][i][0];
        y = finalPath[netId - 1][i][1];

        if (x == 0 && y == 0)                           isLegal = !checkGrid1(grid, x, y);
        else if (x == 0 && y == h - 1)                  isLegal = !checkGrid1(grid, x, y - 1);
        else if (y == 0 && x == w - 1)                  isLegal = !checkGrid1(grid, x - 1, y);
        else if (y == h - 1 && x == w - 1)              isLegal = !checkGrid1(grid, x - 1, y - 1);
        else if (x == 0 && y > 0 && y < h - 1)          isLegal = !checkGrid2(grid, x, y);
        else if (y == 0 && x > 0 && x < w - 1)          isLegal = !checkGrid3(grid, x, y);
        else if (y == h - 1 && x > 0 && x < w - 1)      isLegal = !checkGrid3(grid, x, y - 1);
        else if (x == w - 1 && y > 0 && y < h - 1)      isLegal = !checkGrid2(grid, x - 1, y);
        // normal case
        else                                            isLegal = !checkGrid4(grid, x, y);
        
        if (isLegal == false)
            return false;
    }
    return true;
}

void Router::routeOnGridBox(int idx, int x, int y, GridLayerBox *gridLayer) 
{
    int **grid = gridLayer->gridBox;
    int w = gridLayer->m_gridWidth;
    int h = gridLayer->m_gridHeight;

    grid[x][y] = idx;
    if (x > 0)                                          signGrid1(grid, idx, x - 1, y);
    if (y > 0)                                          signGrid1(grid, idx, x, y - 1);
    if (x < w-1)                                        signGrid1(grid, idx, x + 1, y);
    if (y < h-1)                                        signGrid1(grid, idx, x, y + 1);
    if (x > 0 && y > 0)                                 signGrid1(grid, idx, x - 1, y - 1);
    if (x < w - 1 && y > 0)                             signGrid1(grid, idx, x + 1, y - 1);
    if (w > 0 && y < h - 1)                             signGrid1(grid, idx, x - 1, y + 1);
    if (x < w-1 && y < h-1)                             signGrid1(grid, idx, x + 1, y + 1);
}

/*
// multithread find path
void Router::multiFindPath(GridLayerBox *gridLayer, struct netState* netste, const vector<int> &netToRoute, const vector<vector<vector<int>>> &nodeList)
{
    vector<vector<int>>* arr = new vector<vector<int>> [threadNum];
    bool *res = new bool [threadNum] (); 
    std::thread t[threadNum];
    
    for (size_t i = 0; i < netToRoute.size(); i += threadNum)
        for (size_t j = 0; j < threadNum; j++) {
            int netId = netToRoute[i + j];
            if ((i + j) < netToRoute.size()) {
                log() << "Begin Route Net " << netId << '\t';
                if (nodeList[i + j].empty()) {
                    log() << '\t' << "net with pin, pass" << std::endl;        
                    continue;
                }
                t[j] = std::thread([&] { res[j] = astarRoute( gridLayer, std::ref(nodeList[i + j]), std::ref(arr[j]), netId); });
                
                if (res[j] == true) {
                    log() << '\t' << " finish routing " << std::endl;
                    netste->netRouted.push_back(netId);
                    finalPath[netId - 1].assign(arr[j].begin(), arr[j].end());
                }
                else 
                    netste->netUnroute.push_back(netId);
                t[j].join();
            }
        }
}
*/

void Router::findPath(GridLayerBox *gridLayer, struct netState* netste, const vector<int> &netToRoute, const vector<vector<vector<int>>> &nodeList) 
{
    for (size_t i = 0; i < netToRoute.size(); i++) {
        int netId = netToRoute[i];
        log() << "Begin Route Net " << netId << '\t';
        if (nodeList[i].empty()) {
            log() << '\t' << "net with pin, pass" << std::endl;
            continue;
        }
        vector<vector<int>> tmpPath; //save the routed path
        if (astarRoute(gridLayer, nodeList[i], tmpPath, netId)) {
            log() << '\t' << " finish routing " << std::endl;
            netste->netRouted.push_back(netId);
            finalPath[netId - 1].assign(tmpPath.begin(), tmpPath.end());
        }
        else 
            netste->netUnroute.push_back(netId);
    }
}

void Router::routeNets(GridLayerBox *gridLayer, vector<int>& netToRoute) 
{
    int loopCount = 0;
    int netId;
    int x, y;
    struct netState netSte; //save net routing state

    while (loopCount < RRRLIMIT) {
        netSte.netRouted.clear();
        if (netToRoute.empty())
            break;   
        vector<vector<vector<int>>> portVec(netToRoute.size()); //store convert-to-grid net list
        // write corresponding node_list into port_vec
        for (size_t i = 0; i < netToRoute.size(); i++)
            for (auto n : netList) {
                if (netToRoute[i] == n->m_netId) {
                    portVec[i] = n->m_nodeList;
                    if (!portVec[i].empty())
                        n->convertToGrid(portVec[i], gridLayer->m_layerId); 
                    break;                   
                }
            }

#ifdef DEBUG
        log() << " ---- Print net connect infomation ---- " << std::endl;
        for (size_t i = 0; i < netToRoute.size(); i++) {
            for (size_t j = 0; j < portVec[i].size(); j++)
                std::cout << '\t' << '(' << portVec[i][j][0] << ", " << portVec[i][j][1] << ')' << '\t';
            std::cout << std::endl;
        }
        log() << "--------------------------------------- " << std::endl;
#endif //DEBUG    

        for (size_t i = 0; i < netToRoute.size(); i++)
            for (size_t j = 0; j < portVec[i].size(); j++) {
                x = portVec[i][j][0];
                y = portVec[i][j][1];
                updateGridBox(netToRoute[i], x, y, gridLayer);
            }

        // start net routing
        findPath(gridLayer, &netSte, netToRoute, portVec);

        netToRoute.clear();
        // check rip-up
        for (size_t j = 0; j < netSte.netRouted.size(); j++) {
            netId = netSte.netRouted[j];
            if (checkLegality(gridLayer, netId)) {
                for (size_t k = 0; k < finalPath[netId - 1].size(); k++) {
                    x = finalPath[netId-1][k][0];
                    y = finalPath[netId-1][k][1];
                    routeOnGridBox(netId, x, y, gridLayer);
                }
                log() << '\t' << "Net "<< netId << " success!" << std::endl; 
                layerList[netId-1] = gridLayer->m_layerId;                         
            }
            // add ilegal nets to netToRoute vector and reroute in next iteration
            else {
                log() << '\t' << "net " << netId << " failed [conflict with other net]" << std::endl;
                netToRoute.push_back(netId);
            }
        }
        loopCount++;
    }
    // add unrouted nets to betToRoute vector
    for (auto v : netSte.netUnroute)
        netToRoute.push_back(v);   
}

void Router::runRoutingFlow() 
{
    Timer routerTimer;
    // sort by ascending Manhattan distance
    std::sort(netList.begin(), netList.end(), Router::disCmp);
    int curLayer = 1;
    vector<int> netToRoute; //store the id to be wired
    for (auto net : netList)
            netToRoute.push_back(net->m_netId);         

    while (!netToRoute.empty() && curLayer <= (int)metalList.size()) {
        int gridSpacing = 0;
        for (auto metal : metalList) {
            if (metal->m_id == curLayer) {
                gridSpacing = metal->m_minSpace;
                break;
            }
        }
        assert(gridSpacing != 0 && "grid spacing wrong");
        int width = (DiareaX / gridSpacing) + gridSlack;
        int height = (DiareaY / gridSpacing) + gridSlack;
        GridLayerBox *gridLayer = new GridLayerBox(curLayer, gridSpacing, width, height);
        
        log() << std::endl;
        printlog("------ INITIALIZE GRID ------");
        gridLayer->initGridBox();
        printlog("------ FINIFSH INITIALIZE ------");
        log() << std::endl;
        
        if (gridLayer->m_layerId > 1)
            routeNets(gridLayer, netToRoute);
        // free gridbox memory
        delete gridLayer;
        gridLayer = nullptr;
        curLayer++;
    }

    log() << std::endl;
    log() << "------ FINISH ROUTING ------" << std::endl;
    log() << "Router Process Time: " << routerTimer.elapsed() << std::endl;
    log() << std::endl;
}