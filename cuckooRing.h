#ifndef __CUCKOO_RING_H__
#define __CUCKOO_RING_H__

#include "hash/hash_function.h"
#include "cuckoo.h"

using namespace std;
#include <memory.h>
#include <assert.h>
#include <iostream>
#include <string>
#include <cstring>
#include <vector>
#include <time.h>

class cuckooRing:public cuckoo
{
public:
    class bucket
    {
    public:
        bucket(int _size)
        {
            fp = new uint[_size];
            memset(fp, 0, _size * sizeof(uint));
            valid = new bool[_size];
            memset(valid, 0, _size * sizeof(bool));
        }
        ~bucket()
        {
            delete [] fp;
            delete [] valid;
        }
        uint*fp;
        bool*valid;
    };

    cuckooRing(int _len, int _slot,
        hashFunction _hFP, hashFunction _hOffset, hashFunction _hc)
    :bLen(_len)
    ,bSlot(_slot)
    ,hFP(_hFP)
    ,hOffset(_hOffset)
    ,hc(_hc)
    {
        srand((unsigned)time(NULL));
        buf = new bucket*[_len];
        for(int i=0;i<_len;i++)
            buf[i]=new bucket(_slot);
        hLen=1;
        hPower=0;
        while(hLen<=bLen)
        {
            hLen*=2;
            hPower++;
        }
        hLen/=2;
        hPower--;
    }

    ~cuckooRing()
    {
        for(int i=0;i<bLen;i++)
            delete buf[i];
        delete [] buf;
    }

    int ring(int pos)
    {
        pos=pos%bLen;
        if(pos<0)
            pos+=bLen;
        return pos;
    }

    void getPosByKey(string key, uint&fp, int&p1, int&p2)
    {
        fp=hFP(key.c_str(),4);
        int start=hOffset((char*)&fp,4)%bLen;
        int hashk=hc(key.c_str(),4)&(hLen-1);
        int hashfk=hc((char*)&fp,4)&(hLen-1);
        p1=ring(start+hashk);
        p2=ring(start+(hashk^hashfk));
   }

    int getAnotherPos(int pos, uint fp)
    {
        int start=hOffset((char*)&fp,4)%bLen;
        int hashFk=hc((char*)&fp,4)&(hLen-1);
        return ring(start+(ring(pos-start)^hashFk));
    }

    bool insert(string key)
    {
        uint fp;
        int p1, p2;
        getPosByKey(key, fp, p1, p2);
        return insertWithoutKey(fp, p1, p2);
    }
    bool insertWithoutKey(uint fp, int p1, int p2)
    {
        // already exist
        for(int i=0;i<bSlot;i++)
        {
            if(buf[p1]->valid[i] && buf[p1]->fp[i] == fp)
                return true;
            if(buf[p2]->valid[i] && buf[p2]->fp[i] == fp)
                return true;
        }
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
        int kickP=rand()%2==0?p1:p2;
        uint kickFp=fp;
        for(int i=0;i<MAXKICK;i++)
        {
#if 0
            //random kick with one hop check
            int kickSlot=rand()%bSlot;
            int tmpKickP;

            int offset=rand()%bSlot;
            for(int i=0;i<bSlot;i++,offset++)
            {
                if(offset>=bSlot)offset=0;
                int p2=getAnotherPos(kickP, buf[kickP]->fp[offset]);
                if(kickSlot==offset)
                    tmpKickP=p2;
                for(int j=0;j<bSlot;j++)
                {
                    if(!buf[p2]->valid[j])
                    {
                        buf[p2]->fp[j]=buf[kickP]->fp[offset];
                        buf[p2]->valid[j]=true;
                        buf[kickP]->fp[offset]=kickFp;
                        return true;
                    }
                }
            }
            int tmp=buf[kickP]->fp[kickSlot];
            buf[kickP]->fp[kickSlot]=kickFp;
            kickFp=tmp;
            kickP = tmpKickP;
#endif
#if 1
            //random kick
            int kickSlot=rand()%bSlot;

            int tmp=buf[kickP]->fp[kickSlot];
            buf[kickP]->fp[kickSlot]=kickFp;
            kickFp=tmp;
             
            kickP = getAnotherPos(kickP, kickFp);
            for(int i=0;i<bSlot;i++)
            {
                if(!buf[kickP]->valid[i])
                {
                    buf[kickP]->fp[i]=fp;
                    buf[kickP]->valid[i]=true;
                    return true;
                }
            }
#endif
        }
        return false;
    }

    bool lookup(string key)
    {
        uint fp;
        int p1, p2;
        getPosByKey(key, fp, p1, p2);

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

    bool del(string key)
    {
        uint fp;
        int p1, p2;
        getPosByKey(key, fp, p1, p2);

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

    void calcPower(int len, int&size, int&power)
    {
        if(len<=0)
            return;
        size=1, power=0;
        while(size<=len)
        {
            size*=2;
            power++;
        }
        size=size/2;
        power--;
    }

    bool lazy_update(){
        
    }
    bool resize(int len)
    {
        if(len<=0)
            return false;
        if(len == 2 * bLen)
            lazy_update();
        //save backup info
        int saveBLen=bLen;
        int saveHLen=hLen;
        int saveHPower=hPower;
        bucket**saveBuf=buf;
        
        //new space
        bLen=len;
        calcPower(len, hLen, hPower);
        if(hPower>saveHPower)
        {   //hPower cannot be larger than before
            hLen=saveHLen;
            hPower=saveHPower;
        }
        buf = new bucket*[len];
        for(int i=0;i<len;i++)
            buf[i]=new bucket(bSlot);

        //transfer
        bool suc=true;
        for(int i=0;i<saveBLen;i++)
        {
            for(int j=0;j<bSlot;j++)
            {
                if(saveBuf[i]->valid[j])
                {
                    uint transFp=saveBuf[i]->fp[j];
                    int oldStart=hOffset((char*)&transFp,4)%saveBLen;
                    int newStart=hOffset((char*)&transFp,4)%bLen;
                    int curPos=(i-oldStart+saveBLen)%saveBLen;
                    curPos=curPos&(hLen-1);
                    int hashFk=hc((char*)&transFp,4)&(hLen-1);
                    int p1=ring(newStart+curPos);
                    int p2=ring(newStart+(curPos^hashFk));
                    suc=insertWithoutKey(transFp,p1,p2);
                }
                if(!suc)
                    break;
            }
            if(!suc)
                break;
        }

        //end
        if(!suc)
        {
            for(int i=0;i<bLen;i++)
                delete buf[i];
            delete [] buf;
            //load backup info
            buf = saveBuf;
            bLen = saveBLen;
            return false;
        }
        for(int i=0;i<saveBLen;i++)
            delete saveBuf[i];
        delete [] saveBuf;
        return true;
    }

    bool expand(int len)
    {
        if(len<=bLen)
            return false;
        return resize(len);
    }
    bool expand(){return expand(bLen*2);}

    bool compress(int len)
    {
        if(len>=bLen || len <=0)
            return false;
        return resize(len);
    }
    bool compress(){return compress(bLen/2);}

    void printBuf()
    {
        //return;
        cout<<"para:"<<bLen<<" "<<bSlot<<" "<<hLen<<endl;
        for(int i=0;i<bLen;i++)
        {
            for(int j=0;j<bSlot;j++)
            {
                if(buf[i]->valid[j])
                    cout<<buf[i]->fp[j]<<" ";
                else
                    cout<<"X ";
            }
            cout<<endl;
        }
    }
protected:
    int bLen;
    int bSlot;
    int hLen;
    int hPower;
    bucket **buf;
    hashFunction hFP; //fingerprint
    hashFunction hOffset; //starting position
    hashFunction hc; //cuckoo hash  
    string strategyName;

    int MAXKICK = 250;

    //virtual int findEviction() = 0;
    //virtual void updatePosition(int pos) = 0;
};



#endif
