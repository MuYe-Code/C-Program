#include "check.h"

#define BUFLEN 1024

int main(int argc, char* argv[]){
    ARGS_CHECK(argc,3);

    //chatusr1 1.pipe 2.pipe
    char buf[BUFLEN]={0};
    
    int chat1=open(argv[1],O_RDONLY);
    ERROR_CHECK(chat1,-1,"open chat1");

    int chat2=open(argv[2],O_WRONLY);
    ERROR_CHECK(chat2,-1,"open chat2");
    puts("Connection with USR2 established.");

    fd_set readfds;
    int retVal;
    struct timeval timeout={60,0};
    struct tm *tmbuf;

    while(1){
        FD_ZERO(&readfds);
        FD_SET(STDIN_FILENO, &readfds);
        FD_SET(chat1, &readfds);
        retVal = select(chat1+1,&readfds,NULL,NULL,&timeout);
        ERROR_CHECK(retVal,-1,"select");
        if(timeout.tv_sec==0){
            puts("Time out, USR1 breaks connection.");
            break;
        }

        //check chat1
        if(FD_ISSET(chat1,&readfds)){
            memset(buf,0,BUFLEN);
            //reset timeout
            timeout.tv_sec=60;
            timeout.tv_usec=0;

            retVal = read(chat1,buf,BUFLEN);
            ERROR_CHECK(retVal,-1,"read");
            if(retVal==0) {
                puts("USR2 breaks connection.");
                break;
            }

            gettimeofday(&timeout,NULL);
            tmbuf=localtime(&timeout.tv_sec);
            printf("From USR2: %02d/%02d %02d:%02d:%02d\n",tmbuf->tm_mon,tmbuf->tm_mday,
                                            tmbuf->tm_hour,tmbuf->tm_min,tmbuf->tm_sec);
            puts(buf);
        }
        
        //check stdin
        memset(buf,0,BUFLEN);
        if(FD_ISSET(STDIN_FILENO,&readfds)){
            //printf("Myself(USR1): \n");
            retVal = read(STDIN_FILENO,buf,BUFLEN);
            ERROR_CHECK(retVal,-1,"read");
            write(chat2,buf,strlen(buf));
        }
    }

    return 0;
}
