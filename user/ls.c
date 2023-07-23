#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

char*
fmtname(char *path)   // 输入路径，输出提出的文件名
{
  static char buf[DIRSIZ+1];
  char *p;  // 一个指向字符数组的指针

  // Find first character after last slash.
  for(p=path+strlen(path); p >= path && *p != '/'; p--)   // p的初始位置是路径末尾，不断往前寻找
    ;
  p++;

  // Return blank-padded name.
  if(strlen(p) >= DIRSIZ) // strlen是从p往后一个一个找
    return p;
  memmove(buf, p, strlen(p));
  memset(buf+strlen(p), ' ', DIRSIZ-strlen(p)); // buf末尾处不足dirsiz的地方用‘ ’填充
  return buf;
}

void
ls(char *path)
{
  char buf[512], *p;
  int fd;
  struct dirent de;   // 目录项
  struct stat st;   // 描述文件状态

  if((fd = open(path, 0)) < 0){
    fprintf(2, "ls: cannot open %s\n", path);
    return;
  }

  if(fstat(fd, &st) < 0){
    fprintf(2, "ls: cannot stat %s\n", path);
    close(fd);
    return;
  }

  switch(st.type){
  case T_FILE:
    printf("%s %d %d %l\n", fmtname(path), st.type, st.ino, st.size);
    break;

  case T_DIR:
    if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
      printf("ls: path too long\n");
      break;
    }
    // 如果缓冲区大小能够容纳
    strcpy(buf, path);  // 先把path存存放到buf中
    p = buf+strlen(buf);    // p指针往后移strlen(buf)
    *p++ = '/';   // p后面一位直接用'/'来代表
    while(read(fd, &de, sizeof(de)) == sizeof(de)){
      if(de.inum == 0)
        continue;
      memmove(p, de.name, DIRSIZ);
      p[DIRSIZ] = 0;
      if(stat(buf, &st) < 0){
        printf("ls: cannot stat %s\n", buf);
        continue;
      }
      printf("%s %d %d %d\n", fmtname(buf), st.type, st.ino, st.size);
    }
    break;
  }
  close(fd);
}

int
main(int argc, char *argv[])
{
  int i;

  if(argc < 2){
    ls(".");
    exit(0);
  }
  for(i=1; i<argc; i++)
    ls(argv[i]);
  exit(0);
}
