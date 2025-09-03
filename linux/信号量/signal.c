#include<stdio.h>
#include<stdlib.h>
#include<signal.h>
#include<unistd.h>

void sigint_handler(int signum){
    printf("\n收到%d信号 停止程序",signum);
    exit(signum);
}
/*
这是一个自定义的信号处理函数，当程序收到特定信号时，会自动执行这个函数。
参数 signum 表示收到的信号编号（比如 SIGINT 信号的编号是 2）。
函数功能：打印 “收到信号” 的提示，然后调用 exit 退出程序。
*/
int main(int argc,char*argv[]){
    if(signal(SIGINT,sigint_handler)==SIG_ERR)
    {
        perror("signal");
        return 1;
    }
    /* 
    signal(SIGINT, sigint_handler)：这行代码的作用是 **“注册信号处理规则”**—— 告诉操作系统：
    “当程序收到 SIGINT 信号时，不要执行默认操作，而是执行我定义的 sigint_handler 函数”。
    如果注册失败（返回 SIG_ERR），则用 perror 打印错误信息并退出。
    */
    while(1){
        sleep(1);
        printf("hello");
    }

    //当你按下 Ctrl+C 时（这会发送 SIGINT 信号）
}