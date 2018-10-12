#ifndef __CUCKOO_RING_S_H__
#define __CUCKOO_RING_S_H__

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
#include <assert.h>

class cuckooRingS:public cuckoo
{
public:
    cuckooRingS(int _len, int _slot,
        hashFunction _hFP, hashFunction _hOffset, hashFunction _hc)
    :bLen(_len)
    ,bSlot(_slot)
    ,hFP(_hFP)
    ,hOffset(_hOffset)
    ,hc(_hc)
    ,memory_access_num(0)
    ,hop_num(0)
    {
        srand((unsigned)time(NULL));
        int tSize = _len * _slot;
        buf = new uint[tSize];
        memset(buf, 0, tSize * sizeof(uint));
        valid = new bool[tSize];
        memset(valid, 0, tSize * sizeof(bool));

        isExpanded = false;

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

    ~cuckooRingS()
    {
        delete [] buf;
        delete [] valid;
        if(copy!=NULL)
            delete [] copy;
    }

    inline int getIndex(int pos, int slot)
    {
        return pos*bSlot + slot;
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

    void removeCopy(int pos)
    {
        if(!isExpanded)
            return;
        assert(copy!=NULL);
        if(copy[pos])
            return;

        int bufPos=getIndex(pos, 0);
        if(!valid[bufPos] && !copy[bufPos])
        for(int i=0;i<bSlot;i++)
        {
            if(!valid[bufPos+i])
                continue;
            uint fp=buf[bufPos+i];
            int start=hOffset((char*)&fp,4)%bLen;
            int offset=ring(pos-start);        
            if(offset<hLen)
                copy[bufPos+i]=0;
            else
                valid[bufPos+i]=0;               
        }
        copy[pos]=true;
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
        int bufPos1=getIndex(p1, 0);
        int bufPos2=getIndex(p2, 0);
        removeCopy(p1);
        removeCopy(p2);

        // already exist
        for(int i=0;i<bSlot;i++)
        {
            if(valid[bufPos1+i] && buf[bufPos1+i] == fp){
                memory_access_num += 2;
                return true;
            }
            else
                memory_access_num++;
            if(valid[bufPos2+i] && buf[bufPos2+i] == fp){
                memory_access_num += 2;
                return true;
            }
            else
                memory_access_num++;
        }
        //find empty slot
        for(int i=0;i<bSlot;i++)
        {
            if(!valid[bufPos1+i])
            {
                buf[bufPos1+i]=fp;
                valid[bufPos1+i]=true;
                memory_access_num += 3;
                return true;
            }
            else
                memory_access_num++;
            if(!valid[bufPos2+i])
            {
                buf[bufPos2+i]=fp;
                valid[bufPos2+i]=true;
                memory_access_num += 3;
                return true;
            }
            else
                memory_access_num++;
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
            for(int j=0;j<bSlot;j++,offset++)
            {
                if(offset>=bSlot)offset=0;
                int p2=getAnotherPos(kickP, buf[getIndex(kickP, offset)]);
                removeCopy(p2); 
                int bufPos=getIndex(p2, 0);
                memory_access_num++;

                if(kickSlot==offset)
                    tmpKickP=p2;
                for(int k=0;k<bSlot;k++)
                {
                    if(!valid[bufPos+k])
                    {
                        buf[bufPos+k]=buf[getIndex(kickP, offset)];
                        valid[bufPos+k]=true;
                        buf[getIndex(kickP, offset)]=kickFp;
                        memory_access_num += 4;
                        return true;
                    }
                    else
                        memory_access_num++;
                }
            }
            int tmp=buf[getIndex(kickP, kickSlot)];
            buf[getIndex(kickP,kickSlot)]=kickFp;
            memory_access_num += 2;
            kickFp=tmp;
            kickP = tmpKickP;
#endif
#if 1
            //random kick
            int kickSlot=rand()%bSlot;
            
            int tmp=buf[getIndex(kickP, kickSlot)];
            int kickP2=getAnotherPos(kickP, tmp);
            //remove copy
            removeCopy(kickP2);

            buf[getIndex(kickP, kickSlot)]=kickFp;
            kickFp=tmp;
            memory_access_num += 2;
            kickP = kickP2;
            int kickBufPos=getIndex(kickP, 0);
            for(int j=0;j<bSlot;j++)
            {
                if(!valid[kickBufPos+j])
                {
                    buf[kickBufPos+j]=fp;
                    valid[kickBufPos+j]=true;
                    memory_access_num += 3;
                    hop_num += i;
                    return true;
                }
                else
                    memory_access_num++;
            }
#endif
        }
        return false;
    }

    int Get_Memory_Access_Num(){
        return memory_access_num;
    }
    int Get_Hop_Num(){
        return hop_num;
    }
    bool lookup(string key)
    {
        uint fp;
        int p1, p2;
        getPosByKey(key, fp, p1, p2);

        //remove copy
        removeCopy(p1);
        removeCopy(p2);

        //find
        int bufPos1=getIndex(p1, 0);
        int bufPos2=getIndex(p2, 0); 
        for(int i=0;i<bSlot;i++)
        {
            if(valid[bufPos1+i] && buf[bufPos1+i]==fp)
                return true;
            if(valid[bufPos2+i] && buf[bufPos2+i]==fp)
                return true;
        }
        return false;
    }

    bool del(string key)
    {
        uint fp;
        int p1, p2;
        getPosByKey(key, fp, p1, p2);

        //remove copy
        removeCopy(p1);
        removeCopy(p2);

        //find
        int bufPos1=getIndex(p1, 0);
        int bufPos2=getIndex(p2, 0);  
        for(int i=0;i<bSlot;i++)
        {
            if(valid[bufPos1+i] && buf[bufPos1+i]==fp)
            {
                valid[bufPos1+i]=false;
                return true;
            }
            if(valid[bufPos2+i] && buf[bufPos2+i]==fp)
            {
                valid[bufPos2+i]=false;
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

    bool resize(int len)
    {
        if(len!=2*bLen)
            return false;
        return expand();
    }

    bool expand()
    {
        if(isExpanded)
            return false;
        isExpanded=true;
        
        int len = 2 * bLen;
        //save backup info
        int saveBLen=bLen;
        int saveHLen=hLen;
        int saveHPower=hPower;
        uint*saveBuf=buf;
        bool*saveValid=valid;
        
        //new space
        bLen=len;
        calcPower(len, hLen, hPower);
        if(hPower>saveHPower)
        {   //hPower cannot be larger than before
            hLen=saveHLen;
            hPower=saveHPower;
        }
        int tSize=len * bSlot;
        buf = new uint[tSize];
        valid = new bool[tSize];
        copy = new bool[len];

        //transfer
        int saveSize = saveBLen * bSlot;
        int s1 = saveSize * sizeof(uint), s2 = saveSize * sizeof(bool);
        memcpy((char*)buf, (char*)saveBuf, s1);
        memcpy((char*)buf + s1, (char*)saveBuf, s1);
        memcpy((char*)valid, (char*)saveValid, s2);
        memcpy((char*)valid + s2, (char*)saveValid, s2);
        memset(copy, 0, len*sizeof(bool));

        delete [] saveBuf;
        delete [] saveValid;
        return true;
    }

    void printBuf()
    {
        //return;
        cout<<"para:"<<bLen<<" "<<bSlot<<" "<<hLen<<endl;
        for(int i=0;i<bLen;i++)
        {
            for(int j=0;j<bSlot;j++)
            {
                if(valid[getIndex(i, j)])
                    cout<<buf[getIndex(i, j)]<<" ";
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
    int memory_access_num;
    int hop_num;
    uint *buf;
    bool *valid;
    bool *copy;
    bool isExpanded;
    hashFunction hFP; //fingerprint
    hashFunction hOffset; //starting position
    hashFunction hc; //cuckoo hash  
    string strategyName;

    int MAXKICK = 250;

    //virtual int findEviction() = 0;
    //virtual void updatePosition(int pos) = 0;
};



#endif
