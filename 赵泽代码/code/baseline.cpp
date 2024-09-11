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

using namespace std;

const int N = 100000;
const double thres = 1000;
const int d_batch = 1;
const int t_batch = 1;

/*
void showMemoryInfo(void)
    {
    HANDLE handle=GetCurrentProcess();
    PROCESS_MEMORY_COUNTERS pmc;
    GetProcessMemoryInfo(handle,&pmc,sizeof(pmc));
    cout<<"used rom:"<<pmc.WorkingSetSize/1000 <<"K/"<<pmc.PeakWorkingSetSize/1000<<"K + "<<pmc.PagefileUsage/1000 <<"K/"<<pmc.PeakPagefileUsage/1000 <<"K"<<endl;
    }
    */
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
    return ((int)(sqrt(sqr(a.x - b.x) + sqr(a.y - b.y))/d_batch))*d_batch;
}

double eu_dist(node& a, node& b){
    return sqrt(sqr(a.x - b.x) + sqr(a.y - b.y));
}

class passenger: public node {
public:
    double price;
    int wait_time = 10;
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
int cost_rom = 0;

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
            if(!use[j] && min_dist > distance ) {
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
    //cout << start_rom << endl;
    profit = 0;
    char w_str[8];
    char t_str[8];
    itoa(w_num,w_str,10);
    itoa(t_num,t_str,10);
    char *f_w = (char *)malloc(strlen("worker_") + strlen(w_str) + strlen(".txt"));
    char *f_t = (char *)malloc(strlen("task_") + strlen(t_str) + strlen(".txt"));
    sprintf(f_w,"%s%s%s","worker_",w_str,".txt");
    sprintf(f_t,"%s%s%s","task_",t_str,".txt");
    int n = 0;int m = 0;
    string x,y,t,price;
    ifstream worker(f_w),task(f_t);
    while(worker >> x >> y >> t) {
        istringstream s1(x), s2(y), s3(t);
        d[n].i = n + 1; 
        s1 >> d[n].x; s2 >> d[n].y; s3 >> d[n++].time;

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
    cost_rom = end_rom - start_rom;
    ofstream match;
    match.open("match_baseline.txt");
    match << "( passenger, driver )" << endl;
    for(int i = 0; i < n; ++i) {
        if(Match[p[i].i] > 0) {
            match << "( " << i + 1 << ", " << Match[i] << ")" << endl;
            //cout << i << endl;//
            profit += p[i].price;
        }
    }
    /*
    cout << profit << " ";
    cout << (double)(end_time - start_time) / CLOCKS_PER_SEC << " ";
    cout << cost_rom << endl;
    ofstream baseline;
    baseline.open("baseline.txt",ios::app);
    baseline << profit << " " << (double)(end_time - start_time) / CLOCKS_PER_SEC << " " << cost_rom << endl;
    baseline.close();
    */
    /*
    cout << "time: " << (double)(end_time - start_time) / CLOCKS_PER_SEC << " s"  << endl;
    cout << "Overall Profit: " << profit << endl;
    cout << endl;
    */
   return (double)(end_time - start_time) / CLOCKS_PER_SEC;
}

int main(){
    int T[5] = {1000,2000,3000,4000,10000};
    int W[5] = {1000,2000,3000,4000,10000};
    int i = 0;
    double t = 0;
    int j = 0;
    int repeat = 20;
    
    int T_num = 0, W_num = 0;
    cin >> T_num >> W_num;
    baseline(T_num,W_num);

    int temp_rom = cost_rom;
    repeat++;
    while(j++ < repeat){
        t += baseline(T_num,W_num);
        }

    //t = baseline(T_num,W_num);
    cout << profit << " ";
    cout << t << " ";
    cout << temp_rom << endl;
    ofstream baseline;
    baseline.open("baseline.txt",ios::app);
    baseline << thres << " " << d_batch << " " << T_num << " " << W_num << " " << profit << " " << t/j << " " << temp_rom << endl;
    baseline.close();
    /*
    for(int i = 1; i < 3;i++){
       baseline(T_num,W_num);
   }
   
    baseline(T_num,W_num);
    repeat++;
    while(j++ < repeat){
        t += baseline(T_num,W_num);
        }
        
    cout << profit << " ";
    cout << t/j << " ";
    cout << cost_rom << endl;
    */
    /*
    for(int i = 1; i < 3;i++){
       baseline(T[2],W[2]);
   }
    baseline(T[2],W[2]);
    repeat++;
    while(j++ < repeat){
        t += baseline(T[2],W[2]);
    }
    cout << profit << " ";
    cout << t/j << " ";
    cout << cost_rom << endl;
    */
    /*
    cin >> i >> j;
    baseline(T[i],T[j]);
    ofstream baseline;
    baseline.open("baseline.txt",ios::app);
    baseline << cost_rom << endl;
    baseline.close();
    cout << "T: " << T[i] << " W: " << W[j] << endl; 
    cout << cost_rom << endl;
    */

    /*
    while(j++ < repeat){
        t += baseline(T[2],W[2]);
    }
    ofstream baseline;
    baseline.open("baseline.txt",ios::app);
    baseline << profit << " " << t/j << " " << cost_rom << endl;
    baseline.close();
    cout << "T: " << T[2] << " W: " << W[2] << endl; 
    cout << profit << " ";
    cout << t/j << " ";
    cout << cost_rom << endl;
    */

    /*
    for(int i = 0;i < 5;i++){
        j = 1;
        t = 0;
        t += baseline(T[i],W[i]);
        while(j++ < repeat){
            t += baseline(T[i],W[i]);
        }
        ofstream baseline;
        baseline.open("baseline.txt",ios::app);
        baseline << profit << " " << t/j << " " << cost_rom << endl;
        baseline.close();
        cout << "T: " << T[i] << " W: " << W[i] << endl; 
        cout << profit << " ";
        cout << t/j << " ";
        cout << cost_rom << endl;
    }
    
    for(int i = 0;i < 5;i++){
        j = 0;
        t = 0;
        while(j++ < repeat){
            t += baseline(T[i],W[2]);
        }
        ofstream baseline;
        baseline.open("baseline.txt",ios::app);
        baseline << profit << " " << t/j << " " << cost_rom << endl;
        baseline.close();
        cout << "T: " << T[i] << " W: " << W[2] << endl; 
        cout << profit << " ";
        cout << t/j << " ";
        cout << cost_rom << endl;
    }
    for(int i = 0;i < 5;i++){
        j = 0;
        t = 0;
        while(j++ < repeat){
            t += baseline(T[2],W[i]);
        }
        ofstream baseline;
        baseline.open("baseline.txt",ios::app);
        baseline << profit << " " << t/j << " " << cost_rom << endl;
        baseline.close();
        cout << "T: " << T[2] << " W: " << W[i] << endl; 
        cout << profit << " ";
        cout << t/j << " ";
        cout << cost_rom << endl;
    }
    */
    return 0;
}
