#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/param.h"

#define MAX_ARG_LEN 512

// 辅助函数：将数组清空（所有元素置为0）
void clearArray(char *array, int size)
{
    // 使用memset函数将数组的内存区域设置为0
    memset(array, 0, sizeof(char) * size);
}

int main(int argc, char *argv[])
{
    // printf("xargs here");
    if (argc < 2)
    {
        printf("argument number error\n");
        exit(1);
    }
    char *input[MAXARG];
    for (int i = 0; i < MAXARG; i++)
    {
        input[i] = (char *)malloc(sizeof(char) * MAX_ARG_LEN);
    }

    // 读取标准输入并处理参数
    int lineStart = 0;
    int inputCount = 0;
    char p;

    while (1)
    {
        int rdResult = read(0, &p, sizeof(char));
        if (rdResult <= 0)
            break;
        if (p == '\n')
        {
            if (lineStart >= MAX_ARG_LEN) // 如果有超过，清除当前Input中的值，指针归位
            {
                clearArray(input[inputCount], MAX_ARG_LEN);
                lineStart = 0;
            }
            else
            {
                input[inputCount][lineStart] = '\0';
                inputCount++;
                lineStart = 0;
            }
        }
        else
        {
            input[inputCount][lineStart] = p;
            lineStart++;
        }
    }

    // 创建子进程并执行新的命令
    int pid = fork();
    if (pid == 0)
    {
        // printf("this is the child process\n");
        char *cmd_argv[MAXARG];
        for (int i = 1; i < argc; i++)
        {
            cmd_argv[i - 1] = argv[i];
        }
        for (int i = 0; i < inputCount; i++)
        {
            cmd_argv[i + argc - 1] = input[i];
        }
        cmd_argv[inputCount + argc - 1] = 0;
        exec(cmd_argv[0], cmd_argv);
        exit(1); // 如果exec执行失败则退出子进程
    }
    else
    {
        wait(0);
    }
    for (int i = 0; i < MAXARG; i++)
    {
        free(input[i]);
    }
    exit(0);
}
