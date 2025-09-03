#include<pthread.h>
#include<stdlib.h>
#include<stdio.h>
#include<unistd.h>
#include<math.h>
#include<string.h>

void*task(void *arg){
    printf("线程启动\n");
    //现在是默认的延迟取消类型
    printf("线程is working\n");
    pthread_setcanceltype(PTHREAD_CANCEL_DISABLE,NULL);
//这里是禁用取消
//执行到下面，继续执行，因为之前cancel传输进来取消的时机过了
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS,NULL);
    sleep(2);
    printf("after cancel\n");
}

int main(int argc,char*argv[]){
    pthread_t pid;
    pthread_create(&pid,NULL,task,NULL);
 
    //取消子线程
    if(pthread_cancel(pid)!=0){
        perror("cancel");
    }
    //仅仅发送停止的消息
    void *res;
    pthread_join(pid,&res);
    //join会挂起，一直等待子线程结束

    if(res==PTHREAD_CANCELED)
    {
        printf("线程取消完成");
    }else{
        printf("线程没有被取消%ld",(long)res);
    }
}