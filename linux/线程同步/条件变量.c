
/*
 * @brief 调用该方法的线程必须持有 mutex 锁。调用该方法的线程会阻塞并临时释放 mutex 锁，并等待其他线程调用 pthread_cond_signal 或 pthread_cond_broadcast 唤醒。被唤醒后该线程会尝试重新获取 mutex 锁。
 *cond 指向条件变量的指针。条件变量用于等待某个条件的发生。通过某一 cond 等待的线程需要通过同一 cond 的 signal 唤醒
 *mutex 指向条件变量配合使用的互斥锁的指针。在调用 pthread_cond_wait 之前，线程必须已经获得了这个互斥锁。线程必须已经获得这个互斥锁。
 *int 成功时返回 0；失败时返回错误码，而非 -1。错误码可能包括 EINVAL、EPERM 等，具体取决于错误的性质。
 
int pthread_cond_wait(pthread_cond_t *restrict cond, pthread_mutex_t *restrict mutex);

int pthread_cond_timedwait(pthread_cond_t *restrict cond, pthread_mutex_t *restrict mutex, const struct timespec *restrict abstime);

int pthread_cond_signal(pthread_cond_t *cond);

int pthread_cond_broadcast(pthread_cond_t *cond);

1. pthread_cond_wait
想象一个 “会议室”（共享资源），门口有一把 “钥匙”（互斥锁mutex）。线程要进会议室，必须先拿到钥匙（持有mutex）。
如果线程进了会议室，发现 “开会的条件还没满足”（比如人没到齐），它就会：
把钥匙放回门口（临时释放mutex，让其他线程能进）；
自己去旁边的 “等待室”（条件变量cond）坐着等；
直到有人喊 “可以开会了”（其他线程调用signal或broadcast），它才会从等待室出来，重新抢钥匙（获取mutex），再进会议室检查条件。


2. pthread_cond_timedwait
和pthread_cond_wait类似，也是去等待室等，但多带了个 “闹钟”（abstime，绝对时间）。
如果等了很久没人喊（没被signal/broadcast唤醒），闹钟一响，它就会自己从等待室出来，重新抢钥匙进会议室 —— 不管条件满不满足，超时了就不等了。


3. pthread_cond_signal
相当于在等待室门口 “喊一个人”：从等待室里随机挑一个正在等的线程，叫它 “别等了，出来试试抢钥匙吧”。
被喊的线程会醒过来，去抢钥匙（重新获取mutex），然后再检查条件是否满足。


4. pthread_cond_broadcast
相当于在等待室门口 “大喊一声”：“所有人都别等了，出来抢钥匙！”
等待室里所有线程都会被叫醒，一起去抢钥匙（重新竞争mutex），谁抢到谁就进会议室检查条件。

说明：
（1）使用条件变量时，通常涉及到一个或多个线程等待“条件变量”代表的条件成立，而另外一些线程在条件成立时触发条件变量。
（2）条件变量的使用必须与互斥锁配合，以保证对共享资源的访问是互斥的。
（3）条件变量提供了一种线程间的通信机制，允许线程以无竞争的方式等待特定条件的发生。

操作
- 初始化（`pthread_cond_init`）：创建并初始化条件变量。
- 等待（`pthread_cond_wait`）：在给定的互斥锁上等待条件变量。调用时，线程将释放互斥锁并进入等待状态，直到被唤醒。
- 定时等待（`pthread_cond_timedwait`）：等待条件变量或直到超过指定的时间。
- 信号（`pthread_cond_signal`）：唤醒至少一个等待该条件变量的线程。
- 广播（`pthread_cond_broadcast`）：唤醒所有等待该条件变量的线程。
- 销毁（`pthread_cond_destroy`）：清理条件变量资源。
*/

#include <stdio.h>
#include <pthread.h>

#define BUFFER_SIZE 5
int buffer[BUFFER_SIZE];
int count = 0;

//初始化互斥锁
static pthread_mutex_t mutex=PTHREAD_MUTEX_INITIALIZER;
//初始化条件变量
static pthread_cond_t cond=PTHREAD_COND_INITIALIZER;

//向buffer写入数据
void*producer_thread(void*arg){
    int item=1;
        //互斥锁，这里获取锁
    pthread_mutex_lock(&mutex);
    while(1){
        //如果缓冲区写满了，暂停线程
        if(count==BUFFER_SIZE){
            //暂停线程
        pthread_cond_wait(&cond,&mutex);//要写入锁和条件变量两个参数  
    }
    pthread_cond_signal(&cond);//这里唤醒消费者----放外边，就可以循环了
    //要先停下来，才能收到信号，否则是收不到的

        //缓冲区没满
        buffer[count++]=item++;
        printf("写入数字%d\n",buffer[count-1]);
        // //释放
        // pthread_mutex_unlock(&mutex);
    }
}
//读数据
void *consum_thread(void*arg){
         //互斥锁，这里获取锁
        pthread_mutex_lock(&mutex);   
    while(1){
        //如果缓冲区写满了，暂停线程
        if(count==0){//缓存区消息读完了
            pthread_cond_wait(&cond,&mutex);
        }
        printf("读取的数字%d\n",buffer[--count]);
        pthread_cond_signal(&cond);//这里唤醒生产者
    }
    // //释放
    // pthread_mutex_unlock(&mutex);          ----这里互相都不用释放锁啊，这里条件变量自动就给对方线程了
}
//两个线程不断迭代

int main(int argc,char*argv[]){
    //创建两个线程，一个从buffer读，一个写
    pthread_t producer,consum;
    pthread_create(&producer,NULL,producer_thread,NULL);
    pthread_create(&consum,NULL,consum_thread,NULL);

    //主线程需要挂起等待子线程
    pthread_join(producer,NULL);
    pthread_join(consum,NULL);
    return 0;
}
//上面实现了读满五个就换位，又消耗






/*
void*producer_thread(void*arg){
    int item=1;
    while(1){
        //互斥锁，这里获取锁
        pthread_mutex_lock(&mutex);
        //如果缓冲区写满了，暂停线程
        if(count==BUFFER_SIZE){
            //暂停线程
            pthread_cond_wait(&cond,&mutex);//要写入锁和条件变量两个参数
        }
        //缓冲区没满
        buffer[count++]=item++;
        printf("写入数字%d\n",buffer[count-1]);

        pthread_cond_signal(&cond);//这里唤醒消费者

        //释放
        pthread_mutex_unlock(&mutex);
    }
}
//读数据
void *consum_thread(void*arg){
    while(1){
        //互斥锁，这里获取锁
        pthread_mutex_lock(&mutex);
        //如果缓冲区写满了，暂停线程
        if(count==0){//缓存区消息读完了
            pthread_cond_wait(&cond.&mutex);
        }
        printf("读取的数字%d\n",buffer[--count]);
    }
    pthread_cond_signal(&cond);//这里唤醒生产者

    //释放
    pthread_mutex_unlock(&mutex);
}
*/
//这样的逻辑是，看谁同时运行先抢到锁，运行一次读或者写，因为没满的时候就释放，另一个可能抢到
//也可能没抢到，又是进行一次相同的操作，反正就是如此循环
//结果是无法预测的，