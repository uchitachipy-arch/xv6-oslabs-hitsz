#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

/*
文件项
struct stat {
  int dev;
  uint ino;
  short type;
  short nlink;
  uint64 size;
};
目录项
struct dirent {
  ushort inum;
  char name[DIRSIZ];
};
*/
char *fmtname(char *path) {
    char *p;

  // Find first character after last slash.
    for (p = path + strlen(path); p >= path && *p != '/'; p--)
    ;
    p++;

  // Return blank-padded name.
    return p;
}

void find(char *path,char *name){
    char buf[512],*p;
    int fd;
    struct dirent de;
    struct stat st;
    if((fd = open(path,0))<0){ //open(path,0) 0 表示只读
        fprintf(2,"find: cannot open %s\n",path);
        return;
    }
    if(fstat(fd,&st)<0){
        fprintf(2,"find: cannot stat %s\n",path);
        close(fd);
        return;
    }
    if(1){
        if(strcmp(fmtname(path),name) == 0){
            printf("%s\n",path);
        }
    }
    if(st.type == T_DIR){
        strcpy(buf,path);
        p = buf + strlen(buf);
        *p++ = '/';
        while(read(fd,&de,sizeof(de)) == sizeof(de)){
            if(de.inum == 0) continue;
            memmove(p, de.name, DIRSIZ);
            p[DIRSIZ] = 0;
            if(strcmp(p,".") == 0 || strcmp(p,"..") == 0) continue;
            find(buf,name);
        }
    }
    
    close(fd);
}
int main(int argc, char *argv[])
{
    if(argc != 3){
        fprintf(2,"Usage: find <directory> <filename>\n");
        exit(-1);
    }
    find(argv[1],argv[2]);
    exit(0);
}
