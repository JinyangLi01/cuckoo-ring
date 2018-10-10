#ifndef __CUCKOO_FILTER_H__
#define __CUCKOO_FILTER_H__

#include "hash/hash_function.h"
#include "cuckoo.h"

using namespace std;
#include <cstring>
#include <string>
#include <iostream>
#include <time.h>

class cuckooFilter: public cuckoo{
public:
	class Bucket{
	public:
		Bucket(int _size){
			fp = new uint[_size];
			memset(fp, 0, _size * sizeof(int));
			valid = new bool[_size];
			memset(valid, 0, _size * sizeof(bool));
		}
		~Bucket(){
			delete[] fp;
			delete[] valid;
		}
		uint *fp;
		bool *valid;
	};

	cuckooFilter(int _L, int _slot, hashFunction _hfp, hashFunction _hpos):
	L(_L), slot(_slot), hfp(_hfp), hpos(_hpos), memory_access_num(0),
	hop_num(0){
		bucket = new Bucket*[L];
		for(int i = 0; i < L; ++i)
			bucket[i] = new Bucket(slot);
	}
	~cuckooFilter(){
		for(int i = 0; i < L; ++i)
			delete bucket[i];
		delete[] bucket;
	}
	void Write(int p, int t, uint fp){
		bucket[p]->valid[t] = 1;
		bucket[p]->fp[t] = fp;
		memory_access_num += 2;
	}
 	bool insert(string key){
 		uint fp = hfp(key.c_str(), 4);
 		int p1 = hpos(key.c_str(), 4) % L;
 		int p2 = (p1 ^ hpos((char*)&fp, 4)) % L;
 		// already exist
 		for(int i = 0; i < slot; ++i){
			if(bucket[p1]->valid[i] && bucket[p1]->fp[i] == fp){
				memory_access_num += 2;
				return true;
			}
			else 
				memory_access_num++;
			if(bucket[p2]->valid[i] && bucket[p2]->fp[i] == fp){
				memory_access_num += 2;
				return true;
			}
			else
				memory_access_num++;
		}
		// find empty slot
		for(int i = 0; i < slot; ++i){
			if(bucket[p1]->valid[i] == 0){
				memory_access_num++;
				Write(p1, i, fp);
				return true;
			}
			else if(bucket[p2]->valid[i] == 0){
				memory_access_num += 2;
				Write(p2, i, fp);
				return true;
			}
			else
				memory_access_num += 2;
		}
		// kick
        srand((unsigned)time(NULL));
        int _fp, t;
		for(int i = 0; i < max_kick; ++i){
			t = rand() % slot;
			_fp = bucket[p1]->fp[t];
			memory_access_num++;
			Write(p1, t, fp);
			// find alternative postion
			p1 = (hpos((char*)&_fp, 4)^p1) % L;
			fp = _fp;
			for(int j = 0; j < slot; ++j){
				if(bucket[p1]->valid[j] == 0){
					memory_access_num++;
					Write(p1, j, fp);
					hop_num += i;
					return true;
				}
				else
					memory_access_num++;
			}
		}
		return false;
 	}
 	bool lookup(string key){
 		uint fp = hfp(key.c_str(), 4);
 		int p1 = hpos(key.c_str(), 4) % L;
 		int p2 = (p1 ^ hpos((char*)&fp, 4)) % L;
 		for(int i = 0; i < slot; ++i){
 			if(bucket[p1]->valid[i] && bucket[p1]->fp[i] == fp)
 				return true;
 			if(bucket[p2]->valid[i] && bucket[p2]->fp[i] == fp)
 				return true;
 		}
 		return false;
 	}
	bool del(string key){}
	bool resize(int len){}
	int Get_Memory_Access_Num(){
		return memory_access_num;
	}
	int Get_Hop_Num(){
		return hop_num;
	}

private:
	int L, slot;
	int memory_access_num;
	int hop_num;
	Bucket **bucket;
	hashFunction hfp;
	hashFunction hpos;
	int max_kick = 250;
};

#endif