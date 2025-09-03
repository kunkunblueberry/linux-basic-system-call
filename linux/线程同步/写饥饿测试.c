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
    printf("我是写锁获取了\n");
    pthread_rwlock_wrlock(&rwlock);
    int tmp=shared_data+1;
    sleep(1);
    shared_data=tmp;
    printf("当前是%s,shared_data为%d\n",(char*)arg,shared_data);
    //释放写锁
    pthread_rwlock_unlock(&rwlock);
    printf("我是写锁释放了\n");
    return (void*)0;
}

void* read_write(void*arg){
    //读逻辑,获取读锁
    printf("我是读锁获取了\n");
    pthread_rwlock_rdlock(&rwlock);
    printf("当前是%s,shared_data%d\n",(char*)arg,shared_data);
    sleep(1);
    pthread_rwlock_unlock(&rwlock);
    printf("我是读锁释放了\n");
    return (void*)0;
}

int main(int argc,char*argv[])
{
    //创建属性对象
    pthread_rwlockattr_t attr;
    pthread_rwlockattr_init(&attr);//初始化
    //修改参数，让写优先
    pthread_rwlock_setkind_np(&attr,PTHREAD_RWLOCK_PREFER_WRITER_NONRECURSIVE_NP);

    //动态初始化锁
    pthread_rwlock_init(&rwlock,&attr);//把这里的NULL,换为创建的对象
    pthread_t write1,write2;//声明两个写线程号
    pthread_t read1,read2,read3,read4,read5,read6;//创建6个读线程

    //char argvq[]="write1";这种方法作为下面最后一个参数也是ok的
    pthread_create(&write1,NULL,_lock_write,(void*)"write1");//最后一个参数是void*,但是字符串是const void*,要么强制转换，要么上面声明一遍字符串
    //pthread_create(&write2,NULL,_lock_write,(void*)"write2");


    pthread_create(&read1,NULL,read_write,(void*)"read1");
    pthread_create(&read2,NULL,read_write,(void*)"read2");
    pthread_create(&read3,NULL,read_write,(void*)"read3");
    pthread_create(&write2,NULL,_lock_write,(void*)"write2");
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
其实这个也就是写饥饿哈
读写锁两个锁是互斥的，这里读锁sleep一秒。导致写锁一直无法获取，读操作一直并发执行，不会抢，但是写锁要抢


但是如果加上sleep
我是写锁获取了
我是读锁获取了
我是读锁获取了
我是读锁获取了
我是写锁获取了
我是读锁获取了
我是读锁获取了
我是读锁获取了
当前是write1,shared_data为1
当前是read1,shared_data1
当前是read4,shared_data1
当前是read2,shared_data1
我是写锁释放了
当前是read3,shared_data1
当前是read6,shared_data1
当前是read5,shared_data1
我是读锁释放了
我是读锁释放了
我是读锁释放了
我是读锁释放了
我是读锁释放了
我是读锁释放了
当前是write2,shared_data为2

默认情况下其实读锁的优先级比较高，所以写锁想避免上面的读锁一直霸占，干脆给写锁抬咖位
就是加一个这样的属性

我是写锁获取了
我是读锁获取了
我是读锁获取了
我是读锁获取了
我是写锁获取了
我是读锁获取了
我是读锁获取了
我是读锁获取了
当前是write1,shared_data为1
我是写锁释放了
当前是write2,shared_data为2
当前是read1,shared_data2
我是写锁释放了
当前是read3,shared_data2
当前是read2,shared_data2
当前是read4,shared_data2
当前是read5,shared_data2
当前是read6,shared_data2
我是读锁释放了
我是读锁释放了
我是读锁释放了
我是读锁释放了
我是读锁释放了
我是读锁释放了

解决了
*/