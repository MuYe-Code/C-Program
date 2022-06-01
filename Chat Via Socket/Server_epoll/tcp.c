/*************************************************************************
    > File Name: tcp.c
    > Author: masker
    > Mail: https://github.com/MuYe-Code
    > Created Time: 2022年05月30日 星期一 19时13分22秒
 ************************************************************************/

#include "tcp.h"

//channel_t相关操作                                                                                            
bool channelInit(pchannel_t p){
    if(NULL == p) return false;
    p->lastTime = time(NULL);
    p->fd = -1;
    p->rdBufLen = 0;
    p->wrBufLen = 0;
    memset(p->rdBuf,0,CHANNELBUFLEN);
    memset(p->wrBuf,0,CHANNELBUFLEN);
    return true;
}

bool channelArrayInit(channelarray_t* pchannels){
    if(NULL == pchannels) return false;
    pchannel_t pchannel = pchannels->array;
    for(int i=0; i<MAXEVENTS; ++i){
        channelInit(pchannel);
        ++pchannel;
    }
    pchannels->count = 0;
    pchannels->maxFdIndex = 0;
    return true;
}

bool channelAdd(channelarray_t *pchannels, int fd){
    if(NULL == pchannels || pchannels->count == MAXEVENTS) return false;
    int index = channelGetIndex(pchannels, -1);
    pchannels->maxFdIndex = pchannels->maxFdIndex < index? index:pchannels->maxFdIndex;
    if(index >= 0){
        pchannels->array[index].fd = fd;
        ++(pchannels->count);
        return true;
    }
    return false;
}

bool channelDel(channelarray_t* pchannels, int fd){
    if(NULL == pchannels) return false;
    int index = channelGetIndex(pchannels, fd);
    pchannels->maxFdIndex -= pchannels->maxFdIndex == index? 1:0;
    if(index >= 0){
        channelInit(&pchannels->array[index]);
        --(pchannels->count);
    }
    return true;
}

int channelGetIndex(channelarray_t* pchannels, int fd){
    if(NULL == pchannels) return -1;
    pchannel_t array = pchannels->array;
    int n = MAXEVENTS > pchannels->maxFdIndex+2? pchannels->maxFdIndex+2:MAXEVENTS; 
    for(int i=0; i<n; ++i){
        if(fd == array[i].fd){
            return i;
        }
    }
    return -1;
}
 

//socket相关操作
int tcpSockInit(){
    int listenfd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
    ERROR_CHECK(listenfd, -1, "socket");
    struct sockaddr_in serAddr;
    memset(&serAddr, 0, sizeof(struct sockaddr_in));
    serAddr.sin_family = AF_INET;
    serAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serAddr.sin_port = htons(PORT);
    int reuse = 1;
    int ret = setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
    ERROR_CHECK(ret, -1, "setsockopt");
    ret = bind(listenfd, (struct sockaddr*)&serAddr, sizeof(struct sockaddr_in));
    ERROR_CHECK(ret, -1, "bind");
    return listenfd;
}

void setNonblock(int fd){
    int ret = 0, status = 0;
    status = fcntl(fd, F_GETFL, 0);
    status |= O_NONBLOCK;
    fcntl(fd, F_SETFL, status);
}

int myaccept(int listenfd){
    struct sockaddr_in cliAddr;
    socklen_t len = sizeof(cliAddr);
    memset(&cliAddr,0,len);
    int newfd = accept(listenfd, (struct sockaddr*)&cliAddr,&len);
    ERROR_CHECK(newfd, -1, "accept");
    struct in_addr cliIP = cliAddr.sin_addr;
    printf("Connection from IP: %s, PORT: %d\n", inet_ntoa(cliIP), ntohs(cliAddr.sin_port));
    return newfd;
}

void sendToAll(int epfd, channelarray_t* pchannels, const char *buf, int len, int except){
    if(pchannels == NULL) return;
    pchannel_t array = pchannels->array;
    for(int idx=0; idx<=pchannels->maxFdIndex; ++idx){
        if(-1 != array[idx].fd && except != array[idx].fd){
            memcpy(array[idx].wrBuf+array[idx].wrBufLen, buf, len);
            array[idx].wrBufLen += len;
            epollSetEvent(epfd, array[idx].fd, EPOLLIN | EPOLLOUT);
        }
    }
}

int sendn(int fd, const void* buf, int len, int flags){
    int left= len, ret;
    char* pbuf = (char*)buf;
    while(left > 0){
        ret = send(fd, pbuf, left, flags);
        if(-1 == ret && EINTR != errno){
            perror("send");
            return -1;
        }
        left -= ret;
        pbuf += ret;
    }
    return len - left;
}

void handleConnRequest(int epfd, int listenfd, channelarray_t *pchannels){
    int newfd = myaccept(listenfd);
    epollAddNewFd(newfd, epfd);
    channelAdd(pchannels, newfd);
    char tmp[100] = {0};
    sprintf(tmp,">>Server: New member joins, %d member(s) are online\n", pchannels->count);
    sendToAll(epfd, pchannels, tmp, strlen(tmp),-1);
}

bool handleMsgIn(int epfd, int fd, channelarray_t *pchannels){
    int idx = channelGetIndex(pchannels, fd);
    pchannel_t p = &(pchannels->array[idx]);
    int ret = recv(fd, p->rdBuf, CHANNELBUFLEN, 0);
    if(0 == ret){
        epollDelFd(fd, epfd);
        channelDel(pchannels, fd);
        char tmp[100] = {0};
        sprintf(tmp,">>Server: One member left, %d member(s) are online\n", pchannels->count);
        sendToAll(epfd, pchannels, tmp, strlen(tmp), fd);
    } else if(-1 == ret){
        perror("recv");
        return false;
    } else {
        p->lastTime = time(NULL);
        p->rdBufLen = ret;
        sendToAll(epfd, pchannels, p->rdBuf, p->rdBufLen, fd);
        p->rdBufLen = 0;
    }
    return true;
}

void handleMsgOut(int epfd, int fd, channelarray_t *pchannels){
    int idx = channelGetIndex(pchannels, fd);
    pchannel_t p = &(pchannels->array[idx]);
    if(p->wrBufLen > 0){
        int ret = send(p->fd, p->wrBuf, p->wrBufLen, 0);
        if(ret >= 0){
            p->wrBufLen -= ret;
            if(p->wrBufLen == 0){
                epollSetEvent(epfd, p->fd, EPOLLIN);
            } else {
                memmove(p->wrBuf, p->wrBuf+ret, p->wrBufLen);
            }
        }
        else perror("send");
    } else{
        epollSetEvent(epfd, p->fd, EPOLLIN);
    }
}

void handleMsg(int epfd, struct epoll_event *pevent, channelarray_t *pchannels){
    int fd = pevent->data.fd;
    if(pevent->events & EPOLLIN){
        handleMsgIn(epfd, fd, pchannels);
    }
    if(pevent->events & EPOLLOUT){
        handleMsgOut(epfd, fd, pchannels);
    }
}

//epoll相关操作
int epollAddNewFd(int newfd, int epfd){
    setNonblock(newfd);
    struct epoll_event evt;
    memset(&evt, 0, sizeof(evt));
    evt.data.fd = newfd;
    evt.events = EPOLLIN;
    int ret = epoll_ctl(epfd, EPOLL_CTL_ADD, newfd, &evt);
    if(ret < 0){
        perror("epoll_ctl");
    }
    return ret;
}

int epollDelFd(int fd, int epfd){
    struct epoll_event evt;
    evt.data.fd = fd;
    evt.events = EPOLLIN;
    int ret = epoll_ctl(epfd, EPOLL_CTL_DEL, fd, &evt);
    if(ret < 0){
        perror("epoll_ctl");
    } else {
        puts("One connection closed.");
    }
    close(fd);
    return ret;
}

void epollSetEvent(int epfd, int fd, uint32_t events){
    struct epoll_event evt;
    evt.data.fd = fd;
    evt.events = events;
    int ret = epoll_ctl(epfd, EPOLL_CTL_MOD, fd, &evt);
    if(ret < 0){
        perror("epoll_ctl");
    }
}

void epollCloseAll(channelarray_t *pchannels, int epfd){
    if(pchannels == NULL) return;
    pchannel_t channel = pchannels->array;
    for(int i=0; i<MAXEVENTS; ++i){
        if(channel[i].fd != -1){
            handleMsgOut(epfd, channel[i].fd, pchannels);
            epollDelFd(channel[i].fd, epfd);
            channelInit(&channel[i]);
        }
    }
}

//其他
bool closeTimeoutConnection(int epfd, channelarray_t *pchannels){
    time_t nowTime = time(NULL);
    int maxIndex;
    for(int i=0; i<=pchannels->maxFdIndex; ++i){
        pchannel_t p = &(pchannels->array[i]);
        if(p->fd == -1) continue;
        if(nowTime - p->lastTime > TIMEOUT && p->wrBufLen == 0){
            char sendbuf[] = ">>Server: Time out, server will close the connection.\n";
            sendn(p->fd, sendbuf, strlen(sendbuf), 0);
            printf("Time out. ");
            epollDelFd(p->fd, epfd);
            channelDel(pchannels, p->fd);
        } else {
            maxIndex = i;
        }
    }
    pchannels->maxFdIndex = maxIndex;
    return true;
}

void tcpInit(int *plistenfd, int *pepfd, channelarray_t *pchannels, struct epoll_event **pevtList){
    *plistenfd = tcpSockInit();                                     //初始化TCP连接套接字
    channelArrayInit(pchannels);                                    //初始化管道数组

    int ret = listen(*plistenfd, 50);                               //监听连接请求
    ERROR_CHECK(ret, -1, "listen");
    *pevtList = (struct epoll_event*)calloc(MAXEVENTS, sizeof(struct epoll_event));
    *pepfd = epoll_create1(0);                                      //建立I/O多路复用数据结构
    ERROR_CHECK(*pepfd, -1, "epoll_create1");

    if(epollAddNewFd(*plistenfd,*pepfd) < 0){                       //将listenfd加入监听
        exit(EXIT_FAILURE);
    }

    puts("Status: Listening...");                                   //初始化阶段完成
}
