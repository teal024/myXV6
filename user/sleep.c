#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[])
{
    if (argc==1)    //只有一个参数
    {
        printf("argument number error\n");
        exit(1);
    }
    int ticks = atoi(argv[1]);  // 转换成数字
    sleep(ticks);
    printf("(nothing happens for a little while)\n");
    exit(0);
}