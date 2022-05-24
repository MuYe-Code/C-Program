/*************************************************************************
    > File Name: ProAndCus.c
    > Author: masker
    > Mail: https://github.com/MuYe-Code
    > Created Time: 2022年05月24日 星期二 17时00分30秒
 ************************************************************************/

#include <check.h>

#define QueSize 10      //临界区队列长度
#define MaxOutput 50   //生产者的最大产量

typedef struct SharedRes_s{
    pthread_mutex_t mutex;  //互斥锁
    pthread_cond_t cond;    //条件变量
    int Queue[QueSize];     //队列
    int front;              //队头指针
    int rear;               //队尾指针，指向下一个可以存放的位置
    bool exitFlag;          //用于指示生产者进程是否已经退出，帮助消费者进程正常退出
}SharedRes_t,*pSharedRes_t;

//结构体初始化
void sharedResInit(pSharedRes_t p){
    int ret = 0;
    ret = pthread_mutex_init(&p->mutex,NULL);
    THREAD_ERROR_CHECK(ret,"pthread_mutex_init");
    ret = pthread_cond_init(&p->cond,NULL);
    THREAD_ERROR_CHECK(ret,"pthread_cond_init");
    p->front = 0;
    p->rear = 0;
    p->exitFlag = false;
}

//判断队列是否空
bool isEmpty(pSharedRes_t p){
    return p->front == p->rear;
}

//判断队列是否满
bool isFull(pSharedRes_t p){
    return (p->rear + 1) % QueSize == p->front; 
}

//生产者进程
void *producer(void *arg){
    int ret = 0, cnt = 1;
    srand((unsigned)time(NULL));
    pSharedRes_t p = (pSharedRes_t)arg;
    while(cnt <= MaxOutput){                            //达到最大产量后退出
        ret = pthread_mutex_lock(&p->mutex);            //加锁
        THREAD_ERROR_CHECK(ret,"pthread_mutex_lock");
        
        while(isFull(p)){
            pthread_cond_wait(&p->cond,&p->mutex);      //队列满则阻塞
        }
        int product = rand() % 100;                     //生产一个产品，更新参数
        p->Queue[p->rear] = product;
        p->rear = (p->rear + 1) % QueSize;
        printf(">>Producer add product %d\n",product);
        ++cnt;
        pthread_cond_broadcast(&p->cond);               //唤醒所有消费者进程

        if(cnt  == MaxOutput) p->exitFlag = true;       //如果即将退出，设置退出标志
        ret = pthread_mutex_unlock(&p->mutex);          //解锁
        THREAD_ERROR_CHECK(ret,"pthread_mutex_unlock");
        //usleep(50000);
    }

    printf("Producer %ld exit\n",pthread_self());       //退出
    pthread_exit(NULL);
}

//消费者进程
void *customer(void *arg){
    int ret = 0;
    srand((unsigned)time(NULL));
    pSharedRes_t p = (pSharedRes_t)arg;
    while(1){
        ret = pthread_mutex_lock(&p->mutex);
        THREAD_ERROR_CHECK(ret,"pthread_mutex_lock");
        
        while(isEmpty(p)){                                      //队列空则阻塞
            if(p->exitFlag){                                    //若同时生产者进程已退出，则表明消费者进程也应该退出
                printf("Customer %ld exit\n",pthread_self());
                ret = pthread_mutex_unlock(&p->mutex);
                THREAD_ERROR_CHECK(ret,"pthread_mutex_unlock");
                pthread_exit(NULL); 
            }
            pthread_cond_wait(&p->cond,&p->mutex);
        }
        
        printf(">>--------Customer used product %d \n",p->Queue[p->front]);
        p->front = (p->front + 1) % QueSize;                    //消费一个产品，更新数据

        pthread_cond_signal(&p->cond);                          //队列已经非满，唤醒生产者进程
        ret = pthread_mutex_unlock(&p->mutex);                  //解锁
        THREAD_ERROR_CHECK(ret,"pthread_mutex_unlock");
    }
}

int main(int argc, char* argv[]){
    SharedRes_t shared;
    sharedResInit(&shared);
    
    int ret = 0;
    //将一个生产者进程与两个消费者进程加入线程池
    pthread_t tid_p,tid_c1,tid_c2;
    ret = pthread_create(&tid_p,NULL,producer,&shared);
    THREAD_ERROR_CHECK(ret,"pthread_create");
    ret = pthread_create(&tid_c1,NULL,customer,&shared);
    THREAD_ERROR_CHECK(ret,"pthread_create");
    ret = pthread_create(&tid_c2,NULL,customer,&shared);
    THREAD_ERROR_CHECK(ret,"pthread_create");

    //回收线程
    pthread_join(tid_p,NULL);
    pthread_join(tid_c1,NULL);
    pthread_join(tid_c2,NULL);

    pthread_mutex_destroy(&shared.mutex);
    pthread_cond_destroy(&shared.cond);
    return 0;
}
