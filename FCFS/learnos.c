#include<stdio.h>
#include<memory.h>
#include<unistd.h>
#include<string.h>

int main(){
    char strpwd[301];
    memset(strpwd,0,sizeof(strpwd));
    getcwd(strpwd,300);
    printf("当前目录是：%s\n",strpwd);
    chdir("D://wechat");
    getcwd(strpwd,300);
    printf("当前目录是：%s\n",strpwd);
    chdir("D://About_coding/py/car/FCFS");
    mkdir("look");
    rmdir("look");

}