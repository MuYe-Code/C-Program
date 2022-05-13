#include "check.h"

#define BUFLEN 1024

int main(int argc, char* argv[]){
    ARGS_CHECK(argc,3);
    
    //chatusr2 1.pipe 2.pipe
    char buf[BUFLEN]={0};
    
    //write to 1.pipe, read from 2.pipe
    int chat1=open(argv[1],O_WRONLY);
    ERROR_CHECK(chat1,-1,"open chat1");

    int chat2=open(argv[2],O_RDONLY);
    ERROR_CHECK(chat2,-1,"open chat2");
    puts("Connectin established.");
    
    fd_set readfds;
    int retVal;

    while(1){
        FD_ZERO(&readfds);
        FD_SET(STDIN_FILENO, &readfds);
        FD_SET(chat2, &readfds);
        retVal = select(chat2+1,&readfds,NULL,NULL,NULL);
        ERROR_CHECK(retVal,-1,"select");
        //check stdin
        if(FD_ISSET(STDIN_FILENO,&readfds)){
            memset(buf,0,BUFLEN);
            //printf("Myself(USR2): \n");
            retVal = read(STDIN_FILENO,buf,BUFLEN);
            ERROR_CHECK(retVal,-1,"read");
            write(chat1,buf,strlen(buf));
        }

        //check chat2
        if(FD_ISSET(chat2,&readfds)){
            memset(buf,0,BUFLEN);
            retVal = read(chat2,buf,BUFLEN);
            ERROR_CHECK(retVal,-1,"read");
            if(retVal==0) break;
            printf("From USR1: \n%s\n", buf);
        }
    }

    return 0;
}
