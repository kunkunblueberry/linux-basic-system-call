#include<pthread.h>
#include<stdlib.h>
#include<stdio.h>
#include<unistd.h>
#include<math.h>
#include<string.h>

pthread_rwlock_t rwlock;
//static pthread_rwlock_t rwlock=PTHREAD_RWLOCK_INITIALIZER;
int shared_data=0;  //逻辑是让两个写线程对这一块地操作

void* _lock_write(void * arg){
    //这里添加写锁
    pthread_rwlock_wrlock(&rwlock);
    int tmp=shared_data+1;
    sleep(1);
    shared_data=tmp;
    printf("当前是%s,shared_data为%d\n",(char*)arg,shared_data);
    //释放写锁
    pthread_rwlock_unlock(&rwlock);
    return (void*)0;
}

void* read_write(void*arg){
    //读逻辑,获取读锁
    pthread_rwlock_rdlock(&rwlock);
    printf("当前是%s,shared_data%d\n",(char*)arg,shared_data);
    //sleep(1);   --即使这里加锁了，还会一起显示，因为加锁这里并不会妨碍别的读取，只是防止写入
    pthread_rwlock_unlock(&rwlock);
    return (void*)0;
}

int main(int argc,char*argv[])
{
    //动态初始化锁
    pthread_rwlock_init(&rwlock,NULL);
    pthread_t write1,write2;//声明两个写线程号
    pthread_t read1,read2,read3,read4,read5,read6;//创建6个读线程

    //char argvq[]="write1";这种方法作为下面最后一个参数也是ok的
    pthread_create(&write1,NULL,_lock_write,(void*)"write1");//最后一个参数是void*,但是字符串是const void*,要么强制转换，要么上面声明一遍字符串
    pthread_create(&write2,NULL,_lock_write,(void*)"write2");

    sleep(3);

    pthread_create(&read1,NULL,read_write,(void*)"read1");
    pthread_create(&read2,NULL,read_write,(void*)"read2");
    pthread_create(&read3,NULL,read_write,(void*)"read3");
    pthread_create(&read4,NULL,read_write,(void*)"read4");
    pthread_create(&read5,NULL,read_write,(void*)"read5");
    pthread_create(&read6,NULL,read_write,(void*)"read6");

    //主线程等待创建的子线程
    pthread_join(write1,NULL);
    pthread_join(write2,NULL);
    pthread_join(read1,NULL);
    pthread_join(read2,NULL);
    pthread_join(read3,NULL);
    pthread_join(read4,NULL);
    pthread_join(read5,NULL);
    pthread_join(read6,NULL);

    //销毁锁
    pthread_rwlock_destroy(&rwlock);
    return 0;
}

/*
当前是write1,shared_data为1
当前是write2,shared_data为2
当前是read1,shared_data2
当前是read2,shared_data2
当前是read3,shared_data2
当前是read4,shared_data2
当前是read5,shared_data2
当前是read6,shared_data2

这样的结果，写锁按照顺序执行，后面读的时候就是正确的每个线程加一次，一共两次

后面下一个这里去掉睡眠，把写锁放在中间
*/

/*
1. pthread 互斥锁（pthread mutex）
核心函数（完整形式）
初始化互斥锁
int pthread_mutex_init(pthread_mutex_t *mutex, const pthread_mutexattr_t *attr);
mutex：指向待初始化的互斥锁对象指针
attr：互斥锁属性（传 NULL 表示使用默认属性，如非递归、默认优先级）

加锁（阻塞式）
int pthread_mutex_lock(pthread_mutex_t *mutex);
若锁已被占用，当前线程阻塞，直到锁释放

尝试加锁（非阻塞式）
int pthread_mutex_trylock(pthread_mutex_t *mutex);
若锁已被占用，直接返回错误码（EBUSY），不阻塞线程

解锁
int pthread_mutex_unlock(pthread_mutex_t *mutex);
仅允许持有锁的线程调用，否则返回错误（EPERM）

销毁互斥锁
int pthread_mutex_destroy(pthread_mutex_t *mutex);
需在锁未被占用、且不再使用时调用

2. pthread 读写锁（pthread rwlock）
核心函数（完整形式）
初始化读写锁
int pthread_rwlock_init(pthread_rwlock_t *rwlock, const pthread_rwlockattr_t *attr);
rwlock：指向待初始化的读写锁对象指针
attr：读写锁属性（传 NULL 用默认属性）

加读锁（阻塞式）
int pthread_rwlock_rdlock(pthread_rwlock_t *rwlock);
多个线程可同时获取读锁，若有写锁占用则阻塞

尝试加读锁（非阻塞式）
int pthread_rwlock_tryrdlock(pthread_rwlock_t *rwlock);
若锁被写锁占用或有写锁等待，返回错误码（EBUSY）

加写锁（阻塞式）
int pthread_rwlock_wrlock(pthread_rwlock_t *rwlock);
仅允许一个线程获取写锁，若有读锁 / 写锁占用则阻塞

尝试加写锁（非阻塞式）
int pthread_rwlock_trywrlock(pthread_rwlock_t *rwlock);
若有任何读锁 / 写锁占用，返回错误码（EBUSY）

解锁（读锁 / 写锁通用）
int pthread_rwlock_unlock(pthread_rwlock_t *rwlock);
持有读锁则释放读锁，持有写锁则释放写锁

销毁读写锁
int pthread_rwlock_destroy(pthread_rwlock_t *rwlock);
*/