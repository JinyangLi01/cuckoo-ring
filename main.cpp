#include "testManager.h"

using namespace std;
#include <vector>

int main(int argc, char ** argv)
{
    cuckooRing c(55, 2, BOB1, BOB2, BOB3);
    cout<<c.insert(1)<<endl;c.printBuf();
    cout<<c.insert(2)<<endl;c.printBuf();
    cout<<c.insert(3)<<endl;c.printBuf();
    cout<<c.insert(4)<<endl;c.printBuf();
    cout<<c.insert(5)<<endl;c.printBuf();
    cout<<c.insert(6)<<endl;c.printBuf();
    cout<<c.insert(7)<<endl;c.printBuf();
    cout<<c.insert(8)<<endl;c.printBuf();
    //cout<<c.lookup(1)<<endl;c.printBuf();
    //cout<<c.lookup(-1)<<endl;c.printBuf();
    //cout<<c.del(1)<<endl;c.printBuf();
    //cout<<c.lookup(1)<<endl;c.printBuf();

#if 0
    testManager m;
    //m.beginTest(10000);
    m.log(true);
#endif
    return 0;
}

