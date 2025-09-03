/**
 * @brief 为 rwlock 指向的读写锁分配所有需要的资源，并将锁初始化为未锁定状态。读写锁的属性由 attr 参数指定，
 * 如果 attr 为 NULL，则使用默认属性。当锁的属性为默认时，可以通过 PTHREAD_RWLOCK_INITIALIZER 初始化，即
 * pthread_rwlock_t rwlock = PTHREAD_RWLOCK_INITIALIZER; 效果和调用当前方法并为 attr 传入 NULL 是一样的
 *
 * @param rwlock 读写锁
 * @param attr 读写锁的属性  --没有就设为NULL
 * @return int 成功则返回 0，否则返回错误码
 * int pthread_rwlock_init(pthread_rwlock_t *restrict rwlock, const pthread_rwlockattr_t *restrict attr);
 */
/*
上面是动态初始化，也有静态初始化，和前面的竞态是一个道理，后面加上参数
动态初始化一定要显示销毁
*/

/**
@brief 销毁 rwlock 指向的读写锁对象，并释放它使用的所有资源。当任何线程持有锁的时候销毁锁，或尝试销毁一个未初始化的锁，结果是未定义的。
@param rwlock
@return int
int pthread_rwlock_destroy(pthread_rwlock_t *rwlock);
*/

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
    int tmp=shared_data+1;
    sleep(1);
    shared_data=tmp;
    printf("当前是%s,shared_data为%d\n",(char*)arg,shared_data);

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
//g++ test.c -o test -pthread  ------要用这个才可以正确连接到线程库
/*
结果全是1，因为写锁相互竞争，只有一个数据写入
并且打印的显示也是同时出现的
*/

/*
有一个共享变量 count，线程 A 正在读取 count 的值，此时线程 B 对 count 进行修改。
如果没有读锁的保护，线程 A 读取到的 count 值可能是一个 “中间状态” 的值，既不是修改前的值，
也不是修改后的值，从而导致数据读取错误。通过加读锁，在多个线程同时读时，阻止写线程对数据进行修改，保证读操作获取到的是一致且正确的数据。
*/
