#include "cuckoo.h"
#include "cuckooRing.h"
#include "cuckooRingS.h"
#include "smartCuckoo.h"
#include "cuckooFilter.h"
#include "dynamicCuckoo.h"
#include "hash/hash_function.h"

#include <vector>
#include <string>
#include <cstring>
#include <time.h>
#include <algorithm>
#include <set>
using namespace std;
const int max_num = 1<<17; // equal to 1<<15 * 4
const int Len = 4;
char filename[4][100] = {"dat/0.dat", "dat/1.dat", "dat/2.dat", "dat/3.dat"};
string ele[max_num]; // store different string
set<string> s;
void basicTest(int cnt)
{
    dynamicCuckoo d(1<<15, 4, BOB1, BOB2);
    int len=60000;
    cuckoo *cR = new cuckooRingS(len, 4, BOB1, BOB2, BOB3);
    int insertNum = 0;
    int existNum1 = 0;
    int existNum2 = 0;
    for(int i = 0; i < cnt; ++i)
    {
        if(cR->lookup(ele[i]))
            printf("fingerprints collide!");
        if(cR->insert(ele[i]))
            insertNum++;
        else printf("insert failure!\n");
    }
    for(int i = 0; i < cnt; ++i)
        if(cR->lookup(ele[i]))
            existNum1++;
        else
            printf("element(%d) not found.\n",ele[i]);
    cR->resize(len * 2);
    for(int i = 0; i < cnt; ++i)
        if(cR->lookup(ele[i]))
            existNum2++;
    printf("basic test:cnt:%d, insert:%d, lookup1:%d, lookup2:%d\n",
        cnt, insertNum, existNum1, existNum2);
}
int main(int argc, char ** argv){

// read
#if 1
    FILE *fin = NULL;
    int cnt = 0;
    char five_tuple[13];
    for(int i = 0; i < 2; ++i){
        fin = fopen(filename[i], "rb");
        while(fread((void *)five_tuple, sizeof(char), 13, fin) == 13){
            string str = string(five_tuple, Len);
            if(!s.count(str)){ // assure difference
                ele[cnt++] = string(five_tuple, Len);
                if(cnt == max_num)
                    break;
            	s.insert(str);
            }
        }
        fclose(fin);
    }
#endif
// generate synthetic dataset
#if 0
    srand(time(NULL));
    unsigned char ch[4];
    int cnt = 0;
    for(;;){
    	for(int i = 0; i < 4; ++i)
    		ch[i] = rand() % 256;
    	string str((char *)ch);
    	if(!s.count(str)){
    		ele[cnt++] = str;
    		s.insert(str);
    		if(cnt == (int)((double)max_num * 1))
    			break;
    	}
    }
#endif


    basicTest(cnt);

    cout << max_num << ' ' << cnt << '\n';


    cuckoo *sC = new smartCuckoo(1<<15, 4, BOB1, BOB2);
    FILE *wf = fopen("out.txt", "a");

    clock_t time1, time2;
    int insert_success_num = 0;
    int exist_num = 0;
    long long resns;
    double ave;
    time1 = clock();
    insert_success_num = 0;
    for(int i = 0; i < cnt; ++i)
        if(sC->insert(ele[i]))
            insert_success_num++;
    time2 = clock();
    for(int i = cnt / 2 + 1; i < cnt; ++i)
        if(sC->lookup(ele[i]))
            exist_num++;
    ave = (time2 - time1) / (double)insert_success_num;
#if 0
// test smartCuckoo insert time
    fprintf(wf, "time cost = %.6lf\n", (double)time2 - time1);
    fprintf(wf, "load ratio = %.6lf\n", (double)insert_success_num / cnt);
    fprintf(wf, "average time of each insert(smartCuckoo): %.6lf ms\n", ave);
    fprintf(wf, "throughput of insert(smartCuckoo)%.6lf Kips\n", 1.0 / ave);
#endif
#if 0
// test FP
    fprintf(wf, "FP of (smartCuckoo) %.6lf\n", (double)2 * exist_num / cnt);
#endif
#if 0
// test memory_access_num
    fprintf(wf, "memory_access_num of(smartCuckoo) is %d\n", sC->Get_Memory_Access_Num());
    fprintf(wf, "hop_num of(smartCuckoo) is %d\n", sC->Get_Hop_Num());

#endif
// test smartCuckoo load ratio
#if 0
    fprintf(wf, "load ratio of smartCuckoo :%.6lf%\n", (double)100.0 * insert_success_num / max_num);
    fprintf(wf, "\n");
#endif

    cuckoo *cR = new cuckooRing(1<<15, 4, BOB1, BOB2, BOB3);
    time1 = clock();
    insert_success_num = 0;
    exist_num = 0;
    for(int i = 0; i < cnt; ++i)
        if(cR->insert(ele[i]))
            insert_success_num++;
    time2 = clock();
    for(int i = cnt / 2 + 1; i < cnt; ++i)
        if(cR->lookup(ele[i]))
            exist_num++;
    ave = (time2 - time1) / (double)insert_success_num; 
#if 0
// test cuckooRing insert time
    fprintf(wf, "time cost = %.6lf\n", (double)time2 - time1);
    fprintf(wf, "load ratio = %.6lf\n", (double)insert_success_num / cnt);
    fprintf(wf, "average time of each insert(cuckooRing): %.6lf ms\n", ave);
    fprintf(wf, "throughput of insert(cuckooRing)%.6lf Kips\n", 1.0 / ave);
#endif
#if 0
// test FP
    fprintf(wf, "FP of (cuckooRing) %.6lf\n", (double)2 * exist_num / cnt);
#endif
#if 0
    // test memory_access_num
    fprintf(wf, "memory_access_num of(cuckooRing) is %d\n", cR->Get_Memory_Access_Num());
    fprintf(wf, "hop_num of(cuckooRing) is %d\n", cR->Get_Hop_Num());

#endif
// test cuckooRing load ratio
#if 0
    fprintf(wf, "load ratio of cuckooRing :%.6lf%\n", (double)100.0 * insert_success_num / max_num);
    fprintf(wf, "\n");
#endif
    cuckoo *cF = new cuckooFilter(1<<15, 4, BOB1, BOB2);
    time1 = clock();
    insert_success_num = 0;
    exist_num = 0;
    for(int i = 0; i < cnt; ++i)
        if(cF->insert(ele[i]))
            insert_success_num++;
    time2 = clock();
    for(int i = cnt / 2 + 1; i < cnt; ++i)
        if(cF->lookup(ele[i]))
            exist_num++;

    ave = (time2 - time1) / (double)insert_success_num;
#if 0
// test cuckooFilter insert time
    fprintf(wf, "time cost = %.6lf\n", (double)time2 - time1);
    fprintf(wf, "load ratio = %.6lf\n", (double)insert_success_num / cnt);
    fprintf(wf, "average time of each insert(CuckooFilter): %.6lf ms\n", ave);
    fprintf(wf, "throughput of insert(CuckooFilter)%.6lf Kips\n", 1.0 / ave);
#endif
#if 0
// test FP
    fprintf(wf, "FP of (cuckooFilter) %.6lf\n", (double)2 * exist_num / cnt);
#endif
#if 0
    // test memory_access_num
    fprintf(wf, "memory_access_num of(cuckooFilter) is %d\n", cF->Get_Memory_Access_Num());
    fprintf(wf, "hop_num of(cuckooFilter) is %d\n", cF->Get_Hop_Num());

#endif
// test cuckooFilter load ratio
#if 0
    fprintf(wf, "load ratio of CuckooFilter :%.6lf%\n", (double)100.0 * insert_success_num / max_num);
    fprintf(wf, "\n");
#endif

#if 0
// test resize speed
// 
    cuckooRingS *cRS = new cuckooRingS(1<<15, 4, BOB1, BOB2, BOB3);
    dynamicCuckoo  *dC = new dynamicCuckoo(1<<15, 4, BOB1, BOB2);

    for(int i = 0; i < cnt; ++i){
        if(!dC->insert(ele[i])){
            cout << "i = " << i;
            break;
        }
        cRS->insert(ele[i]);
    }
    time1 = clock();
    cout << "time1 = " << time1 << ' ';
    cRS->expand();
    time2 = clock();
    cout << "time2 = " << time2 << ' ';
    cout << "time2 - time1 = " << time2 - time1 << ' ';
#endif

// test lookup time
    cuckooRingS *cRS = new cuckooRingS(1<<15, 4, BOB1, BOB2, BOB3);
    for(int i = 0; i < cnt / 2; ++i)
        cRS->insert(ele[i]);
    sort(ele, ele + cnt);
    time1 = clock();
    for(int i = 0; i < cnt; ++i)
        cRS->lookup(ele[i]);
    time2 = clock();

    printf("cnt = %d lookup time(ms) = %d ave lookup time(ms) %.6lf\n", cnt, time2 - time1, (double)(time2 - time1) / cnt);

    fclose(wf);
    delete cR;
    delete sC;
    delete cF;
    delete cRS;
    // delete dC;
    return 0;
}

