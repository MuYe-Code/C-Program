#ifndef _CHECK_H_
#define _CHECK_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <stdbool.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/select.h>
#include <sys/time.h>

#define ARGS_CHECK(argc,num) {\
    if(argc!=num){\
        fprintf(stderr,"args error: expected %d arguments\n",num);\
        exit(1);\
    }\
}

#define ERROR_CHECK(retVal, val, msg) {\
    if(retVal==val){\
        perror(msg);\
        exit(1);\
    }\
}


#endif

