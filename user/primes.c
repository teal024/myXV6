#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void exec_pipe(int* fd)
{
    int num;    // 储存fd中第一个值以进行打印
    read(fd[0], &num, 4);
    printf("prime %d\n", num);

    int p[2];
    pipe(p);
    int tmp = -1;
    while (1) {
        int n = read(fd[0], &tmp, 4);   // 读取输入的所有值
        if (n <= 0) {
            break;
        }
        if (tmp % num != 0) {       // 写入管道
            write(p[1], &tmp, 4);
        }
    }
    if (tmp == -1) {    // 管道中缓冲区空，筛选完
        close(p[1]);
        close(p[0]);
        close(fd[0]);
        return;
    }
    int pid = fork();
    if (pid == 0) {
        close(p[1]);
        close(fd[0]);
        exec_pipe(p);
        close(p[0]);
    }
    else {      // 父进程，执行完直接阻塞，等待子进程结束
        close(p[1]);    // 关闭写端
        close(p[0]);    // 关闭读端
        close(fd[0]);
        wait(0);
    }
}

int
main() {
    int p[2];
    pipe(p);    // 初始化一个管道，用来把2到35全传进管道等待读
    for(int i = 2; i <= 35; i++)
    {
        write(p[1],&i,4);
    }
    close(p[1]);
    exec_pipe(p);
    close(p[0]);
    exit(1);
}