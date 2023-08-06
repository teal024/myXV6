#include "kernel/param.h"
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  // 前两个命令行参数被用来设置跟踪掩码和指定要执行的新程序。其余的命令行参数是要传递给新程序的实际参数
  int i;
  char *nargv[MAXARG];

  if(argc < 3 || (argv[1][0] < '0' || argv[1][0] > '9')){   // 命令行参数中第二个首字母的值为数字
    fprintf(2, "Usage: %s mask command\n", argv[0]);
    exit(1);    // 需要有3个以上参数，即必须trace x ……
  }

  if (trace(atoi(argv[1])) < 0) {   // 找到argv对应的数组对应的mask
    fprintf(2, "%s: trace failed\n", argv[0]);
    exit(1);
  }
  
  for(i = 2; i < argc && i < MAXARG; i++){
    nargv[i-2] = argv[i];
  }
  exec(nargv[0], nargv);  // 系统调用
  exit(0);
}
