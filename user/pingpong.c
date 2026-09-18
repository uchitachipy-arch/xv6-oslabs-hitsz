#include "kernel/types.h"
#include "user/user.h"

int main() {
    int p[2];
    int p2[2];
    pipe(p); //父写子读
    pipe(p2); // 父读子写
    int father_pid = getpid();
    int child_pid = fork();
    if (child_pid == 0) {
        close(p[1]); //子关闭写
        close(p2[0]); //子关闭读
        char buf[15];
        int n=read(p[0],buf,4);
        buf[n]= '\0';
        printf("%d: received %s from pid %d\n", getpid(),buf, father_pid);
        close(p[0]);
        write(p2[1], "pong", 4);
        close(p2[1]);
    } else {
        close(p[0]); //父关闭读
        close(p2[1]); //父关闭写
        write(p[1], "ping", 4);
        close(p[1]);
        char buf[15];
        int n=read(p2[0],buf,4);
        buf[n] = '\0';
        printf("%d: received %s from pid %d\n", father_pid, buf, child_pid);
        close(p2[0]);
    }
    exit(0);
}