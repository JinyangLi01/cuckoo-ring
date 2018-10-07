#include "cuckoo.h"
#include "cuckooRing.h"
#include "hash/hash_function.h"

#include <vector>
#include <string>
#include <cstring>
#include <time.h>
#include <set>
using namespace std;
const int max_num = (int)(0.2*(1<<16)); // equal to 1<<14 * 4
const int Len = 4;
char filename[4][100] = {"dat/0.dat", "dat/1.dat", "dat/2.dat", "dat/3.dat"};
string ele[max_num]; // store different string
set<string> s;
int main(int argc, char ** argv){
    

// read
// assure cnt == 1<<16
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
            }
            s.insert(str);
        }
        fclose(fin);
    }


    FILE *wf = fopen("out.txt", "a");

    clock_t time1, time2;
    int insert_success_num;
    long long resns;
    double ave;

    cuckoo *cR = new cuckooRing(1<<14, 4, BOB1, BOB2, BOB3);
// test cuckooRing insert time
// #if 1
    insert_success_num = 0;
    for(int i = 0; i < max_num; ++i)
        if(cR->insert(ele[i]))
            insert_success_num++;
    cout << insert_success_num << ' ' << max_num << '\n';
    cout << (double)insert_success_num / max_num << '\n';
    delete cR;

//二分找到最小的L2
    double L, R, mid;
    L = 0.2;
    R = 1;

    
    while(R - L >= 0.001){
        mid = (L + R) / 2;
        // create and insert
        cR = new cuckooRing(1<<14, 4, BOB1, BOB2, BOB3);
        for(int i = 0; i < max_num; ++i)
            cR->insert(ele[i]);

        if(cR->resize((int)(mid * (1<<16)))){
            R = mid;
        }
        else L = mid;
        delete cR;
        cout << "OK" << '\n';
    }
    fprintf(wf, "max load ratio to L2 = %d is %.6lf\n", (int)(mid * (1 << 16)), (double)0.2/mid);

// create and insert
    cR = new cuckooRing(1<<14, 4, BOB1, BOB2, BOB3);
    for(int i = 0; i < max_num; ++i)
        cR->insert(ele[i]);  
// test time of compress and expand
    int L2 = (int)(0.9 * (1<<16)); 
    time1 = clock();
    if(!cR->resize(L2))
        cout << "L2 is too small";
    time2 = clock();
    fprintf(wf, "compress to L2 = %d cost %.6lf ms\n", L2, (double)time2 - time1);
    delete cR;

// create and insert
    cR = new cuckooRing(1<<14, 4, BOB1, BOB2, BOB3);
    for(int i = 0; i < max_num; ++i)
        cR->insert(ele[i]);     
    L2 = (int)(1.2 * (1<<16));
    time1 = clock();
    cR->resize(L2);
    time2 = clock();
    fprintf(wf, "expand to L2 = %d cost %.6lf ms\n", L2, (double)time2 - time1);
    delete cR;

// lazy update 
    // create and insert
    // cR = new cuckooRing(1<<14, 4, BOB1, BOB2, BOB3);
    // for(int i = 0; i < max_num; ++i)
    //     cR->insert(ele[i]); 
    // int L2 = (int)(2 * (1<<16));
    // time1 = clock();
    // cR->resize(L2);
    // time2 = clock();
    // fprintf(wf, "double the memory (lazy update) cost %.6lf ms\n", (double)time2 - time1);
    // delete cR;

    fclose(wf);
    return 0;
}

