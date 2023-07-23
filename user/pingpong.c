#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main() {
  int pipefdpc[2], pipefdcp[2];
  int pid;
  char *ping = "ping";
  char *pong = "pong";
  char buf[512] = {0};

  pipe(pipefdpc);
  pipe(pipefdcp);

  pid = fork();
  if (pid != 0) {
    // parent
    close(pipefdpc[0]);     // 关闭parent->child的pipe的读端
    close(pipefdcp[1]);     // 关闭child->parent的pipe的写端
    // 这样做是为了确保在parent进程中只能写入到pipefdpc管道并读取pipefdcp管道，这里注释掉也不影响
    write(pipefdpc[1], ping, strlen(ping));     // parent把ping传给child
    read(pipefdcp[0], buf, sizeof(buf));        // parent从pipefdcp管道中读出child发给他的
    printf("%d: received %s\n", getpid(), buf);     // parent打印读出来的
    exit(0);
  } else {
    // child
    close(pipefdpc[1]);
    close(pipefdcp[0]);
    read(pipefdpc[0], buf, sizeof(buf));
    printf("%d: received %s\n", getpid(), buf);
    write(pipefdcp[1], pong, strlen(pong));
    exit(0);
  }
}