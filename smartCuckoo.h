#ifndef __SMART_CUCKOO_H__
#define __SMART_CUCKOO_H__

#include "hash/hash_function.h"
#include "cuckoo.h"

using namespace std;
#include <cstring>
#include <string>
#include <iostream>

class smartCuckoo: public cuckoo{
public:
	int same_num;

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

	smartCuckoo(int _L, int _slot, hashFunction _hfp, hashFunction _hpos):
	L(_L), slot(_slot), hfp(_hfp), hpos(_hpos){
		same_num = 0;
		bucket = new Bucket*[L];
		for(int i = 0; i < L; ++i)
			bucket[i] = new Bucket(slot);
	}
	~smartCuckoo(){
		for(int i = 0; i < L; ++i)
			delete bucket[i];
		delete[] bucket;
	}
	bool EmptyAlternative(int p, int t, int &alter_pos){
		uint fp = bucket[p]->fp[t];
		int y = (hpos((char*)&fp, 4)^p) % L;
		for(int i = 0; i < slot; ++i)
			if(bucket[y]->valid[i] == 0){
				alter_pos = i;
				return true;
			}
		return false;		
	}
	bool InsertWithoutKey(uint fp, int p1, int p2){
		// already exist
		for(int i = 0; i < slot; ++i){
			if(bucket[p1]->valid[i] && bucket[p1]->fp[i] == fp){
				same_num++;
				cout << fp << '\n';
				cout << same_num << '\n';
				return true;
			}
			if(bucket[p2]->valid[i] && bucket[p2]->fp[i] == fp){
				same_num++;
				cout << fp << '\n';
				cout << same_num << '\n';
				return true;
			}
		}
		int alter_pos = 0;
		for(int i = 0; i < slot; ++i){
			if(bucket[p1]->valid[i] == 0 || EmptyAlternative(p1, i, alter_pos)){
				uint _fp = bucket[p1]->fp[i];
				int x = alter_pos;
				int y = (hpos((char*)&_fp, 4)^p1) % L;
				bucket[y]->fp[x] = _fp;
				bucket[y]->valid[x] = 1;
				bucket[p1]->fp[i] = fp;
				bucket[p1]->valid[i] = 1;
				return true;				
			}
			if(bucket[p2]->valid[i] == 0 || EmptyAlternative(p2, i, alter_pos)){
				uint _fp = bucket[p2]->fp[i];
				int x = alter_pos;
				int y = (hpos((char*)&_fp, 4)^p2) % L;
				bucket[y]->fp[x] = _fp;
				bucket[y]->valid[x] = 1;
				bucket[p2]->fp[i] = fp;
				bucket[p2]->valid[i] = 1;
				return true;				
			}		
		}
		return false;
	}
 	bool insert(string key){
 		uint fp = hfp(key.c_str(), 4);
		if(fp == 4099916420)
			cout << fp << '\n';

 		int p1 = hpos(key.c_str(), 4) % L;
 		int p2 = (p1 ^ hpos((char*)&fp, 4)) % L;
 		return InsertWithoutKey(fp, p1, p2);
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


private:
	int L, slot;
	Bucket **bucket;
	hashFunction hfp;
	hashFunction hpos;
	int max_kick = 1;
};

#endif