#include<stdlib.h>
#include<iostream>
#include<random>
#include<vector>
#include<fstream>
#define MAX_WORKERS 50000
#define MAX_TASKS 50000
#define MAX_PRICE 100
#define MAX_TIME 5
#define BOUND 4000

using namespace std;
void Gen(int w_num,int t_num,int price_upper_bound){
    vector<double> w_x,w_y,t_x,t_y,t_p;
    vector<int> w_t,t_t;
    double temp = 0;   
    int temp_t = 0; 
    default_random_engine w,t;
    uniform_real_distribution<double> gen_coordinate(-BOUND,BOUND), gen_price(0,MAX_PRICE);
    uniform_int_distribution<int>  gen_time(0,MAX_TIME);
    ofstream worker,task;
    worker.open("worker.txt");
    task.open("task.txt");
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

int main(){
    int w_num = 20,t_num = 10,price_upper_bound = 100;
    cout << "passenger format: latitude, longitude, cur_time, price, tolerable_time\n" << endl;
    
    cout << "driver format:    latitude, longitude, cur_time" << endl;
    cout << "intput number of passengers and drivers:" << endl;
    cin >> t_num;
    cin >> w_num;
    Gen(w_num,t_num,price_upper_bound);
    
   /*
    int T[5] = {5000,10000,20000,30000,100000};
    int W[5] = {5000,10000,20000,30000,100000};
    int B[5] = {1000,2000,3000,4000,5000};
    for(int i = 0;i < 5;i++){
        for(int j = 0;j < 5;j++){
            for(int k = 0;k < 5;k++){
                cout << W[i] << ',' << T[j] << ',' << B[k] << ':' << "Completed" << endl;
                Gen(W[i],T[j],B[k]);
            }
        }
    }
    */
    return 0;
}