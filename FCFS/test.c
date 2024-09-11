#include<stdio.h>
#include<math.h>
#include<string.h>
#include<time.h>
#include<memory.h>
#include<malloc.h>
#include <stdbool.h>
#include<unistd.h>

void fileOutputStream(int t_num,int w_num){
    // 写入match结果
    FILE *fp=NULL;
   
    char * f_name=(char*)malloc(100);
    sprintf(f_name,"match_t%dw%d.txt",t_num,w_num);
    fp=fopen(f_name,"w");
    for(int i=0;i<w_num;i++){
        // printf("%d\n",*(matches+i));
        // fprintf(fp,"%d\n",*(matches+i));
    }
    fclose(fp);
    // free(f_name);
    fp=NULL;
    // 写入运行信息
    f_name=(char*)malloc(strlen("match_t")+6+strlen("w")+6+strlen(".txt"));
    sprintf(f_name,"result_t%dw%d.txt",t_num,w_num);
    fp=fopen(f_name,"w");
    // fprintf(fp,"profit %lf distance %lf runtime %lf memory %d\n ",profit,distance,runtime,cost_rom);
    fclose(fp);
}

int main(){
    int t_num=4000,w_num=3000;
    fileOutputStream(t_num,w_num);
}