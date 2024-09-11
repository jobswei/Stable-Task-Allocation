#include<stdlib.h>
#include<math.h>
#include<stdio.h>
#include<iostream>
#include<algorithm>
#include<string>
#include<iostream>
#include<fstream>
#include<sstream>       
#include<string.h>
#include<ctime>
#include<windows.h>
#include<psapi.h>
#include <fstream>

#pragma comment(lib,"psapi.lib")
#define PI 3.141592653589793238
#define R_EARTH 6371.004 * 1000

using namespace std;

const int N = 1000000;
const double thres = 2000;
const int d_batch = 1000;
const int t_batch = 1;

void showMemoryInfo(void)
    {
    HANDLE handle=GetCurrentProcess();
    PROCESS_MEMORY_COUNTERS pmc;
    GetProcessMemoryInfo(handle,&pmc,sizeof(pmc));
    cout<<"used rom:"<<pmc.WorkingSetSize/1000 <<"K/"<<pmc.PeakWorkingSetSize/1000<<"K + "<<pmc.PagefileUsage/1000 <<"K/"<<pmc.PeakPagefileUsage/1000 <<"K"<<endl;
    }
    
double sqr(double a) {
    return a * a;
}

class node{
public:
    double x, y;
    int i, time;
};

double dist(node& a, node& b) {
    //cout <<((int)(sqrt(sqr(a.x - b.x) + sqr(a.y - b.y))/d_batch))*d_batch <<endl;
    double d = acos((sin(a.y*PI/180)*sin(b.y*PI/180))+(cos(a.y*PI/180)*cos(b.y*PI/180)*cos((a.x-b.x)*PI/180)))*R_EARTH;
    //cout << d << endl;/////
    return ((int)(d/d_batch))*d_batch;
}

double eu_dist(node& a, node& b){
    double d = acos((sin(a.y*PI/180)*sin(b.y*PI/180))+(cos(a.y*PI/180)*cos(b.y*PI/180)*cos((a.x-b.x)*PI/180)))*R_EARTH;
    return d;
}

class passenger: public node {
public:
    double price;
    int wait_time = 100;
    void input(int id) {
        i = id;
        scanf("%lf%lf%d%lf%d",&x, &y, &time, &price, &wait_time);
    }
} p[N], curr_p[N];

class driver: public node {
public:
    void input(int id) {
        i = id;
        scanf("%lf%lf%d", &x, &y, &time);
    }
} d[N], curr_d[N];

bool cmp_t(const node& a, const node& b) {
    return a.time < b.time;
}

bool cmp_p(const passenger& a, const passenger& b) {
    return a.price > b.price;
}

int n, m, curr_n, curr_m;
int Match[N];
double profit = 0;

bool intime(int x, int st, int len) {
    return x >= st && x < st + len;
}

void match(passenger* P, int n, driver* D, int m) {
    static bool use[N];
    memset(use, 0, sizeof(bool) * (m + 2));
    sort(P, P + n, cmp_p);
    for(int i = 0; i < n; ++i) {
        double min_dist = thres;
        int id = -1;
        for(int j = 0; j < m; ++j) {
            double distance = dist(P[i], D[j]);
            if(!use[j] && min_dist > distance) {
                id = j;
                min_dist = distance;
            }     
        }
        if(id < 0) continue;
        Match[P[i].i] = D[id].i;
        //cout << i << endl;/////
        use[id] = true;
        //profit += P[i].price;
    }
}

double baseline(int t_num, int w_num) {
    HANDLE handle = GetCurrentProcess();
    PROCESS_MEMORY_COUNTERS pmc;
    GetProcessMemoryInfo(handle, &pmc, sizeof(pmc));
    int start_rom = pmc.WorkingSetSize;
    profit = 0;
    
    char w_str[8];
    itoa(w_num,w_str,10);
    /*
    char t_str[8];
    char b_str[8];
    itoa(w_num,w_str,10);
    itoa(t_num,t_str,10);
    itoa(bound,b_str,10);
    char *f_w = (char *)malloc(strlen("worker_") + strlen(w_str) + strlen("_") + strlen(b_str) + strlen(".txt"));
    char *f_t = (char *)malloc(strlen("task_") + strlen(t_str) + strlen("_") +strlen(b_str) + strlen(".txt"));
    sprintf(f_w,"%s%s%s%s%s","worker_",w_str,"_",b_str,".txt");
    sprintf(f_t,"%s%s%s%s%s","task_",t_str,"_",b_str,".txt");
    */
    char *f_w = (char *)malloc(strlen("worker_") + strlen(w_str) + strlen("_0.txt"));
    char* f_t = (char *)malloc(strlen("task_real.txt"));
    sprintf(f_w,"%s%s%s","worker_",w_str,"_0.txt");
    sprintf(f_t,"%s","task_real.txt");
    n = 0;m = 0;
    //cout << "!" << endl;///
    string x,y,t,price;
    ifstream worker(f_w),task(f_t);
    while(worker >> x >> y >> t) {
        //cout << "!" << endl;///
        istringstream s1(x), s2(y), s3(t);
        d[n].i = n + 1; 
        s1 >> d[n].x; s2 >> d[n].y; s3 >> d[n++].time;
        //s1 >> d[n].x; s2 >> d[n].y; 0 >> d[n++].time;
    }
    while(task >> x >> y >> t >> price) {
        istringstream s1(x), s2(y), s3(t), s4(price);
        p[m].i = m + 1;
        s1 >> p[m].x; s2 >> p[m].y; s3 >> p[m].time; s4 >> p[m++].price;
    }
    clock_t start_time, end_time;
    start_time = clock();
    memset(Match, -1, sizeof(Match));
    sort(p, p + n, cmp_t);
    sort(d, d + m, cmp_t);
    int st_time = min(p[0].time, d[0].time),
        ed_time = max(p[n - 1].time, d[m - 1].time);
    int batch_size = t_batch, ptr_p = 0, ptr_d = 0;
    //cout << "st_time: " << st_time << endl;
    //cout << "ed_time: " << ed_time << endl;

    for(int T = st_time; T <= ed_time; T += batch_size) {
        curr_n = curr_m = 0;
        /*
        while(ptr_p < n && intime(p[ptr_p].time, T, batch_size))
            curr_p[curr_n++] = p[ptr_p++];
        while(ptr_d < m && intime(d[ptr_d].time, T, batch_size))
            curr_d[curr_m++] = d[ptr_d++];
            */
        while(ptr_p < n && intime(T, p[ptr_p].time, p[ptr_p].wait_time))
            curr_p[curr_n++] = p[ptr_p++];
        while(ptr_d < m && d[ptr_d].time < T + batch_size)
            curr_d[curr_m++] = d[ptr_d++];
        match(curr_p, curr_n, curr_d, curr_m);
    }
    
    end_time = clock();
    GetProcessMemoryInfo(handle, &pmc, sizeof(pmc));
    int end_rom = pmc.WorkingSetSize;
    int cost_rom = end_rom - start_rom;
    
    ofstream match;
    match.open("match_baseline1.txt");
    match << "( passenger, driver )" << endl;
    for(int i = 0; i < n; ++i) {
        if(Match[p[i].i] > 0) {
            match << "( " << i + 1 << ", " << Match[i] << ")" << endl;
            //cout << i << endl;//
            profit += p[i].price;
        }
    
    }
    cout << profit << " ";
    cout << (double)(end_time - start_time) / CLOCKS_PER_SEC << " ";
    cout << cost_rom << endl;
    ofstream baseline;
    baseline.open("baseline.txt",ios::app);
    //baseline << profit << " " << (double)(end_time - start_time) / CLOCKS_PER_SEC << " " << cost_rom << endl;
    baseline.close();
    return (double)(end_time - start_time) / CLOCKS_PER_SEC;
    
    cout << "time: " << (double)(end_time - start_time) / CLOCKS_PER_SEC << " s"  << endl;
    cout << "Overall Profit: " << profit << endl;
    cout << endl;
    
}

int main(){
    int T[5] = {1000,2000,3000,4000,10000};
    int W[5] = {1000,2000,3000,4000,10000};
    int B[5] = {1000,2000,3000,4000,10000};
    int i = 0;
    cin >> i ;
    int j = 0;
    int repeat = 1;
    double t = 0;
    while(j++ < repeat){
        t += baseline(9615,i);
    }
    /*
    ofstream baseline;
    baseline.open("baseline_real.txt",ios::app);
    baseline << thres << " " << d_batch << " " << T_num << " " << W_num << " " << profit << " " << t/j << " " << cost_rom << endl;
    baseline.close();
    */
    //cout << t/j << endl;
    /*
    for(int i = 0;i < 5;i++){
        for(int j = 0;j < 5;j++){
            cout << "baseline: " << W[i] << ',' << T[j] << ',' << B[2] << ':' << "Completed" << endl;
            baseline(W[i],T[j],B[2]);
        }
    }
    */
   /*
    for(int i = 0;i < 5;i++){
        //cout << "baseline: " << W[i] << ',' << T[i] << ',' << B[2] << ':' << "Completed" << endl;
        baseline(W[i],T[i],B[2]);
    }
    /*
    for(int i = 0;i < 5;i++){
        //cout << "baseline: " << W[i] << ',' << T[2] << ',' << B[2] << ':' << "Completed" << endl;
        baseline(W[i],T[2],B[2]);
    }
    for(int i = 0;i < 5;i++){
        //cout << "baseline: " << W[2] << ',' << T[i] << ',' << B[2] << ':' << "Completed" << endl;
        baseline(W[2],T[i],B[2]);
    }
    /*
    for(int i = 0;i < 5;i++){
        //cout << "baseline: " << W[2] << ',' << T[2] << ',' << B[i] << ':' << "Completed" << endl;
        baseline(W[2],T[2],B[i]);
    }
    */
    
    return 0;
}