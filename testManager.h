#ifndef __TEST_MANAGER_H__
#define __TEST_MANAGER_H__

#include "cuckooRing.h"

using namespace std;
#include <stdio.h>
#include <vector>
#include <fstream>

class testManager
{
public:
    int strategySize;
    vector<cuckoo*> strategyList;
    ifstream fin;        
    char readBuf[105];

    testManager();
    ~testManager();

    bool addStrategy(cuckoo* s);

    int read(); 
    
    void beginTest(int updateNum);

    void log(bool showDetail=false);
};














#endif
