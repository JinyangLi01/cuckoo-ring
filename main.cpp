#include "testManager.h"

using namespace std;
#include <vector>

int main(int argc, char ** argv)
{
    //timer t;
    //t.run();
#if 0
    dynamicCuckoo c(16, 4, BOB1, BOB2);
    cout<<c.insert(1)<<endl;c.printBuf();
    cout<<c.insert(2)<<endl;c.printBuf();
    cout<<c.insert(3)<<endl;c.printBuf();
    cout<<c.insert(4)<<endl;c.printBuf();
    cout<<c.insert(5)<<endl;c.printBuf();
    cout<<c.insert(6)<<endl;c.printBuf();
    cout<<c.insert(7)<<endl;c.printBuf();
    cout<<c.insert(8)<<endl;c.printBuf();
    cout<<c.lookup(1)<<endl;c.printBuf();
    cout<<c.lookup(-1)<<endl;c.printBuf();
    cout<<c.del(1)<<endl;c.printBuf();
    cout<<c.lookup(1)<<endl;c.printBuf();
#endif
#if 0
    cuckooRing c(16, 4, BOB1, BOB2, BOB3);
    cout<<c.insert(1)<<endl;c.printBuf();
    cout<<c.insert(2)<<endl;c.printBuf();
    cout<<c.insert(3)<<endl;c.printBuf();
    cout<<c.insert(4)<<endl;c.printBuf();
    cout<<c.insert(5)<<endl;c.printBuf();
    cout<<c.insert(6)<<endl;c.printBuf();
    cout<<c.insert(7)<<endl;c.printBuf();
    cout<<c.insert(8)<<endl;c.printBuf();
    cout<<c.lookup(1)<<endl;c.printBuf();
    cout<<c.lookup(-1)<<endl;c.printBuf();
    cout<<c.del(1)<<endl;c.printBuf();
    cout<<c.lookup(1)<<endl;c.printBuf();
    cout<<c.resize(10)<<endl;c.printBuf();
    cout<<c.resize(4)<<endl;c.printBuf();
    cout<<c.lookup(2)<<endl;
    cout<<c.lookup(3)<<endl;
    cout<<c.lookup(4)<<endl;
    cout<<c.resize(2)<<endl;c.printBuf();
    cout<<c.resize(14)<<endl;c.printBuf();
    cout<<c.lookup(2)<<endl;
    cout<<c.lookup(3)<<endl;
    cout<<c.lookup(4)<<endl;
    cout<<c.expand()<<endl;c.printBuf();
    cout<<c.compress()<<endl;c.printBuf();
    cout<<c.compress()<<endl;c.printBuf();
    cout<<c.compress()<<endl;c.printBuf();
    cout<<c.expand()<<endl;c.printBuf();
#endif
#if 1
    testManager m1,m2;
    m1.addStrategy(new cuckooRing(40000, 4, BOB1, BOB2, BOB3));
    m2.addStrategy(new dynamicCuckoo(40000, 4, BOB1, BOB2));
    timer t1,t2;
    t1.run();
    m1.insertTest(100000);
    t1.pause();
    t2.run();
    m2.insertTest(100000);
    t2.pause();
    cout<<"time:"<<t1.getTime()<<" "<<t2.getTime()<<endl;
#endif
    //cout<<"time:"<<t.getTime()<<endl;
    return 0;
}

