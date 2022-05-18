#include "check.h"

#define BUFLEN 1024

int main(int argc, char* argv[]) {
    //two file pathname is needed
    //copy argv[1] to argv[2]
    ARGS_CHECK(argc,3);
    int fd1[2];
    pipe(fd1);

    //fork a child processs
    pid_t pid = fork();
    ERROR_CHECK(pid, -1, "fork");
    
    //child read from file, write to pipe
    if (pid == 0) {
        close(fd1[0]);
        char buf[BUFLEN];
        
        //open file
        int fdr=open(argv[1],O_RDONLY);
        ERROR_CHECK(fdr,-1,"open");
        
        //read from file, write to pipe
        int retVal;
        while((retVal=read(fdr,buf,BUFLEN))!=0){
            ERROR_CHECK(retVal,-1,"read");
            write(fd1[1],buf,retVal);
            memset(buf,0,retVal);
        }
        close(fd1[1]);
        close(fdr);
        exit(0);
    }
    
    close(fd1[1]);
    char buf[BUFLEN];
    //open file
    int fdr=open(argv[2],O_WRONLY|O_CREAT|O_TRUNC,0666);
    ERROR_CHECK(fdr,-1,"open");
    //read from pipe, write to file
    int retVal;
    while((retVal=read(fd1[0],buf,BUFLEN))!=0){
        ERROR_CHECK(retVal,-1,"read");
        write(fdr,buf,retVal);
        memset(buf,0,BUFLEN);
    }
    wait(NULL);
    close(fdr);
    close(fd1[0]);
    return 0;
}
