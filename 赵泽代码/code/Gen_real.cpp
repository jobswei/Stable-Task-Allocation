#include<stdlib.h>
#include<iostream>
#include<random>
#include<vector>
#include<fstream>
#include<string>
#include<string.h>
#define MAX_WORKERS 50000
#define MAX_TASKS 50000
#define MAX_PRICE 50
#define MAX_TIME 50
#define BOUND 3000

using namespace std;
void Gen(int w_num,int t_num,int bound){
    vector<double> w_x,w_y,t_x,t_y,t_p;
    vector<int> w_t,t_t;
    double temp = 0;   
    int temp_t = 0; 
    default_random_engine w,t;
    uniform_real_distribution<double> gen_x(-74.0233,-73.753712), gen_y(40.60122,40.895357),gen_price(0,MAX_PRICE);
    uniform_int_distribution<int>  gen_time(0,MAX_TIME);
    ofstream worker,task;
    char w_str[8];
    char t_str[8];
    char b_str[8];
    itoa(w_num,w_str,10);
    itoa(t_num,t_str,10);
    itoa(bound,b_str,10);
    char *f_w = (char *)malloc(strlen("worker_") + strlen(w_str) + strlen("_") + strlen(b_str) + strlen(".txt"));
    char *f_t = (char *)malloc(strlen("task_") + strlen(t_str) + strlen("_") +strlen(b_str) + strlen(".txt"));
    sprintf(f_w,"%s%s%s%s%s","worker_",w_str,"_",b_str,".txt");
    sprintf(f_t,"%s%s%s%s%s","task_",t_str,"_",b_str,".txt");
    worker.open(f_w);
    task.open(f_t);
    for(int i = 1;i <= w_num;i++){
        temp = gen_x(w);
        w_x.push_back(temp);
        worker << temp << " ";
        temp = gen_y(w);
        w_y.push_back(temp);    
        worker << temp << " ";
        temp_t = gen_time(w);
        w_t.push_back(temp_t);
        worker << temp_t << endl;
    }
    for(int i = 1;i <= t_num;i++){
        temp = gen_x(t);
        t_x.push_back(temp);
        task << temp << " ";
        temp = gen_y(t);
        t_y.push_back(temp);
        task << temp << " ";
        temp_t = gen_time(t);
        t_t.push_back(temp_t);
        task << temp_t << " ";
        temp = gen_price(t);
        t_p.push_back(gen_price(t));
        task << temp << endl;
    }
    worker.close();
    task.close();
}

void CutWorker(int w_origin,int w_target){
    char w_str[8];
    itoa(w_origin,w_str,10);
    char *f_origin = (char *)malloc(strlen("worker_") + strlen(w_str) + strlen("_0.txt"));
    sprintf(f_origin,"%s%s%s","worker_",w_str,"_0.txt");
    itoa(w_target,w_str,10);
    char *f_target = (char *)malloc(strlen("worker_") + strlen(w_str) + strlen("_0.txt"));
    sprintf(f_target,"%s%s%s","worker_",w_str,"_0.txt");
    string x,y,t;
    ifstream origin(f_origin);
    cout << f_target << endl;
    ofstream target(f_target);
    for(int i = 1;i <= w_target;i++){
        origin >> x >> y >> t;
        target << x << " ";   
        target << y << " ";
        target << t << endl;
    }
    origin.close();
    target.close();
}

int main(){
    /*
    int w_num = 20,t_num = 10,bound = 100;
    cout << "passenger format: latitude, longitude, cur_time, price, tolerable_time\n" << endl;
    
    cout << "driver format:    latitude, longitude, cur_time" << endl;
    
    cout << "intput number of passengers and drivers:" << endl;
    cin >> t_num;
    cin >> w_num;
    Gen(w_num,t_num,bound);
    */
   
    int T[5] = {1000,2000,3000,4000,10000};
    int W[5] = {1000,2000,3000,4000,10000};
    int B[5] = {1000,2000,3000,4000,10000};
    int i = 0;
    cin >> i;
    //Gen(W[i],9615,0);
    CutWorker(10000,5000);
    return 0;
}