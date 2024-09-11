#include<stdlib.h>
#include<iostream>
#include<random>
#include<vector>
#include<fstream>
#include<string>
#include<string.h>
//#include<kdtree.h>
#define MAX_WORKERS 20000
#define MAX_TASKS 10000
#define MAX_PRICE 50
#define MAX_TIME 50
#define BOUND 28000

using namespace std;
void Gen(int w_num,int t_num){
    vector<double> w_x,w_y,t_x,t_y,t_p;
    vector<int> w_t,t_t;
    int bound = BOUND;
    double temp = 0;   
    int temp_t = 0; 
    default_random_engine w,t;
    uniform_real_distribution<double> gen_coordinate(-bound,bound), gen_price(0,MAX_PRICE);
    uniform_int_distribution<int>  gen_time(0,MAX_TIME);
    ofstream worker,task;
    char w_str[8];
    char t_str[8];
    itoa(w_num,w_str,10);
    itoa(t_num,t_str,10);
    char *f_w = (char *)malloc(strlen("worker_") + strlen(w_str) + strlen(".txt"));
    char *f_t = (char *)malloc(strlen("task_") + strlen(t_str) + strlen(".txt"));
    sprintf(f_w,"%s%s%s","worker_",w_str,".txt");
    sprintf(f_t,"%s%s%s","task_",t_str,".txt");
    worker.open(f_w);
    task.open(f_t);
    for(int i = 1;i <= w_num;i++){
        temp = gen_coordinate(w);
        w_x.push_back(temp);
        worker << temp << " ";
        temp = gen_coordinate(w);
        w_y.push_back(temp);    
        worker << temp << " ";
        temp_t = gen_time(w);
        w_t.push_back(temp_t);
        worker << temp_t << endl;
    }
    for(int i = 1;i <= t_num;i++){
        temp = gen_coordinate(t);
        t_x.push_back(temp);
        task << temp << " ";
        temp = gen_coordinate(t);
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
    char *f_origin = (char *)malloc(strlen("worker_") + strlen(w_str) + strlen(".txt"));
    sprintf(f_origin,"%s%s%s","worker_",w_str,".txt");
    itoa(w_target,w_str,10);
    char *f_target = (char *)malloc(strlen("worker_") + strlen(w_str) + strlen(".txt"));
    sprintf(f_target,"%s%s%s","worker_",w_str,".txt");
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

void CutTask(int t_origin,int t_target){
    char t_str[8];
    itoa(t_origin,t_str,10);
    char *f_origin = (char *)malloc(strlen("task_") + strlen(t_str) + strlen(".txt"));
    sprintf(f_origin,"%s%s%s","task_",t_str,".txt");
    itoa(t_target,t_str,10);
    char *f_target = (char *)malloc(strlen("task_") + strlen(t_str) + strlen(".txt"));
    sprintf(f_target,"%s%s%s","task_",t_str,".txt");
    string x,y,t,p;
    ifstream origin(f_origin);
    cout << f_target << endl;
    ofstream target(f_target);
    for(int i = 1;i <= t_target;i++){
        origin >> x >> y >> t >> p;
        target << x << " ";   
        target << y << " ";
        target << t << " ";
        target << p << endl;
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
    /*
    int T[5] = {1000,2000,3000,4000,10000};
    int W[5] = {1000,2000,3000,4000,10000};
    for(int i = 0;i < 5;i++){
        for(int j = 0;j < 5;j++){
            Gen(W[i],T[j]);
        }
    }
    */
    Gen(20000,10000);
    for(int i = 1;i <= 10;i++){
        CutTask(10000,i*1000);
        CutWorker(2000*i,5000);
    }
    return 0;
}