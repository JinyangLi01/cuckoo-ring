#include "cuckoo.h"
#include "cuckooRing.h"
#include "smartCuckoo.h"
#include "cuckooFilter.h"
#include "hash/hash_function.h"

#include <vector>
#include <string>
#include <cstring>
#include <time.h>
#include <set>
using namespace std;
const int max_num = 1<<17; // equal to 1<<15 * 4
const int Len = 4;
char filename[4][100] = {"dat/0.dat", "dat/1.dat", "dat/2.dat", "dat/3.dat"};
string ele[max_num]; // store different string
set<string> s;
int main(int argc, char ** argv){
    

// read
    FILE *fin = NULL;
    int cnt = 0;
    char five_tuple[13];
    for(int i = 0; i < 1; ++i){
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
    

    cuckoo *sC = new smartCuckoo(1<<15, 4, BOB1, BOB2);
    FILE *wf = fopen("out.txt", "a");

    clock_t time1, time2;
    int insert_success_num;
    long long resns;
    double ave;
// test smartCuckoo insert time
#if 1
    time1 = clock();
    insert_success_num = 0;
    for(int i = 0; i < max_num; ++i)
        if(sC->insert(ele[i]))
            insert_success_num++;
    time2 = clock();
    ave = (time2 - time1) / (double)insert_success_num;
    fprintf(wf, "time cost = %.6lf\n", (double)time2 - time1);
    fprintf(wf, "insert_success_num = %d\n", insert_success_num);
    fprintf(wf, "average time of each insert(smartCuckoo): %.6lf ms\n", ave);
    fprintf(wf, "throughput of insert(smartCuckoo)%.6lf Kips\n", 1.0 / ave);
#endif
// test smartCuckoo load ratio
#if 1
    fprintf(wf, "load ratio of smartCuckoo :%.6lf%\n", (double)100.0 * insert_success_num / max_num);
    fprintf(wf, "\n");
#endif

    cuckoo *cR = new cuckooRing(1<<15, 4, BOB1, BOB2, BOB3);
// test cuckooRing insert time
#if 1
    time1 = clock();
    insert_success_num = 0;
    for(int i = 0; i < max_num; ++i)
        if(cR->insert(ele[i]))
            insert_success_num++;
    time2 = clock();
    ave = (time2 - time1) / (double)insert_success_num; 
    fprintf(wf, "time cost = %.6lf\n", (double)time2 - time1);
    fprintf(wf, "insert_success_num = %d\n", insert_success_num);
    fprintf(wf, "average time of each insert(cuckooRing): %.6lf ms\n", ave);
    fprintf(wf, "throughput of insert(cuckooRing)%.6lf Kips\n", 1.0 / ave);
#endif
// test cuckooRing load ratio
#if 1
    fprintf(wf, "load ratio of cuckooRing :%.6lf%\n", (double)100.0 * insert_success_num / max_num);
    fprintf(wf, "\n");
#endif

    cuckoo *cF = new cuckooFilter(1<<15, 4, BOB1, BOB2);
// test cuckooFilter insert time
#if 1
    time1 = clock();
    insert_success_num = 0;
    for(int i = 0; i < max_num; ++i)
        if(cF->insert(ele[i]))
            insert_success_num++;
    time2 = clock();
    ave = (time2 - time1) / (double)insert_success_num;
    fprintf(wf, "time cost = %.6lf\n", (double)time2 - time1);
    fprintf(wf, "insert_success_num = %d\n", insert_success_num); 
    fprintf(wf, "average time of each insert(CuckooFilter): %.6lf ms\n", ave);
    fprintf(wf, "throughput of insert(CuckooFilter)%.6lf Kips\n", 1.0 / ave);
#endif
// test cuckooFilter load ratio
#if 1
    fprintf(wf, "load ratio of CuckooFilter :%.6lf%\n", (double)100.0 * insert_success_num / max_num);
    fprintf(wf, "\n");
#endif

    fclose(wf);
    delete cR;
    delete sC;
    delete cF;
    return 0;
}

