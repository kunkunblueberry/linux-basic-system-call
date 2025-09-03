/*
void pthread_exit(void *retval);
参数void *retval，返回给创建线程
当某个线程调用用`pthread_exit`方法后，该线程会被关闭（相当于`return`）。
线程可以通过`retval`向其它线程传递信息，`retval`指向的区域不可以放在线程函数的栈内。
其他线程（例如主线程）如果需要获得这个返回值，需要调用`pthread_join`方法。

pthread_join(pthread_t thread,void **retval)
 pthread_t thread：指定线程ID。
 void **retval：这是一个可选参数，用于接收线程结束后传递的返回值。
如果非空，`pthread_join`会在成功时将线程的 exit status
复制到`*retval`所指向的内存位置。如果线程没有显式地通过`pthread_exit`提供返回值，
则该参数将被设为 NULL 或忽略。
成功返回0，失败返回1。

int pthread_detach (pthread_t thread);
功能：将线程标记为 detached 状态。
说明：POSIX 线程终止后，若未调用 pthread_detach 或 pthread_join，
其资源会持续占用内存（类似僵尸进程）。
线程默认处于可 join 状态，可通过 pthread_join 等待其终止并回收资源。
若主线程无需等待线程终止，可将其标记为 detached 状态，线程终止后资源会自动被系统回收。
-------join是等待同步，detach是自己关闭回收

pthread_cancel
向目标线程发送请求，目标现场是否和合适响应取决于它的取消状态和类型
这个操作异步
简单来说吗，他只会说一声关闭，但是是否关闭需要其他函数再检查提醒
*/
#include<pthread.h>
#include<stdlib.h>
#include<stdio.h>
#include<unistd.h>
#include<math.h>
#include<string.h>
//整理一下各个层级的输入输出，fgets,write

//定义结构体接收线程结果
typedef struct Result{
    char*p;
    int len;
}result;
//红代码执行函数
void *red_thread(void *arg){
    //初始化result结构体
    Result *result=malloc(sizeof(Result));
    char code=((char*)arg);  //参数是void，强制转换才可以使用，而且这里传入的是参数
    //声明存放消息的字符串
    char *ans=malloc(100);//存放读取的字符
    while(1){
        fgets(ans,100,stdin);
        if(ans[0]==code){
            //这里接到了回复的消息
            free(ans);
            printf("红线程离开");
            char *redans=strdup("红线程离开");
            result->p=redans;
            result->len=strlen(redans);
            //线程结束
            pthread_exit((void*)result);    //结果是指针，返回值无法接受，这里来进行接受
        }
    }
}
void *white_thread(void *arg){
    // 初始化result结构体（堆内存分配）
    Result *result = malloc(sizeof(Result));
    // 从参数获取目标字符
    char code = *((char*)arg);
    
    // 声明存放消息的字符串（堆内存）
    char *ans = malloc(100);
    while(1){
        printf("请输入内容（输入 '%c' 结束白线程）: ", code);
        fgets(ans, 100, stdin);
        // 移除fgets读取的换行符
        ans[strcspn(ans, "\n")] = '\0';
        
        if(ans[0] == code){
            free(ans);  // 释放输入缓冲区
            printf("白线程准备离开\n");
            
            // 准备返回结果
            char *whiteans = strdup("白线程已正常退出");
            result->p = whiteans;
            result->len = strlen(whiteans);
            
            // 线程退出并返回结果
            pthread_exit((void*)result);
        }
    }
}

int main(int argc,char*argv[]){
    //创建两个线程，红，白，都等着同一个目标
    pthread_t pid_r;
    pthread_t pid_w;

    char read_code='r';
    char white_code='w';

    Result*red_result=NULL;
    Result*white_result=NULL;
    //红线程
    pthread_create(&pid_r,NULL,red_thread,&red_code);       //最后一位传递的是参数，函数参数
    //白线程
    pthread_create(&pid_w,NULL,white_thread,&white_code);
    
    pthread_join(pid_r,(void**)&red_result);   //二级指针传入一级指针的结果，这里记得强行抓换
    pthread_join(pid_w,(void**)&white_result);
    //线程通过pthread_exit(void *retval)传递的结果，由主线程的pthread_join函数接收

    printf("红线程的结局%d是%s\n",red_result->len,red_result->p);
    printf("白线程的结局%d是%s\n",white_result->len,white_result->p);

    //获取线程结果

    free(red_result->p);
    free(white_result->p);
    free(red_result);
    free(white_result);
    return 0;
}


//不过这份代码还是有问题，多个线程同时接入到输入输出，会有冲突，导致只有一个线程可以使用标准输入输出
//用锁来解决