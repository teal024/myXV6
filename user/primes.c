#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void exec_pipe(int* fd)
{
    int num;
    read(fd[0], &num, 4);  // 把fd中的第一个内容读出到num中
    printf("prime %d\n", num);

    int p[2];
    pipe(p);
    int tmp = -1;
    while (1) {
        int n = read(fd[0], &tmp, 4);      // 把fd中剩下的值读给tmp
        if (n <= 0) {
            break;
        }
        if (tmp % num != 0) {
            // 不断读缓冲区，只要有，就读出来测试
            // 第一轮num是从2开始读，tmp是从3开始读，tmp读的过程中只留下不是2的倍数的
            // 等到第二轮开始，就是从3开始读，
            write(p[1], &tmp, 4);
        }
    }
    if (tmp == -1) {
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
    else {
        close(p[1]);
        close(p[0]);
        close(fd[0]);
        wait(0);
        // 阻塞父进程，直到有子进程结束
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