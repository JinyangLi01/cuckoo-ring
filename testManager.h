#ifndef __TEST_MANAGER_H__
#define __TEST_MANAGER_H__

#include "cuckooRing.h"
#include "dynamicCuckoo.h"
#include "timer.h"

using namespace std;
#include <stdio.h>
#include <vector>
#include <fstream>
#include <set>
#include <ctime>

class testManager
{
public:
    int strategySize;
    vector<cuckoo*> strategyList;
    ifstream fin;        
    char readBuf[105];
    set<int> keySet;

    testManager();
    ~testManager();

    bool addStrategy(cuckoo* s);

    int read(); 
    
    void insertTest(int updateNum);

    void log(bool showDetail=false);
};














#endif
