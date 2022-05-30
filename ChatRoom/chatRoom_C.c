/*************************************************************************
    > File Name: chatViaTCP_C.c
    > Author: masker
    > Mail: https://github.com/MuYe-Code
    > Created Time: 2022年05月28日 星期六 12时43分15秒
 ************************************************************************/

#include <check.h>

#define PORT 1234
#define BUFLEN 1024
int main(int argc, char* argv[]){
    //chatViaTCP_c IP Name
    //ARGS_CHECK(argc,3);
    if(argc < 2){
        printf("Expect at least 2 arguments\n");
        exit(0);
    }
    char name[20] = {0};

    if(argc == 2){
        srand((unsigned)time(NULL));
        sprintf(name,"unknown%d",rand()%100);
    }
    else strcpy(name,argv[2]);

    //create socket
    int sfd = socket(AF_INET,SOCK_STREAM,0);
    ERROR_CHECK(sfd,-1,"socket");
    struct sockaddr_in serAddr;
    memset(&serAddr,0,sizeof(serAddr));
    serAddr.sin_family = AF_INET;
    serAddr.sin_addr.s_addr = inet_addr(argv[1]);
    serAddr.sin_port = htons(PORT);

    //connect
    int ret = connect(sfd,(struct sockaddr*)&serAddr,sizeof(serAddr));
    ERROR_CHECK(ret,-1,"connet");
    printf("Connection to %s established\n\n",argv[1]);

    struct tm *tmbuf;
    struct timeval timeout={60,0};
    char buf[BUFLEN] = {0};
    fd_set readset;
    while(1){
        FD_ZERO(&readset);
        FD_SET(STDIN_FILENO,&readset);
        FD_SET(sfd,&readset);
        //listening
        select(sfd+1,&readset,NULL,NULL,NULL);
        if(FD_ISSET(STDIN_FILENO,&readset)){
            memset(buf,0,BUFLEN);
            puts("--------------------------------------------------------------");
            //send name and time
            gettimeofday(&timeout,NULL);
            tmbuf=localtime(&timeout.tv_sec);
            sprintf(buf,"%s %02d/%02d %02d:%02d:%02d\n",name,tmbuf->tm_mon,tmbuf->tm_mday,
                                             tmbuf->tm_hour,tmbuf->tm_min,tmbuf->tm_sec);
            ret = read(STDIN_FILENO,buf+strlen(buf),BUFLEN);
            ERROR_CHECK(ret,-1,"read");
            if(0 == ret){
                break;
            }
            send(sfd,buf,strlen(buf),0);
        }
        else if(FD_ISSET(sfd,&readset)){
            memset(buf,0,BUFLEN);
            ret = recv(sfd,buf,BUFLEN,0);
            ERROR_CHECK(ret,-1,"recv");
            if(0 == ret){
                puts("Server closed connection.");
                break;
            }
            printf("%s\n",buf);
            puts("--------------------------------------------------------------");
        }
    }
    close(sfd);
    return 0;
}
