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

using namespace std;

const int N = 10000;
const double thres = 3000.0;

double sqr(double a) {
    return a * a;
}

class node{
public:
    double x, y;
    int i, time;
};

double dist(node& a, node& b) {
    return abs(a.x - b.x) + abs(a.y - b.y);
}

class passenger: public node {
public:
    double price;
    int wait_time;
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
        use[id] = true;
        //profit += P[i].price;
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
    while(task >> x >> y >> price >> t) {
        istringstream s1(x), s2(y), s3(price), s4(t);
        p[m].i = m + 1;
        s1 >> p[m].x; s2 >> p[m].y; s3 >> p[m].time; s4 >> p[m++].price;
    }

    memset(Match, -1, sizeof(Match));
    sort(p, p + n, cmp_t);
    sort(d, d + m, cmp_t);
    int st_time = min(p[0].time, d[0].time),
        ed_time = max(p[n - 1].time, d[m - 1].time);
    int batch_size = 10, ptr_p = 0, ptr_d = 0;
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
    ofstream match;
    match.open("match_baseline1.txt");
    match << "( passenger, driver )" << endl;
    for(int i = 0; i < n; ++i) {
        if(Match[i] > 0) {
            match << "( " << i + 1 << ", " << Match[i] << ")" << endl;
            profit += p[i].price;
        }
    
    }
    cout << "Overall Profit: " << profit;
    return 0;
}