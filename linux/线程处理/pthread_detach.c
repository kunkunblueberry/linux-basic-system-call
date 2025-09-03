#include<pthread.h>
#include<stdlib.h>
#include<stdio.h>
#include<unistd.h>
#include<math.h>
#include<string.h>

void*task(void *arg){
    printf("线程启动\n");
    sleep(2);
    printf("线程结束\n")
}

int main(int argc,char*argv[]){
    pthread_t pid;
    pthread_create(&pid,NULL,task,NULL);

    //使用detach标记线程后会回收相关资源
    pthread_detach(pid);

    //主线程运行完成比创建的子线程慢,如果父线程先结束，会强制杀死线程
    printf("main线程,continue\n");
    sleep(3);
    printf("main线程ending");
}
/*
将指定线程标记为 "分离态（detached）"，使得线程终止后系统会自动回收其占用的资源（如线程 ID、栈内存等），
无需主线程调用pthread_join等待回收。
若不调用pthread_detach，线程默认处于 "可连接态（joinable）"，此时：
子线程结束后，资源不会立即释放（类似 "僵尸进程"）
必须通过pthread_join(pid, NULL)显式回收，否则会造成资源泄漏
*/