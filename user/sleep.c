#include "kernel/types.h"
#include "user/user.h"

int argv_size(char *argv[]){
    int cnt = 0;
    while (argv[cnt] != 0) cnt++;
    return cnt;
}
int main(int argc, char *argv[])
{
    // printf("argc=%d,argv_size=%d\n",argc,argv_size(argv));
    // for(int i=0;i<argc;i++){
    //     printf("argv[%d]=%s\n",i,argv[i]);
    // }
    if(argc != 2){
        printf("Sleep needs one argument!\n");
        exit(-1);
    }

    int ticks = atoi(argv[1]);
    sleep(ticks);
    printf("Sleep %d\n", ticks);
    exit(0);
}
