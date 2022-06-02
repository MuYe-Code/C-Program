/*************************************************************************
    > File Name: processPool.c
    > Author: masker
    > Mail: https://github.com/MuYe-Code
    > Created Time: 2022年06月01日 星期三 19时15分34秒
 ************************************************************************/

#include "processPool.h"


void tcpInit(int *plistenfd, int *pepfd, processData_t p[], int processNum, struct epoll_event **pevtList){
    *plistenfd = tcpSockInit();                                     //初始化TCP连接套接字
    int ret = listen(*plistenfd, 50);                               //监听连接请求
    ERROR_CHECK(ret, -1, "listen");
    *pevtList = (struct epoll_event*)calloc(MAXEVENTS, sizeof(struct epoll_event));
    *pepfd = epoll_create1(0);                                      //建立I/O多路复用数据结构
    ERROR_CHECK(*pepfd, -1, "epoll_create1");
    makeChild(p, processNum, *pepfd);

    if(epollAddNewFd(*plistenfd,*pepfd) < 0){                       //将listenfd加入监听
        exit(EXIT_FAILURE);
    }

    puts("Status: Listening...");                                   //初始化阶段完成
}                                                                                                  


int makeChild(processData_t p[], int processNum, int epfd){
    for(int i=0; i<processNum;){
        int fds[2];
        socketpair(AF_LOCAL, SOCK_STREAM, 0, fds);
        pid_t pid = fork();
        if(pid == -1 && EINTR != errno){
            perror("fork");
        } 
        if(0 == pid) {
            close(fds[1]);
            handleEvent(fds[0]);
            exit(0);
        }
        close(fds[0]);
        p[i].pid = pid;
        p[i].fd = fds[1];
        p[i].status = FREE;
        ++i;
    }
    return 0;
}

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


void handleEvent(int pipefd){
    while(1){
        int peerfd = recvfd(pipefd);
        myecho(peerfd);
        int sig = 1;
        write(pipefd, &sig, sizeof(sig));
    }
}

void myecho(int fd){
    char buff[1024] = {0};
    while(1){
        memset(buff, 0, sizeof(buff));
        int ret = recv(fd, buff, sizeof(buff), 0);
        if(ret > 0){
            ret = send(fd, buff, strlen(buff), MSG_DONTWAIT);
        } else if(0 > ret){
            perror("recv");
            return;
        } else{
            close(fd);
            return;
        }
    }
}

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


void handleConnRequest(int listenfd, processData_t p[], int processNum){
    int peerfd = myaccept(listenfd);
    int idx = getIndex(p, processNum, -1);
    p[idx].status = BUSY;
    sendfd(p[idx].fd, peerfd);
}

void handleMsg(processData_t p[], int processNum, int fd){
    int ret;
    do{
        char buff[10] = {0};
        ret = read(fd, buff, sizeof(buff));
    }while(ret == -1 && EINTR == errno);

    int idx = getIndex(p, processNum, fd);
    p[idx].status = FREE;
}

int getIndex(processData_t p[], int processNum, int pipefd){
    if(pipefd >= 0){
        for(int i=0; i<processNum; ++i){
            if(p[i].fd == pipefd){
                return i;
            }
        }
        return -1;
    }

    for(int i=0; i<processNum; ++i){
        if(p[i].status == FREE){
            return i;
        }
    }
    return -1;
}
