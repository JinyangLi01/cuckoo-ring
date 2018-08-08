#ifndef __CUCKOO_H__
#define __CUCKOO_H__

class cuckoo
{
public:
    virtual bool insert(int key) = 0;
    virtual bool lookup(int key) = 0;
    virtual bool del(int key) = 0;
    virtual~cuckoo(){};
};



#endif
