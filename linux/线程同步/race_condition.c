#include<pthread.h>
#include<stdlib.h>
#include<stdio.h>
#include<unistd.h>
#include<math.h>
#include<string.h>

#define THREAD_COUNT 20000

//初始化锁
static pthread_mutex_t counter_mutex=PTHREAD_MUTEX_INITIALIZER;
//这里不用显示销毁，静态锁最后会自己关掉

void*add_thread(void*num){
    //基本上第一步都是参数转换
    int*p=(int*)p;

    //累加之前获取锁,对同一块区域操作的时候
    pthread_mutex_lock(&counter_mutex);
    (*p)++;
    //累加后释放
    pthread_mutex_unlock(&counter_mutex);
    return (void*)0;    //第一次见，新奇欸
}

int main(int argc,char*argv[]){
    pthread_t pid[THREAD_COUNT];
    int num=0;
    for(size_t i=0;i<THREAD_COUNT;i++){
        //创建的线程是给传入的参数加一
        pthread_create(pid+i,NULL,add_thread,&num);
    }

    //等待所有的线程执行完成
    for(size_t i=0;i<THREAD_COUNT;i++){
        pthread_join(pid[i],NULL);
    }

    //打印最终的结果
    printf("累加的结果%d",num);
}
//可以看到每次结果都不是确定值，也就是竞争导致了这种情况
//加锁
/*
1，互斥锁   同一个时间只有一个线程可以访问区域
2，读写锁   允许多个同时读，但是写只能有一个
3，自旋锁   获取锁之前，线程在循环里面忙中等待，适用于锁持有时间短的情况
*/