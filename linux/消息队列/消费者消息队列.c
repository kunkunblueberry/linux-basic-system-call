#include <time.h>
#include <mqueue.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

int main(int argc,char*argv[]){
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

    char read_buffer[1024];//这里要从控制台写入消息，需要buffer中转
    while(1)//表示一直不断接受消息
    {
        memset(read_buffer,0,1024);
        //ssize_t count=read(STDIN_FILEND,buffer,sizeof(buffer)); //初始化和打开通道
        clock_gettime(0,&time_info);
        time_info.tv_sec+=5;//时间的设置,这里真正表示的是读不到等五秒，否则一直读
    
        if(mq_timedreceive(mqdes,read_buffer,100,NULL,&time_info)==-1){
            perror("read");
        }
        //判断当前是否读取完成
        if(read_buffer[0]==EOF){
            printf("接收到生产者发送的结束信息");
            break;
        }
        //正常读取的消息队列的信息，打印到控制台
        printf("接收到信息%s",read_buffer);
    }
        close(mqdes);  //关闭消息队列描述符
        mq_unlink(mq_name);//关闭声明的队列,只需要执行一次，消费者执行更好
    return 0;
}


//目前为止，上面的结构都不支持双向消息发送，后面的套接字才可以相互通信