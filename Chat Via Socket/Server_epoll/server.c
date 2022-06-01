/*************************************************************************
    > File Name: chatRoom_Epoll_Server.c
    > Author: masker
    > Mail: https://github.com/MuYe-Code
    > Created Time: 2022年05月30日 星期一 19时29分45秒
 ************************************************************************/

#include "tcp.h"

int main(int argc, char* argv[]){
    signal(SIGPIPE,SIG_IGN);
    int listenfd, epfd;                                             //处理连接请求的listenfd，管理监听数据结构的epfd
    channelarray_t channels;                                        //存放建立好连接的文件描述符
    struct epoll_event *pevtList;                                   //存放就绪的文件描述符
    tcpInit(&listenfd, &epfd, &channels, &pevtList);                //初始化TCP连接相关数据结构

    while(1){
        int ret = epoll_wait(epfd, pevtList, MAXEVENTS, 10000);     //每次监听最长时间为10秒
        closeTimeoutConnection(epfd, &channels); 		                //检查是否超时，若超时将会断开连接
        if(-1 == ret){                                              //错误处理
            if(EINTR != errno){
                perror("epoll_wait");
            }                
            continue;
        }

        for(int idx=0; idx<ret; ++idx){                             //处理消息
            int fd = pevtList[idx].data.fd;
            if(fd == listenfd){                                     //处理连接请求
                handleConnRequest(epfd, listenfd, &channels);
            } else {
                handleMsg(epfd, &(pevtList[idx]), &channels);       //转发消息
            }
        }
    }
    close(listenfd);
    return 0;
}
