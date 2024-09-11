#include <stdio.h>
#include <string.h>
#include <unistd.h>
void progressBar(){
	/*进度条状态数组*/
	char buff[40];
	/*字符数组清零*/
	memset(buff,'\0',40);
	/*动态标签*/
	char label[] = "\\/\\/";
	for(int i = 0;i < 40;i++){
		printf("[%-39s][%c][%.1f%%]\r",buff,label[i%4],(i+1)*2.5);
		/*刷新缓冲区，先将缓冲区已有的内容打印*/
		fflush(stdout);
		buff[i] = '>';
		/*等待0.1s(100000us)刷新进度条*/
		usleep(100000);
	}
	putchar('\n');
}
int main(){
	progressBar();
	return 0;
}
