#include "testManager.h"

testManager::testManager():
strategySize(0)
{
    srand(unsigned(time(0)));
    fin = ifstream("dat/0.dat",ios::in|ios::binary);
}

testManager::~testManager()
{
    for(auto it=strategyList.begin();it!=strategyList.end();it++)
    {
        cuckoo *s = *it;
        delete s;
    }
}

bool testManager::addStrategy(cuckoo* s)
{
    strategySize++;
    strategyList.push_back(s);
    return true;
}

int testManager::read()
{
    return rand();
    fin.read(readBuf,13);
    readBuf[4]='\0';
    int tmp=0;
    for(int i=0;i<4;i++)tmp=tmp*256+(int)readBuf[i];
    return tmp;
}

void testManager::insertTest(int updateNum)
{
    while(updateNum--)
    {
        int n=read();
        while(keySet.find(n)!=keySet.end())
            n=read();
        keySet.insert(n);
        for(auto it=strategyList.begin();it!=strategyList.end();it++)
        {
            bool flag=(*it)->insert(n);
            if(flag==false)
            {
                cout<<"INSERT FAIL!"<<endl;
                return;
            }
        }
    }
}

void testManager::log(bool showDetail)
{
    for(auto it=strategyList.begin();it!=strategyList.end();it++)
    {
        //(*it)->log(showDetail);
    }
}
