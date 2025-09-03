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
/*
线程启动
线程is working
线程取消完成
*/
/*
子线程在打印 “线程 is working” 后，立即被 pthread_setcanceltype 改为异步取消模式。
主线程发送的 pthread_cancel 请求会立即生效（无需等待取消点），子线程在执行 sleep(2) 期间被强制终止，因此不会打印 “after cancel”。

延迟取消（默认）	- 线程收到取消请求后，不会立即终止，而是在执行到取消点时才响应。
- 取消点：sleep、read、pthread_testcancel 等函数。

异步取消	- 线程收到取消请求后，立即终止（无论当前执行到哪行代码，无需等待取消点）。
- 可能在任意指令处被中断。

延迟取消更加安全，能够完整的释放资源，异步取消很紧急
*/

/*
pthread_cancel 是 “喊停的指令”；
pthread_setcanceltype 是 “约定喊停后怎么反应”（等做完手头事，还是立刻停）；  里面这里根据第一个参数指定规则
pthread_testcancel() 是 “主动查岗”（怕没听到喊停，自己多问一句）。
*/