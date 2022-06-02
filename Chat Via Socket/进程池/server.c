/*************************************************************************
    > File Name: chatRoom_Epoll_Server.c
    > Author: masker
    > Mail: https://github.com/MuYe-Code
    > Created Time: 2022年05月30日 星期一 19时29分45秒
 ************************************************************************/

#include "processPool.h"

int main(int argc, char* argv[]){
    ARGS_CHECK(argc, 2);
    signal(SIGPIPE,SIG_IGN);
    int listenfd, epfd;                                             //处理连接请求的listenfd，管理监听数据结构的epfd
    struct epoll_event *pevtList;                                   //存放就绪的文件描述符
    int processNum = atoi(argv[1]);
    processData_t *processArray = (processData_t*)calloc(processNum, sizeof(processData_t));
    tcpInit(&listenfd, &epfd, processArray, processNum, &pevtList);                 //初始化TCP连接相关数据结构
    
    while(1){
        int ret = epoll_wait(epfd, pevtList, MAXEVENTS, 10000);     //每次监听最长时间为10秒
        if(-1 == ret){                                              //错误处理
            if(EINTR != errno){
                perror("epoll_wait");
            }                
            continue;
        }

        for(int idx=0; idx<ret; ++idx){                             //处理消息
            int fd = pevtList[idx].data.fd;
            if(fd == listenfd){                                     //处理连接请求
                handleConnRequest(listenfd, processArray, processNum);
            } else {
                handleMsg(processArray, processNum, fd);       //转发消息
            }
        }
    }
    close(listenfd);
    return 0;
}
