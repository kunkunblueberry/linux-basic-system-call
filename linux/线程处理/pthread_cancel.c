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
    sleep(2);

    //人为调用取消函数
    pthread_testcancel();
    //pthread_testcancel() 所以这个在上面的代码里面作用就是，万一sleep已经做完了，也能够停止
    //就是你完全把这个函数作为一个不存在，他只是看一看主线程有没有发cancel，没有就继续执行下面的printf

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
主线程创建子线程：pthread_create 创建子线程，子线程开始执行 task 函数。
主线程发送取消请求：pthread_cancel(pid) 向子线程发送 “取消请求”（注意：这只是 “请求”，不是立即终止）。
子线程执行逻辑：
打印 “线程启动” 和 “线程 is working”。
调用 sleep(2)（休眠 2 秒）。--这样主线程必定已经发送完毕
调用 pthread_testcancel()（显式检查是否有取消请求）。
若未被取消，打印 “after cancel”。
主线程等待子线程结束：pthread_join 阻塞等待子线程结束，并通过 res 获取子线程的退出状态。
判断是否取消成功：若 res == PTHREAD_CANCELED，说明子线程被成功取消。
*/


/*
自定义数据（堆内存指针）
线程通过 pthread_exit(void* retval) 或 return (void*)retval 主动返回的数据，通常是堆上分配的内存（如结构体、基本类型指针等）

特殊宏 PTHREAD_CANCELED
当线程被 pthread_cancel 取消并响应请求后，会自动返回 PTHREAD_CANCELED（宏定义，通常为 (void*)-1），用于标识线程是被取消而非正常结束。
*/

/*
sleep(2)这里只是测试函数，表示同步和异步在执行到sleep时的反应
同步和异步这里本质的区别就是会不会迅速终止
那加入我现在while(1)一直循环，同步取消这里就不会取消了，那么异步取消会在cancel传输的瞬间终止
*/