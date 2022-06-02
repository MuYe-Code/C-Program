/*************************************************************************
    > File Name: transferFd.c
    > Author: masker
    > Mail: https://github.com/MuYe-Code
    > Created Time: 2022年06月01日 星期三 14时43分26秒
 ************************************************************************/

#include <check.h>
#include "transferFd.h"

int sendfd(int pipefd, int fd)
{
    
	//1. 构造iov结构体
	char buf[10] = "hello";
	struct iovec iov;
	iov.iov_base = buf;
	iov.iov_len = strlen(buf);
    

	//2. 构造cmsghdr
	int len = CMSG_LEN(sizeof(int));
	struct cmsghdr *p = (struct cmsghdr*)calloc(1, len);
	p->cmsg_len = len;
	p->cmsg_level = SOL_SOCKET;
	p->cmsg_type = SCM_RIGHTS;
	int * pfd = (int*)(CMSG_DATA(p));
	*pfd = fd;
	//3. 构造msghdr
	struct msghdr msg;
	memset(&msg, 0, sizeof(msg));
	msg.msg_iov = &iov;
	msg.msg_iovlen = 1;
	msg.msg_control = p;
	msg.msg_controllen = len;

	int ret = sendmsg(pipefd, &msg, 0);
	ERROR_CHECK(ret, -1, "sendmsg");
}

int recvfd(int pipefd) 
{
    
	//1. 构造iov结构体
	char buf[10] = {0};
	struct iovec iov;
	iov.iov_base = buf;
	iov.iov_len = sizeof(buf);
    

	//2. 构造cmsghdr
	int len = CMSG_LEN(sizeof(int));
	struct cmsghdr *p = (struct cmsghdr*)calloc(1, len);
	p->cmsg_len = len;
	p->cmsg_level = SOL_SOCKET;
	p->cmsg_type = SCM_RIGHTS;
	//3. 构造msghdr
	struct msghdr msg;
	memset(&msg, 0, sizeof(msg));
	msg.msg_iov = &iov;
	msg.msg_iovlen = 1;
	msg.msg_control = p;
	msg.msg_controllen = len;

	int ret = recvmsg(pipefd, &msg, 0);
	ERROR_CHECK(ret, -1, "recvmsg");
	int fd = *(int*)CMSG_DATA(p);
	return fd;
}
