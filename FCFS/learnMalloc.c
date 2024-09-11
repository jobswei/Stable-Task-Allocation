#include<stdio.h>
#include<math.h>
#include<memory.h>
#include<malloc.h>
typedef struct Worker{
    int id;
    double x;
    double y;
    double time;
    _Bool isMatched;
} Worker;
int main(){
    int *a;
    a=(int *)malloc(sizeof(int)*4);
    // 按字节初始化。每个字节都初始化为11111111，当然4个字节16个1还是-1，但是如果是其他数就不能保持了
    memset(a,-1,16);
    printf("%d\n",sizeof(a));
    printf("%d\n",*a);
    free(a);
    // 虽然sizeof(a)没有改变（a一直是一个指针），但是输出a指向的值就没有了。因为被释放了
    printf("%d\n",*a);
    printf("%d\n",sizeof(a));

    // 不能用null初始化worker，也不能将work与{}直接用==作比较。只能这样初始化
    Worker w={-1};
    printf("%d %lf\n",w.id,w.x);

    int n=0;
    do {if(n==5) continue;
        printf("%d\n",n);} while (++n<10);
    return 0;

}