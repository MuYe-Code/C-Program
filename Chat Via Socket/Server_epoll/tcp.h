/*************************************************************************
    > File Name: tcp.h
    > Author: masker
    > Mail: https://github.com/MuYe-Code
    > Created Time: 2022年05月30日 星期一 19时09分54秒
 ************************************************************************/
#ifndef _TCP_H_
#define _TCP_H_
#include <check.h>

#define CHANNELBUFLEN 1024      //应用层缓冲区大小
#define MAXEVENTS 1000          //监听的最大连接数量
#define TIMEOUT 60
#define PORT 1234

typedef struct Channel{
    int fd;                     //文件描述符
    int rdBufLen;               //读缓冲区数据长度
    int wrBufLen;               //写缓冲区数据长度
    char rdBuf[CHANNELBUFLEN];  //读缓冲区
    char wrBuf[CHANNELBUFLEN];  //写缓冲区
    time_t lastTime;            //最后活跃时间
}channel_t,*pchannel_t;

typedef struct ChannelArray{ 
    int count;                  //处于连接的文件描述符总数
    int maxFdIndex;             //最大的文件描述符在数组中的下标，减少轮询次数
    channel_t array[MAXEVENTS]; //存放通道结构的数组
}channelarray_t;

//channel_t相关操作
bool channelInit(pchannel_t p);                         //通道初始化
bool channelArrayInit(channelarray_t* pchannels);       //通道数组初始化
bool channelAdd(channelarray_t* pchannels, int fd);     //向数组添加通道
bool channelDel(channelarray_t* pchannels, int fd);     //删除文件描述符为fd的通道
int channelGetIndex(channelarray_t* pchannels, int fd); //获取文件描述符为fd的通道的下标

//socket相关操作
int tcpSockInit();                                      //初始化套接字，返回套接字文件描述符
void setNonblock(int fd);                               //为文件描述符添加非阻塞特性
int myaccept(int listenfd);                             //accept函数的封装，返回accept函数的返回值
void sendToAll(int epfd, channelarray_t* pchannels,     //向除文件描述符为except的通道外的其余通道的发送缓冲区复制buf
               const char* buf, int len, int except);
int sendn(int fd, const void* buf, int len, int flags); //send函数的封装，返回-1表示发生错误，其余表示发送的字节数

/*
 从相应缓冲区取数据，会自动进行状态操作
 两个函数需要配合使用，即函数handleMsgIn会触发函数handleMsgOut，配合完成消息转发
 */
bool handleMsgIn(int epfd, int fd, channelarray_t *pchannels);              //处理读请求，效果为向其他连接转发读缓冲区的消息
void handleMsgOut(int epfd, int fd, channelarray_t *pchannels);             //处理写请求，效果为向本连接的对端发送写缓冲区的消息
void handleMsg(int epfd, struct epoll_event *pevent, channelarray_t *pchannels);
void handleConnRequest(int epfd, int listenfd, channelarray_t *pchannels);  //处理连接请求，添加一个新的连接进入监听

//epoll相关操作
int epollAddNewFd(int newfd, int epfd);                 //将文件描述符newfd加入读监听
int epollDelFd(int fd, int epfd);                       //将文件描述符fd移除监听并关闭连接
void epollSetEvent(int epfd, int fd, uint32_t events);  //修改文件描述符的监听属性为events
void epollCloseAll(channelarray_t *pchannels, int epfd);

//其他
bool closeTimeoutConnection(int epfd, channelarray_t *pchannelarray);
void tcpInit(int *plistenfd, int *pepfd, channelarray_t *pchannels, struct epoll_event **pevtList);
#endif
