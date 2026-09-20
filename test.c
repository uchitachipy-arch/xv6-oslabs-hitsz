#include <unistd.h>

int main() {
    int p[2];
    char *argv[2];
    argv[0] = "wc";
    argv[1] = 0;
    pipe(p);

    if (fork() == 0) {
        close(0);          // 先把标准输入 0 腾出来
        dup(p[0]);         // dup() 返回当前最小未使用 fd，此时通常就是 0
        close(p[0]);       // 原来的读端 fd 不再需要了
        //close(p[1]);       // 子进程不用写端，必须关闭
        execv("/bin/wc", argv);   // 现在 wc 会从标准输入 0 读取数据
    } else {
        close(p[0]);                     // 父进程不用读端，关闭
        write(p[1], "hello world\n", 12); // 向管道写入数据
        close(p[1]);                     // 写完后关闭写端，告诉读端“数据结束了”
    }

    return 0;
}