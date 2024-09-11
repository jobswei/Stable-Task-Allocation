#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<string.h>
#include<time.h>
#include<memory.h>
#include<malloc.h>
#include <stdbool.h>
#include<unistd.h>
// #include<winnt.h>
// #include<psapi.h>
// #include<windows.h>
#define N 100000
#define LPCWSTR LPCTSTR
#ifndef UNICODE
#define UNICODE
#endif

// 不考虑容忍时间
typedef struct Worker{
    int id;
    double x;
    double y;
    double time;
    _Bool isMatched;
} Worker;
typedef struct Task
{
    int id;
    double x;
    double y;
    double time;
    double price;
} Task;

static Worker * workers;
static Task * tasks;
static int * matches;
static double profit=0;
static double distance=0;
static double runtime=0;
static int cost_rom=0;
double dist(Task t,Worker w){
    return sqrt(pow(w.x-t.x,2)+pow(w.y-t.y,2));
}
_Bool timeOK(Task t,Worker w){
    return (t.time>=w.time);// t比w后出现
}
void freeall(){
    free(tasks);
    free(workers);
    free(matches);
    profit=0;
    distance=0;
    runtime=0;
    cost_rom=0;
}
void fileInputStream(int t_num,int w_num){
    /* 读入tasks和workers */
    tasks=(Task*)malloc(sizeof(Task)*(t_num+1));
    workers=(Worker*)malloc(sizeof(Worker)*(w_num+1));

    FILE *fp1=NULL,*fp2=NULL;
    char*f_t=(char*)malloc(strlen("task")+6+strlen(".txt"));
    char*f_w=(char*)malloc(strlen("worker_")+6+strlen(".txt"));
    sprintf(f_t,"%s%d%s","task_",t_num,".txt");
    sprintf(f_w,"%s%d%s","worker_",w_num,".txt");
    fp1=fopen(f_t,"r");
    fp2=fopen(f_w,"r");
    for(int i=0;i<t_num;i++){
        double x,y,time,price;
        fscanf(fp1,"%lf %lf %lf %lf\n",&x,&y,&time,&price);
        Task t={i,x,y,time,price};
        *(tasks+i)=t;
    }
    for(int i=0;i<w_num;i++){
        double x,y,time;
        fscanf(fp2,"%lf %lf %lf\n",&x,&y,&time);
        Worker w={i,x,y,time,false};
        *(workers+i)=w;
    }
    fclose(fp1);
    fclose(fp2);
}
void fileOutputStream(int t_num,int w_num){
    // 写入match结果
    FILE *fp=NULL;
    char * f_name=(char*)malloc(strlen("match_t")+6+strlen("w")+6+strlen(".txt"));
    sprintf(f_name,"match_t%dw%d.txt",t_num,w_num);
    fp=fopen(f_name,"w");
    for(int i=0;i<w_num;i++){
        // printf("%d\n",*(matches+i));
        fprintf(fp,"%d\n",*(matches+i));
    }
    fclose(fp);
    // free(f_name);
    fp=NULL;
    // 写入运行信息
    f_name=(char*)malloc(strlen("match_t")+6+strlen("w")+6+strlen(".txt"));
    sprintf(f_name,"result_t%dw%d.txt",t_num,w_num);
    fp=fopen(f_name,"w");
    fprintf(fp,"profit %lf distance %lf runtime %lf memory %d\n ",profit,distance,runtime,cost_rom);
    fclose(fp);

    fp=fopen("FCFS.txt","a+");
    fprintf(fp,"t_num %d w_num %d profit %lf distance %lf runtime %lf memory %d\n",t_num,w_num,profit,distance,runtime,cost_rom);
    fclose(fp);
}
void match(Task *tasks,int t_num,Worker *workers,int w_num){
    /*输入的task和work都是经过按时间排序处理的*/

    // 人多车少，车一定是都被匹配的，所以说按车的数量来
    matches=(int*)malloc(sizeof(int)*w_num);
    memset(matches,-1,sizeof(int)*w_num);

    // HANDLE handle = GetCurrentProcess();
    // PROCESS_MEMORY_COUNTERS pmc;
    // SIZE_T bytesRead;
    // ReadProcessMemory(handle, &pmc, (LPVOID)0x1000,sizeof(pmc),&bytesRead);
    // int start_rom=pmc.WorkingSetSize;
    clock_t start_time,end_time;
    start_time=clock();

    int m=0;
    do{
        Task t=*(tasks+m);
        Worker w={-1};
        double dis=INFINITY;

        int n=0;
        do{
            Worker w_new=*(workers+n);
            if(w_new.isMatched==true) continue;
            if(!timeOK(t,w_new)) break;// 因为是按时间排序过的，所以只要一个不行，后面的都不行了
            if(dis<=dist(t,w_new))continue;
            if(w.id!=-1) {
                w.isMatched=false;
                *(workers+w.id)=w;
                }
            w=w_new;// 这里有错误要改，因为w是独异于列表里的值得一个新的东西，二者不会同步改变
            w.isMatched=true;
            *(workers+w.id)=w;
            dis=dist(t,w);
        }while(++n<w_num);
        if(w.id==-1) continue; // 没匹配上
        *(matches+w.id)=t.id;
        // printf("%d\n",*(matches+1));
        distance+=dis;
        profit+=t.price;
    }while(++m<t_num);

    end_time=clock();
    runtime=(double)(end_time - start_time) / CLOCKS_PER_SEC;
    // SIZE_T bytesRead;
    // ReadProcessMemory(handle, &pmc, (LPVOID)0x1000,sizeof(pmc),&bytesRead);
    // int end_rom = pmc.WorkingSetSize;
    // cost_rom = end_rom - start_rom;
}
void bubbleSort(Task* tasks,int t_num,Worker* workers, int w_num) {
    int i, j;
    for (i = 0; i < t_num-1; i++) {
        for (j = 0; j < t_num-i-1; j++) {
            Task tj=*(tasks+j);
            Task tj1=*(tasks+j+1);
            if (tj.time > tj1.time) {
                *(tasks+j) = tj1;
                *(tasks+j+1) = tj;
            }
        }
    }
    for (i = 0; i < w_num-1; i++) {
        for (j = 0; j < w_num-i-1; j++) {
            Worker wj=*(workers+j);
            Worker wj1=*(workers+j+1);
            if (wj.time > wj1.time) {
                *(workers+j) = wj1;
                *(workers+j+1) = wj;
            }
        }
    }
}

void FCFS(int t_num,int w_num){
    fileInputStream(t_num,w_num);
    bubbleSort(tasks,t_num,workers,w_num);
    match(tasks,t_num,workers,w_num);
    fileOutputStream(t_num,w_num);
    printf("profit %lf distance %lf runtime %lf memory %d\n ",profit,distance,runtime,cost_rom);
    // freeall();
}
int main(){
    chdir("D://About_coding/py/car/FCFS");
    int i=0,j=0;
    for(i=3;i<4;i++){
        for(j=5;j<5+1;j++){
            int t_num=i*1000;
            int w_num=j*1000;
            printf("t %d w %d begin!\n",t_num,w_num);
            FCFS(t_num,w_num);
            printf("OK!\n");
        }
    }
    return 0;
}