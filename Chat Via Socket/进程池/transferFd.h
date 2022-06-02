/*************************************************************************
    > File Name: tansferFd.h
    > Author: masker
    > Mail: https://github.com/MuYe-Code
    > Created Time: 2022年06月01日 星期三 14时41分22秒
 ************************************************************************/

#ifndef _TRANSFERFD_H_
#define _TRANSFERFD_H_

int sendfd(int pipefd, int fd);
int recvfd(int pipefd);

#endif
