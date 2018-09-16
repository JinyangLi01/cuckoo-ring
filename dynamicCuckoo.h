#ifndef __DYNAMIC_CUCKOO_H__
#define __DYNAMIC_CUCKOO_H__

#include "hash/hash_function.h"
#include "cuckoo.h"

using namespace std;
#include <memory.h>
#include <assert.h>
#include <iostream>
#include <string>
#include <vector>
#include <time.h>
#include <set>

class dynamicCuckoo: public cuckoo
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
		bucket() {}
		~bucket() 
        {
			delete[] fp;
			delete[] valid;
		}
		uint * fp;
		bool * valid;
	};
	class LinkedCF
    {
	public:
		//static int MAXKICK;
		int nbucket;
		int bucketsize;
        int hLen;
		int count;
		hashFunction hFP;
		hashFunction hc;
		LinkedCF * nextCF;
		bucket ** buc;
		LinkedCF(int _nbucket, int _bucketsize, hashFunction _hFP, hashFunction _hc, LinkedCF * _nextCF = NULL) 
        :nbucket(_nbucket)
        ,bucketsize(_bucketsize)
        ,hFP(_hFP)
        ,hc(_hc)
        ,nextCF(_nextCF) 
        {
			buc = new bucket * [nbucket];
			for (int i = 0; i < nbucket; i++)
				buc[i] = new bucket(_bucketsize);
            hLen=1;
            while(hLen<=nbucket)
                hLen*=2;
            hLen/=2;
			count = 0;
		}
		virtual ~LinkedCF()
		{
			if (buc != NULL)
				delete[] buc;
		}
	public:
		void getPosByKey(int key, uint & fp, int & p1, int & p2) 
        {
			fp = hFP((uchar * )&key, 4);
			uint hs = hc((uchar *)&key, 4);
			p1 = hs & (hLen-1);
			p2 = (hs ^ fp) & (hLen-1);
		}
		int getAnotherPos(int pos, uint fp) 
        {
			return (pos ^ fp) & (hLen-1);
		}

		bool inquiry(int key) 
        {
			uint fp;
			int p1, p2;
			getPosByKey(key, fp, p1, p2);

			//find
			for (int i = 0; i<bucketsize; i++) 
            {
				if (buc[p1]->valid[i] && buc[p1]->fp[i] == fp)
					return true;
				if (buc[p2]->valid[i] && buc[p2]->fp[i] == fp)
					return true;
			}
			return false;
		}
		bool del(int key) 
        {
			uint fp;
			int p1, p2;
			getPosByKey(key, fp, p1, p2);

			//find
			for (int i = 0; i<bucketsize; i++) 
            {
				if (buc[p1]->valid[i] && buc[p1]->fp[i] == fp) 
                {
					buc[p1]->valid[i] = false;
					count--;
					return true;
				}
				if (buc[p2]->valid[i] && buc[p2]->fp[i] == fp) 
                {
					buc[p2]->valid[i] = false;
					count--;
					return true;
				}
			}
			return false;
		}
		bool insert(int key, uint & victimfp) 
        {
			uint fp;
			int p1, p2;
			getPosByKey(key, fp, p1, p2);
			return rand() % 2 == 0 ? insertWithoutKey(fp, p1, p2, victimfp) 
									: insertWithoutKey(fp, p2, p1, victimfp);
		}
		bool insertWithoutKey(uint fp, int p1, int p2, uint & victimfp) 
        {
			//find empty slot
			for (int i = 0; i<bucketsize; i++)
			{
				if (!buc[p1]->valid[i])	
                {
					buc[p1]->fp[i] = fp;
					buc[p1]->valid[i] = true;
					count++;
					return true;
				}
				if (!buc[p2]->valid[i]) 
                {
					buc[p2]->fp[i] = fp;
					buc[p2]->valid[i] = true;
					count++;
					return true;
				}
			}

			//kick
			int kickP = rand() % 2 == 0 ? p1 : p2;
			uint kickFp = fp;
			for (int i = 0; i<MAXKICK; i++) 
            {
#if 1
				//random kick with one hop check
				int kickSlot = rand() % bucketsize;
				int tmpKickP;

                int offset=rand()%bucketsize;
				for (int i = 0; i<bucketsize; i++, offset++) 
                {
                    if (offset>=bucketsize)offset=0;
					int p2 = getAnotherPos(kickP, buc[kickP]->fp[offset]);
					if (kickSlot == offset)
						tmpKickP = p2;
					for (int j = 0; j<bucketsize; j++) 
                    {
						if (!buc[p2]->valid[j]) 
                        {
							buc[p2]->fp[j] = buc[kickP]->fp[offset];
							buc[p2]->valid[j] = true;
							buc[kickP]->fp[offset] = kickFp;
							count++;
							return true;
						}
					}
				}
				int tmp = buc[kickP]->fp[kickSlot];
				buc[kickP]->fp[kickSlot] = kickFp;
				kickFp = tmp;
				kickP = tmpKickP;
#endif
#if 0
				//random kick
				int kickSlot = rand() % bucketsize;

				int tmp = buc[kickP]->fp[kickSlot];
				buc[kickP]->fp[kickSlot] = kickFp;
				kickFp = tmp;

				kickP = getAnotherPos(kickP, kickFp);
				for (int i = 0; i<bucketsize; i++) 
                {
					if (!buc[kickP]->valid[i]) 
                    {
						buc[kickP]->fp[i] = fp;
						buc[kickP]->valid[i] = true;
						count++;
						return true;
					}
				}
#endif
			}
			victimfp = kickFp;
			return false;
		}
		bool insertWithoutkick(uint fp, int idx) 
        {
			for (int i = 0; i < bucketsize; i++) 
            {
				if (!buc[idx]->valid[i]) 
                {
					buc[idx]->fp[i] = fp;
					buc[idx]->valid[i] = true;
					count++;
					return true;
				}
			}
			return false;
		}
	};
	class LinkedCFoperator 
    {
	public:
		bool operator() (const LinkedCF* lc, const LinkedCF* rc) 
        {
			return lc->count < rc->count;
		}
	};

protected:
	int nBucket;
	int BucketSize;
	LinkedCF * head;
	LinkedCF * curr;
	LinkedCF * next;
	hashFunction hFP; //fingerprint
	hashFunction hc; //cuckoo hash  

	const static int MAXKICK = 250;
public:
	dynamicCuckoo(int _nBucket, int _BucketSize, hashFunction _hFP, hashFunction _hc)
    :nBucket(_nBucket)
    ,BucketSize(_BucketSize)
    ,hFP(_hFP)
    ,hc(_hc) 
    {
		//LinkedCF(int _nbucket, int _bucketsize, hashFunction _hFP, hashFunction _hc, LinkedCF * _nextCF = NULL)
		next = NULL;
		curr = new LinkedCF(_nBucket, _BucketSize, _hFP, _hc);
		head = curr;
	}
	bool insert(int key) 
    {
		uint victimfp;
		bool token = curr->insert(key, victimfp);
		if (token) 
        {
			return true;
		}
		else 
        {
			curr->nextCF = new LinkedCF(nBucket, BucketSize, hFP, hc);
			curr = curr->nextCF;
			token = curr->insert(key, victimfp);
			if (!token) 
            {
				//err occurs here
				cerr << "insert failed" << endl;
			}
		}
		return true;
	}
	bool lookup(int key) 
    {
		LinkedCF * tmp = head;
		while (tmp != NULL) 
        {
			if (tmp->inquiry(key)) 
            {
				return true;
			}
			tmp = tmp->nextCF;
		}
		return false;
	}
	bool del(int key) 
    {
		LinkedCF * tmp = head;
		bool token = false;
		while(tmp != NULL) 
        {
			if (tmp->del(key)) 
            {
				token = true;
			}
			tmp = tmp->nextCF;
		}
		return token;
	}
	bool compact() 
    {
		multiset<LinkedCF *, LinkedCFoperator> CFQ;
		multiset<LinkedCF *, LinkedCFoperator> cfq;
		multiset<LinkedCF *, LinkedCFoperator>::iterator lit;
		multiset<LinkedCF *, LinkedCFoperator>::reverse_iterator rit;
		CFQ.clear();
		LinkedCF * tmp = head;
		while (tmp != NULL) 
        {
			CFQ.insert(tmp);
		}
		for (lit = CFQ.begin(); lit != CFQ.end(); lit++) 
        {
			int failnum = 0;
			//search every bucket
			for (int i = 0; i < nBucket; i++) 
            {
				//for each bucket, search every unit
				for (int j = 0; j < BucketSize; j++) 
                {
					if ((*lit)->buc[i]->valid[j]) 
                    {
						uint tmpfp = (*lit)->buc[i]->fp[j];
						bool succ = false;
						//search from the end to the start
						for (rit = CFQ.rbegin(); (*rit) != (*lit); rit++) 
                        {
							succ = (*rit)->insertWithoutkick(tmpfp, i);
							if (succ) 
                            {
								(*lit)->buc[i]->valid[j] = false;
								(*lit)->count--;
								break;
							}
						}
						if (!succ)
							failnum++;
					}
					if ((*lit)->count == failnum)
						break;
				}
				if ((*lit)->count == failnum)
					break;
			}
		}
		cfq.clear();
		for (lit = CFQ.begin(); lit != CFQ.end(); lit++) 
        {
			cfq.insert(*lit);
		}
		CFQ.clear();
		//adjust to the new DCF structure
		rit = cfq.rbegin();
		head = *rit;
		LinkedCF * prev = head;
		for (++rit; rit != cfq.rend(); rit++) 
        {
			if ((*rit)->count == 0) 
            {
				prev->nextCF = NULL;
				delete[] (*rit);
			}
			else 
            {
				prev->nextCF = *rit;
				prev = *rit;
			}
		}
		curr = prev;
		return true;
	}
    void printBuf(){};
};



#endif

#pragma once
