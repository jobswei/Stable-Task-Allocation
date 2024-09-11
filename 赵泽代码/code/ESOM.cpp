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

#pragma comment(lib,"psapi.lib")

using namespace std;

const int N = 100000;
const double thres = 400;
const int d_batch = 1;
const int t_batch = 1;

double sqr(double a) {
    return a * a;
}

class node{
public:
    double x, y;
    int i, time;
    bool matching = 0;
};

double dist(node& a, node& b) {
    return ((int)(sqrt(sqr(a.x - b.x) + sqr(a.y - b.y))/d_batch))*d_batch;
}

double eu_dist(node& a, node& b){
    return sqrt(sqr(a.x - b.x) + sqr(a.y - b.y));
}

class passenger: public node {
public:
    double price;
    int wait_time = 10;
    bool f = 0;
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

void match(passenger *P, int n, driver *D, int m) {
    int** Available = new int*[n];
    for(int i = 0; i < n;i++) {
        Available[i] = new int[m]{};
    }
    static int use[N];//与driver匹配的passenger,1,2...
    memset(use, 0, sizeof(int) * (m + 5));
    static int r[N];
    memset(r, 0, sizeof(int) * (n + 5));
    static int W_M[N];
    memset(W_M, -1, sizeof(int) * (m + 5));
    sort(P, P + n, cmp_p);
    double min_dist = thres;
    double distance = thres + 1;
    int id = -1;
    for(int i = 0;i < n && !P[i].f;i++){
        int count = 0;
        while(count < m && Match[P[i].i] == -1){
            count++;
            min_dist = thres;
            distance = 0;
            id = -1;            
            for(int j = 0;j < m;j++){
                if(Available[i][j] != 0){
                    continue;
                }
                distance =  dist(P[i],D[j]);
                if(distance <= min_dist){                                    //XXXXX
                    min_dist = distance;
                    id = j;
                }
            }
            if(id == -1){
                P[i].f = 1;
                break;
            }
            if(W_M[id] == -1){
                Match[P[i].i] = D[id].i;
                W_M[id] == i;
            }
            else{
                int k = W_M[id];
                bool sub = 0;
                min_dist = dist(P[k],D[id]);
                for(int j = 0;j < m;j++){
                    distance = dist(P[k],D[j]);
                    if(distance == min_dist && j != id && W_M[j] == -1){   //XXXXXXX
                    sub = 1;
                    }
                }
                if(sub == 1){
                    Match[P[k].i] = -1;
                    Match[P[i].i] = id;
                    if(P[i].price == P[k].price && r[i] > r[k]){
                        Available[k][id] = 1;
                    }
                    i = k;
                }
                else{
                    Available[i][id] = 1;
                }
            }
        }
        if(Match[P[i].i] == -1){
            if(r[i] == 0){
                r[i] = 1;
                for(int j = 0;j < m;j++){
                    Available[i][j] = 0;
                }
            }
        }
        else{
            P[i].f = 1;
        }
    }
}

double ESOM(int t_num, int w_num) {
    // HANDLE handle = GetCurrentProcess();
    // PROCESS_MEMORY_COUNTERS pmc;
    // GetProcessMemoryInfo(handle, &pmc, sizeof(pmc));
    // int start_rom = pmc.WorkingSetSize;
    profit = 0;
    char w_str[8];
    char t_str[8];
    itoa(w_num,w_str,10);
    itoa(t_num,t_str,10);
    char *f_w = (char *)malloc(strlen("worker_") + strlen(w_str) + strlen(".txt"));
    char *f_t = (char *)malloc(strlen("task_") + strlen(t_str) + strlen(".txt"));
    sprintf(f_w,"%s%s%s","worker_",w_str,".txt");
    sprintf(f_t,"%s%s%s","task_",t_str,".txt");
    int n = 0;
    int m = 0;
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
    time_t start_time, end_time;
    start_time = clock();
    memset(Match, -1, sizeof(Match));
    sort(p, p + n, cmp_t);
    sort(d, d + m, cmp_t);
    int st_time = min(p[0].time, d[0].time),
        ed_time = max(p[n - 1].time, d[m - 1].time);
    int batch_size = t_batch, ptr_p = 0, ptr_d = 0;
    for(int T = st_time; T <= ed_time; T += batch_size) {
        curr_n = curr_m = 0;
        while(ptr_p < n && intime(T, p[ptr_p].time, p[ptr_p].wait_time))
            curr_p[curr_n++] = p[ptr_p++];
        while(ptr_d < m && d[ptr_d].time < T + batch_size)
            curr_d[curr_m++] = d[ptr_d++]; 
        match(curr_p, curr_n, curr_d, curr_m);   
    }
    end_time = clock();
    // GetProcessMemoryInfo(handle, &pmc, sizeof(pmc));
    // int end_rom = pmc.WorkingSetSize;
    // cost_rom = end_rom - start_rom;
    ofstream match;
    match.open("match.txt");
    match << "( passenger, driver )" << endl;
    for(int i = 0; i < n; ++i) {
        if(Match[p[i].i] > 0) {
            match << "( " << p[i].i << ", " << Match[p[i].i] << ")" << endl;
            profit += p[i].price;
        }
    }
    return (double)(end_time - start_time) / CLOCKS_PER_SEC;
}

int main(){
    int i = 0;
    double t = 0;
    int j = 0;
    int repeat = 20;
    int T_num = 0, W_num = 0;
    cin >> T_num >> W_num;
    for(int i = 1; i < 3;i++){
       ESOM(T_num,W_num);
   }
    ESOM(T_num,W_num);
    repeat++;
    while(j++ < repeat){
        t += ESOM(T_num,W_num);
        }
    cout << profit << " ";
    cout << t/j << " ";
    cout << cost_rom << endl;
    ofstream ESOM;
    ESOM.open("ESOM.txt",ios::app);
    ESOM << thres << " " << d_batch << " " << T_num << " " << W_num << " " << profit << " " << t/j << " " << cost_rom << endl;
    ESOM.close();
    return 0;
}