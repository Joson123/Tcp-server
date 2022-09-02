#ifndef __THREAD_WORK__
#define __THREAD_WORK__
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <linux/socket.h>
typedef struct task
{
	void (*do_task)(void *arg);			//函数指针保存任务函数的首地址
	void *arg;							//函数参数
	struct task *next;					//指向下一个结点
	
}Task;
typedef struct thread_pool
{
	Task *first;						//指向任务队列的第一个结点
	Task *last;							//指向任务队列的最后一个结点
	pthread_t *tids;						//保存成功创建的线程的ID号
	int pthread_num;					//线程个数
	int task_num;						//任务个数
	pthread_mutex_t mutex;				//线程互斥锁
	pthread_cond_t cond;				//线程条件变量，用于唤醒线程
	int shutdown;						//线程池状态    	     0表示开机		1表示关机
}Pool;

Pool *pool_init(int max_pool_num);				//初始化一个线程池
void pool_add_worker(Pool * pool, void(* do_task)(void *), void * arg);		//加入任务到线程池中
void *pthread_task(void * arg);			//线程函数
void pool_destory(Pool *pool);			//销毁一个线程池






#endif