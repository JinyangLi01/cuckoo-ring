#ifndef __CUCKOO_RING_H__
#define __CUCKOO_RING_H__

#include "hash/hash_function.h"

using namespace std;
#include <memory.h>
#include <assert.h>
#include <iostream>
#include <string>
#include <vector>
#include <time.h>

typedef unsigned int(*hashFunction)(const unsigned char*str, unsigned int len);
typedef unsigned int uint;
typedef unsigned char uchar;

class cuckooRing
{
public:
    class bucket
    {
    public:
        bucket(int _bSize)
        {
            fp = new uint[_bSize];
            memset(fp, 0, _bSize * sizeof(uint));
            valid = new bool[_bSize];
            memset(valid, 0, _bSize * sizeof(bool));
        }
        ~bucket()
        {
            delete [] fp;
            delete [] fp;
        }
        uint*fp;
        bool*valid;
    };

    cuckooRing(int _bucketNum, int _bucketSize,
        hashFunction _hf, hashFunction _hs, hashFunction _hc)
    :bSize(_bucketNum)
    ,bSlot(_bucketSize)
    ,hf(_hf)
    ,hs(_hs)
    ,hc(_hc)
    {
        buf = new bucket*[_bucketNum];
        for(int i=0;i<_bucketNum;i++)
            buf[i]=new bucket(_bucketSize);
        hSize=1;
        while(hSize<=bSize)
            hSize*=2;
        hSize/=2;
    }

    virtual ~cuckooRing()
    {
        for(int i=0;i<bSize;i++)
            delete buf[i];
        delete [] buf;
    }

    int ring(int pos)
    {
        if(pos>=bSize)
            return pos-bSize;
        if(pos<0)
            return pos+bSize;
        return pos;
    }

    bool insert(int key)
    {
        //calc pos
        uint fp=hf((uchar*)&key,4);
        int start=hs((uchar*)&fp,4);
        int hashk=hc((uchar*)&key,4);
        int hashfk=hc((uchar*)&key,4);
        int p1=ring(start+hashk);
        int p2=ring(start+(hashk^hashfk));

        //find empty slot
        for(int i=0;i<bSlot;i++)
        {
            if(!buf[p1]->valid[i])
            {
                buf[p1]->fp[i]=fp;
                buf[p1]->valid[i]=true;
                return true;
            }
            if(!buf[p2]->valid[i])
            {
                buf[p2]->fp[i]=fp;
                buf[p2]->valid[i]=true;
                return true;
            }
        }

        //kick
        srand((unsigned)time(NULL));
        int kickP=rand()%2==0?p1:p2;
        uint kickFp=fp;
        for(int i=0;i<MAXKICK;i++)
        {
            int kickSlot=rand()%bSlot;
            int tmp=buf[kickP]->fp[kickSlot];
            buf[kickP]->fp[kickSlot]=kickFp;
            kickFp=tmp;
            
            int kickStartPos=hs((uchar*)&kickFp,4);
            int kickHashFk=hc((uchar*)&kickFp,4);
            kickP=ring((ring(kickP-kickStartPos)^kickHashFk)+kickStartPos);

            for(int i=0;i<bSlot;i++)
            {
                if(!buf[kickP]->valid[i])
                {
                    buf[kickP]->fp[i]=fp;
                    buf[kickP]->valid[i]=true;
                    return true;
                }
            }
        }
        return false;
    }

    bool lookup(int key)
    {
        //calc pos
        uint fp=hf((uchar*)&key,4);
        int start=hs((uchar*)&fp,4);
        int hashk=hc((uchar*)&key,4);
        int hashfk=hc((uchar*)&key,4);
        int p1=ring(start+hashk);
        int p2=ring(start+(hashk^hashfk));

        //find
        for(int i=0;i<bSlot;i++)
        {
            if(buf[p1]->valid[i] && buf[p1]->fp[i]==fp)
                return true;
            if(buf[p2]->valid[i] && buf[p2]->fp[i]==fp)
                return true;
        }
        return false;
    }

    bool del(int key)
    {
        //calc pos
        uint fp=hf((uchar*)&key,4);
        int start=hs((uchar*)&fp,4);
        int hashk=hc((uchar*)&key,4);
        int hashfk=hc((uchar*)&key,4);
        int p1=ring(start+hashk);
        int p2=ring(start+(hashk^hashfk));

        //find
        for(int i=0;i<bSlot;i++)
        {
            if(buf[p1]->valid[i] && buf[p1]->fp[i]==fp)
            {
                buf[p1]->valid[i]=false;
                return true;
            }
            if(buf[p2]->valid[i] && buf[p2]->fp[i]==fp)
            {
                buf[p2]->valid[i]=false;
                return true;
            }
        }
        return false;
    }

    void log(bool showDetail=false)
    {
        cout<<"*******************"<<endl;
        cout<<strategyName<<":"<<endl;
        if(showDetail)
        {
        }
    }
protected:
    int bSize;
    int bSlot;
    int hSize;
    bucket **buf;
    hashFunction hf; //fingerprint
    hashFunction hs; //starting position
    hashFunction hc; //cuckoo hash  
    string strategyName;

    int MAXKICK = 250;

    //virtual int findEviction() = 0;
    //virtual void updatePosition(int pos) = 0;
};



#endif
