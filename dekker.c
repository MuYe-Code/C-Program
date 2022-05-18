#include "check.h"

typedef struct sig_s{
    bool wants_to_enter[2];
    int turn;
}Sig;

int main(void){
    //alloc a shared memory
    int shmid=shmget(IPC_PRIVATE,4096,IPC_CREAT|0666);
    ERROR_CHECK(shmid,-1,"shmget");

    //give it a value 0
    int* shmaddr=(int*)shmat(shmid,NULL,0);
    ERROR_CHECK(shmaddr,NULL,"shmat");
    (*shmaddr)=0;
    
    //use another shared memory to save semaphore
    int  shmsig=shmget(IPC_PRIVATE,4096,IPC_CREAT|0666);
    ERROR_CHECK(shmsig,-1,"shmget");

    Sig* sig=(Sig*)shmat(shmsig,NULL,0);
    ERROR_CHECK(sig,NULL,"shmat");
    sig->wants_to_enter[0]=false;
    sig->wants_to_enter[1]=false;
    sig->turn=0;

    //creat another process
    int pid=fork();
    ERROR_CHECK(pid,-1,"fork");
    
    //child process -- 1
    if(pid==0){
        //process 1 is ready
        sig->wants_to_enter[1]=true;
        //if process 0 is ready, let it first
        while(sig->wants_to_enter[0]){
            if(sig->turn != 1){
                //if it's the turn of process 0
                //process 1 will hold on until turn==1
                sig->wants_to_enter[1]=false;
                while(sig->turn != 1);
                //process 0 has done it's work, process 1 wake up
                sig->wants_to_enter[1]=true;
            }
        }
        
        //now, it's the turn of process 1
        
        //do works
        for(int i=0;i<10000000;i++){
            (*shmaddr)++;
        }

        //work done, wake up process 0
        sig->turn=0;
        sig->wants_to_enter[1]=false;
        //detach shared memory
        shmdt(shmaddr);
        shmdt(sig);
        exit(0);
    }

    //parent process -- 0
    //process 0 is ready
    sig->wants_to_enter[0]=true;
    //if process 1 is ready, let it first
    while(sig->wants_to_enter[1]){
        if(sig->turn != 0){
            //if it's the turn of process 1
            //process 0 will hold on until turn==0
            sig->wants_to_enter[0]=false;
            while(sig->turn != 0);
            //process 1 has finished it's work, then process 0 wake up
            sig->wants_to_enter[0]=true;
        }
    }

    //now, it's the turn of process 0
    //parent do works
    for(int i=0;i<10000000;i++){
        (*shmaddr)++;
    }
    //work done, wake up process 1
    sig->turn=1;
    sig->wants_to_enter[0]=false;
    //release the source of child process
    wait(0);
    
    //print the result
    printf("%d\n",*shmaddr);
    //detach the shared memory
    shmdt(shmaddr);
    shmdt(sig);
    shmctl(shmid,IPC_RMID,NULL);
    shmctl(shmsig,IPC_RMID,NULL);
    return 0;
}
