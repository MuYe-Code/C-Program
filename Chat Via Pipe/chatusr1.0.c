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
    puts("Connection established.");

    fd_set readfds;
    int retVal;

    while(1){
        FD_ZERO(&readfds);
        FD_SET(STDIN_FILENO, &readfds);
        FD_SET(chat1, &readfds);
        retVal = select(chat1+1,&readfds,NULL,NULL,NULL);
        ERROR_CHECK(retVal,-1,"select");

        //check chat1
        if(FD_ISSET(chat1,&readfds)){
            memset(buf,0,BUFLEN);
            retVal = read(chat1,buf,BUFLEN);
            ERROR_CHECK(retVal,-1,"read");
            if(retVal==0) break;;
            printf("From USR2: %s\n",buf);
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
