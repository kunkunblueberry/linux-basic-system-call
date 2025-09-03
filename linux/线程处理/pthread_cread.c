#include<pthread.h>
#include<stdlib.h>
#include<stdio.h>
#include<unistd.h>

/*int pthread_create(pthread_t *thread,
 const pthread_attr_t *attr, 
 void *(*start_routine)(void *), void *arg);
    pthread_t 和pid_t一个性质，这里创建一个线程独一无二编号
    const pthread_attr_t *attr, ，指向线程属性结构体，一般传入NULL
    void *(*start_routine)(void *),要执行函数的函数指针
    void *arg，函数参数
*/
#define BUF 1024
char *buf;

//程序实现创建两个线程
//1，读取控制台信息，写入到缓存
//2，缓存信息写到控制台

    //声明线程需要执行的逻辑
    void* input_thread(void *argv){
        int i=0;
        while(1){
            char c=fgetc(stdin);
            if(c&&c!='\n'){
                buf[i++]=c;
            }
            //缓冲区索引溢出
            if(i>=BUF){
                i=0;//写满了从头开始写
            }
        }
    }
    void * output_thread(void *argv){
        int i=0;
        while(1){
            if(buf[i]){  //初始化buf全部为0，非零则读取，不为零停止
                fputs(buf[i],stdout);
                fput('\n',stdout);  
                buf[i++]=0; //一边读取，一边清空
                if(i>=BUF){
                    i=0;
                }
                else{   //因为线程是同时执行，一边输入，一边输出，防止速度不同，线程空转
                    //如果没有输入就sllep
                    sleep(1);
                }
            }
        }
    }
int main(int argc,char*argv[]){
    //声明线程id
    pthread_t pid_input;
    pthread_t pid_output;
    //初始化数组
    buf=malloc(BUF);


    //创建线程
    pthread_create(&pid_input,NULL,input_thread,NULL);//参数为空
    pthread_create(&pid_output,NULL,output_thread,NULL);
    
    //主线程也要记得控制
    pthread_join(pid_input,NULL);
    pthread_join(pid_output,NULL);
    return 0;
}