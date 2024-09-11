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

using namespace std;

const int N = 1000000;
const double thres = 200;
const int d_batch = 80;

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
    //cout <<((int)(sqrt(sqr(a.x - b.x) + sqr(a.y - b.y))/d_batch))*d_batch <<endl;
    return ((int)(sqrt(sqr(a.x - b.x) + sqr(a.y - b.y))/d_batch))*d_batch;
}

class passenger: public node {
public:
    double price;
    int wait_time;
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
                if(distance <= min_dist){
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
                    if(distance == min_dist && j != id && W_M[j] == -1){
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

int main() {
/*
    scanf("%d%d", &n, &m);
    for(int i = 0; i < n; ++i) {
        p[i].input(i);
    }
    for(int i = 0; i < m; ++i) {
        d[i].input(i);
    }

*/
    n = 0;m = 0;
    string x,y,t,price;
    ifstream worker("worker.txt"),task("task.txt");
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
    int batch_size = 30, ptr_p = 0, ptr_d = 0;
    //cout << "st_time: " << st_time << endl;
    //cout << "ed_time: " << ed_time << endl;
    for(int T = st_time; T <= ed_time; T += batch_size) {
        curr_n = curr_m = 0;
        while(ptr_p < n && intime(p[ptr_p].time, T, batch_size))
            curr_p[curr_n++] = p[ptr_p++];
        while(ptr_d < m && intime(d[ptr_d].time, T, batch_size))
            curr_d[curr_m++] = d[ptr_d++]; 
        match(curr_p, curr_n, curr_d, curr_m);
    }
    end_time = clock();
    ofstream match;
    match.open("match.txt");
    match << "( passenger, driver )" << endl;
    for(int i = 0; i < n; ++i) {
        if(Match[p[i].i] > 0) {
            match << "( " << p[i].i << ", " << Match[p[i].i] << ")" << endl;
            profit += p[i].price;
        }
    }

    cout << "time: " << (double)(end_time - start_time) / CLOCKS_PER_SEC << " s"  << endl;
    cout << "Overall Profit: " << profit;
    return 0;
}