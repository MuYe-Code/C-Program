#include "check.h"

#define BUFLEN 1024

int main(int argc, char* argv[]){
    ARGS_CHECK(argc,3);

    //chatusr1 1.pipe 2.pipe
    char readbuf[BUFLEN]={0};
    char writebuf[BUFLEN]={0};
    
    int chat1=open(argv[1],O_RDONLY);
    ERROR_CHECK(chat1,-1,"open chat1");
    puts("open 1.pipe");

    int chat2=open(argv[2],O_WRONLY);
    ERROR_CHECK(chat2,-1,"open chat2");
    puts("open 2.pipe");

    while(1){
        memset(readbuf,0,BUFLEN);
        int len = read(chat1, readbuf, BUFLEN);
        printf("From USR2:\n");
        write(STDOUT_FILENO,readbuf,len);

        memset(writebuf,0,BUFLEN);
        printf("Myself(USR1):\n");
        len=read(STDIN_FILENO,writebuf,BUFLEN);
        write(chat2,writebuf,len);
    }

    return 0;
}
