#ifndef __CUCKOO_H__
#define __CUCKOO_H__

typedef unsigned int(*hashFunction)(const unsigned char*str, unsigned int len);
typedef unsigned int uint;
typedef unsigned char uchar;

class cuckoo
{
public:
    virtual bool insert(int key) = 0;
    virtual bool lookup(int key) = 0;
    virtual bool del(int key) = 0;
    virtual~cuckoo(){};
};



#endif
