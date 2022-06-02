/*************************************************************************
    > File Name: processPool.h
    > Author: masker
    > Mail: https://github.com/MuYe-Code
    > Created Time: 2022年06月01日 星期三 19时07分48秒
 ************************************************************************/

#ifndef _PROCESSPOOL_H_
#define _PROCESSPOOL_H_

#include <check.h>                                                                                           
#include "transferFd.h"

#define MAXEVENTS 1000
#define PORT 1234

enum workerStatus {FREE, BUSY};

typedef struct ProcessDate{
    pid_t pid;
    int fd;
    int status;
} processData_t;

//tcp连接初始化
void tcpInit(int *plistenfd, int *pepfd, processData_t p[], int processNum, struct epoll_event **pevtList);
//初始化tcp套接字，返回套接字文件描述符
int tcpSockInit();
//初始化子进程，将相关管道加入监听
int makeChild(processData_t p[], int processNum, int epfd);
//处理方法
void handleEvent(int fd);
//回响操作
void myecho(int fd);
//将文件描述符加入监听
int epollAddNewFd(int newfd, int epfd);
//设置文件描述符为非阻塞
void setNonblock(int fd);
//取下一个连接请求，返回套接字
int myaccept(int listenfd);
//处理连接请求
void handleConnRequest(int listenfd, processData_t p[], int processNum);
//处理其他请求
void handleMsg(processData_t p[], int processNum, int fd);
//当pipefd为负时，寻找第一个空闲的子进程；否则寻找管道文件描述符为pipefd的子进程，返回下标
int getIndex(processData_t p[], int processNum, int pipefd);
    
#endif
