#include "router/router.h"
#include "parser/parser.h"
#include "util/config.h"
#include "util/utils.h"

int main(int argc, char *argv[]) 
{
    printlog("------------------------------------------------------------------------------");
    printlog("                 INTEGRATED CIRCUIT EDA ELITE CHALLENGE 2020                  ");
    printlog("              A Detailed Routing Algorithm Based On A* Algorithm              ");
    printlog("                  Author: Ziyang Gao, Kaifan Cai, Shuqi wang                  ");
    printlog("                  Affiliation: Shanghai Jiao Tong University                  ");
    printlog("------------------------------------------------------------------------------");

    if (argc < 3) {
        std::cout << "no enough input file!" << std::endl;
        return -1;
    }
    parseInputSpec(argc, argv);
    Router router;
    router.runRoutingFlow();
    writeReusltToDef();
    deallocate();  

    printlog("------------------------------------------------------------------------------");
    printlog("                                  FINISHED                                    ");
    printlog("------------------------------------------------------------------------------");

    return 0;
}