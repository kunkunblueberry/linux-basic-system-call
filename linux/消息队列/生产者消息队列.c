#include <time.h>
#include <queue.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

int main(int argc,chgar*argv[]){
    //消息队列的信息
    struct mq_attr attr;
    attr.mq_maxmsg=10;
    attr.mq_msgsize=100;

    attr.mq_flags=0;
    attr.mq_curmsgs=0;
    //声明时间
    struct timespace time_info;

    char *mq_name="/btw_pro";  //打开通道
    mqd_t mqdes=mq_open(mq_name,O_RDWR|O_CREAT,0664,&attr); //mqdes就是文件描述符
    if(mqdes==(mqd_t)-1){
    perror("mqd_open");
    exit(EXIT_FAILURE);
    }

    char buffer[1024];//这里要从控制台写入消息，需要buffer中转
    while(1)//表示一直不断接受消息
    {
        memset(buffer,0,1024);
        ssize_t count=read(STDIN_FILENO,buffer,sizeof(buffer)); //初始化和打开通道
        
        clock_gettime(0,&time_info);
        time_info.tv_sec+=5;//时间的设置
        if(count==-1){
        perror("read");
        continue;
        }
        else if(count==0){  //等于0表示ctrl +d退出控制台
        printf("文件结束读取");
        char eof=EOF;       //EFO就是-1
        if(mq_timesend(mqdes,&eof,1,0,&time_info)==-1){
            perror("time_send");
        }
            break;//终止循环
        }
        else{
        if(mq_timesend(mqdes,buffer,sizeof(buffer),0,&time_info)==-1){
            perror("time_send");
        }
        printf("从命令行接收到了消息,发送出去了");
        }
    }
        close(mqdes);  //关闭消息队列描述符
        //mq_unlink(mq_name);//关闭声明的队列
    return 0;
}