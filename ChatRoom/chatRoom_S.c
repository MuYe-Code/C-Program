/*************************************************************************
    > File Name: chatRoom_S.c
    > Author: masker
    > Mail: https://github.com/MuYe-Code
    > Created Time: 2022年05月28日 星期六 12时43分15秒
 ************************************************************************/

#include <check.h>

#define PORT 1234
#define BUFLEN 1024
#define MAXMEM 20       //聊天室最大容量+1


int main(int argc, char* argv[]){
    //管理处于连接中的文件描述符
    int connfd[MAXMEM] = {0};
    int curMem = 1;

    //创建套接字用于监听连接请求
    int listenfd = socket(AF_INET,SOCK_STREAM,0);
    ERROR_CHECK(listenfd,-1,"socket");
    struct sockaddr_in serAddr;
    memset(&serAddr,0,sizeof(serAddr));
    serAddr.sin_family = AF_INET;
    serAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serAddr.sin_port = htons(PORT);

    int reuse = 1;
    int ret = setsockopt(listenfd,SOL_SOCKET,SO_REUSEPORT,&reuse,sizeof(reuse));
    ERROR_CHECK(ret,-1,"setsockopt");
    ret = bind(listenfd,(struct sockaddr*)&serAddr,sizeof(serAddr));
    ERROR_CHECK(ret,-1,"bind");

    //监听
    ret = listen(listenfd,10);
    ERROR_CHECK(ret,-1,"listen");
    puts("Status: Listening...\n");
    
    //将listenfd加入监听，用select监听是否有连接发送了数据
    char buf[BUFLEN] = {0};
    fd_set readset;
    connfd[0] = listenfd;
    int maxfd = listenfd;
    signal(SIGPIPE,SIG_IGN);

    while(1){
        //将建立了连接的文件描述符加入监听
        FD_ZERO(&readset);
        for(int i=0; i<curMem; ++i){
            FD_SET(connfd[i],&readset);
            maxfd = connfd[i]>maxfd? connfd[i]:maxfd;
        }
        ret = select(maxfd+1,&readset,NULL,NULL,NULL);

        //有新连接
        if(FD_ISSET(listenfd,&readset)){
            struct sockaddr_in cliAddr;
            socklen_t len = sizeof(cliAddr);
            memset(&cliAddr,0,len);
            connfd[curMem++] = accept(listenfd,(struct sockaddr*)&cliAddr,&len);
            char *cliIP = inet_ntoa(cliAddr.sin_addr);
            printf("Connetion from %s. %d connection(s) left.\n",cliIP,curMem-1);
        }

        //有消息需要转发
        //逐一检查有哪些连接发送了消息
        for(int i=1; i<curMem; ++i){
            memset(buf,0,BUFLEN);
            if(FD_ISSET(connfd[i],&readset)){
                errno = 0;
                ret = recv(connfd[i],buf,BUFLEN,0);
                if(-1 == ret){
                    if(EINTR == errno){
                        --i;
                        continue;
                    }
                    perror("recv");
                    continue;
                }
                //ERROR_CHECK(ret,-1,"recv");
                //接受到的消息长度为0，说明已经断开连接
                if(0 == ret){
                    close(connfd[i]);
                    if(i == curMem - 1) --curMem;
                    else connfd[i] = connfd[--curMem];
                    printf("Connetction closed. %d connection(s) left.\n",curMem-1);
                    --i;
                    continue;
                }
                //将接受到的消息转发出去
                for(int j=1; j<curMem; ++j){
                    if(j == i) continue;
                    ret = send(connfd[j],buf,ret,0);
                    if(ret == -1){
                        if(EINTR == errno){
                            --j;
                            continue;
                        }
                        perror("send");
                        exit(0);
                    }
                }
            }//处理完一条消息
        }//处理完所有消息
    }//全部连接断开
    close(listenfd);
    return 0;
}
