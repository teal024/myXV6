#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

void find(char *path, const char *filename)
{
	char buf[512];	  // 保存路径名
	int fd;			  // 文件描述符
	struct dirent de; // 目录项
	struct stat st;	  // 描述文件状态

	if ((fd = open(path, 0)) < 0)
	{
		fprintf(2, "ls: cannot open %s\n", path);
		return;
	}

	if (fstat(fd, &st) < 0)
	{ // 是否成功获取fd的状态, fstat根据文件描述符来获取状态，stat根据路径来获取状态
		fprintf(2, "ls: cannot stat %s\n", path);
		close(fd);
		return;
	}

	switch (st.type)
	{
	// find 不考虑find一个文件，只考虑find一个目录
	case T_FILE:
		//   printf("%s %d %d %l\n", fmtname(path), st.type, st.ino, st.size);
		break;

	case T_DIR:
		if (strlen(path) + 1 + DIRSIZ + 1 > sizeof buf)
		{
			printf("find: path too long\n");
			break;
		}
		// 如果缓冲区大小能够容纳
		strcpy(buf, path);			 // 先把path存存放到buf中
		char *p = buf + strlen(buf); // p从buf后面开始
		*p++ = '/';					 // p后面一位直接用'/'来代表，p指针指向下一位
		while (read(fd, &de, sizeof(de)) == sizeof(de))
		{ // 每次读取一个目录项
			// 从文件描述符 fd 所指向的文件中读取数据，并将读取的数据保存在 de 结构体中
			// 与sizeof(de)比较是为了比较是否相等，如果不相等就不继续
			if (de.inum == 0) // 检查目录项是否有效
				continue;
			memmove(p, de.name, DIRSIZ); // 将目录项 de 中的目录/文件名(de.name)复制到缓冲区 p 中
			p[DIRSIZ] = 0;				 // 添加终止符
			if (stat(buf, &st) < 0)
			{ // 获取buf状态是否失败，一般就在打印buf有关信息前用下即可
				printf("find: cannot stat %s\n", buf);
				continue;
			}
			if (st.type == T_DIR)
			{
				if (strcmp(p, ".") == 0 || strcmp(p, "..") == 0)
					continue; // 避免在"."以及".."中发生递归
				find(buf, filename);
			}
			else if (st.type == T_FILE)
			{
				if (strcmp(filename, p) == 0)
					printf("%s\n", buf);    // 这里打印的是buf，也即完整的路径
			}
		}
		break;
	}
	close(fd);
}

int main(int argc, char *argv[])
{
	if(argc != 3){
    printf("argument number error\n");
    exit(1);
  }
  find(argv[1], argv[2]);
  exit(0);
}
