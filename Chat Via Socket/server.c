/*************************************************************************
    > File Name: chatViaTCP_S2.c
    > Author: masker
    > Mail: https://github.com/MuYe-Code
    > Created Time: 2022年05月27日 星期五 20时30分15秒
 ************************************************************************/

#include <check.h>

#define PORT 1234
#define BUFLEN 1024
int main(int argc, char* argv[]){
    //ARGS_CHECK(argc,2);
    
    //create socket
    int sfd = socket(AF_INET,SOCK_STREAM,0);
    ERROR_CHECK(sfd,-1,"socket");
    struct sockaddr_in serAddr;
    memset(&serAddr,0,sizeof(serAddr));
    serAddr.sin_family = AF_INET;
    serAddr.sin_addr.s_addr = INADDR_ANY;
    serAddr.sin_port = htons(PORT);

    //bind socket 
    int ret = bind(sfd,(struct sockaddr*)&serAddr,sizeof(serAddr));
    ERROR_CHECK(ret,-1,"bind");

    //listening
    ret = listen(sfd,10);
    ERROR_CHECK(ret,-1,"listen");
    puts("Status: Listening...\n");
    
    //为两个客户端程序分别建立套接字
    int newFd1 = accept(sfd,NULL,NULL);
    ERROR_CHECK(newFd1,-1,"accept");
    send(newFd1,"Waiting...\n",11,0);
    int newFd2 = accept(sfd,NULL,NULL);
    ERROR_CHECK(newFd2,-1,"accept");
    send(newFd1,"Connection established.\n",24,0);
    send(newFd2,"Connection established.\n",24,0);
    puts("Connectin established.");

    char buf[BUFLEN] = {0};
    fd_set readset;
    while(1){
        //监听
        FD_ZERO(&readset);
        FD_SET(newFd1,&readset);
        FD_SET(newFd2,&readset);
        select(newFd2+1,&readset,NULL,NULL,NULL);
        //转发
        if(FD_ISSET(newFd2,&readset)){
            memset(buf,0,BUFLEN);
            ret = recv(newFd2,buf,BUFLEN,0);
            ERROR_CHECK(ret,-1,"recv");
            if(0 == ret){
                send(newFd1,"Connection closeed.\n",20,0);
                break;
            }
            send(newFd1,buf,strlen(buf),0);
        }
        else if(FD_ISSET(newFd1,&readset)){
            memset(buf,0,BUFLEN);
            ret = recv(newFd1,buf,BUFLEN,0);
            ERROR_CHECK(ret,-1,"recv");
            if(0 == ret){
                send(newFd2,"Connection closeed.\n",20,0);
                break;
            }
            send(newFd2,buf,strlen(buf),0);
        }
    }
    //关闭
    puts("Connection closed.");
    close(sfd);
    close(newFd1);
    close(newFd2);
    return 0;
}
