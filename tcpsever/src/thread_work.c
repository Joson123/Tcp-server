#include "thread_work.h"


/*
	线程池初始化
	@max_pool_num:线程池中的线程个数
	返回值
	成功返回 线程池的结构体的地址
	失败返回 NULL
*/
Pool *pool_init(int max_pool_num)
{
	Pool *pool = malloc(sizeof(Pool));
	if(pool == NULL)
	{
		printf("malloc failed\n");
		return NULL;
	}
	pool->tids = malloc(max_pool_num * sizeof(pthread_t));
	pool->first = NULL;
	pool->last = NULL;
	pool->task_num = 0;
	pool->pthread_num = max_pool_num;
	pthread_cond_init(&(pool->cond),NULL);
	pthread_mutex_init(&(pool->mutex),NULL);
	pool->shutdown = 0;			
	for(int i  = 0 ; i < max_pool_num ; i++)
	{
		pthread_create(pool->tids[i],NULL,pthread_task,(void *)pool);
	}
	return pool;
}

/*
	向任务队列中增加一个任务
	@pool:线程池的地址
	@do_task:任务函数
	@arg:任务函数的参数
*/

void pool_add_worker(Pool * pool, void(* do_task)(void *), void * arg)
{
	if(pool->shutdown || pool == NULL)
	{
		return;
	}
	Task *t = malloc(sizeof(Task));
	t->arg = arg;
	t->do_task = do_task;
	t->next = NULL;
	pthread_mutex_lock(&(pool->mutex));
	if(pool->first == NULL)
	{
		pool->first = NULL;
		pool->last = NULL;
	}
	else
	{
		pool->last->next = t;
		pool->last = t;
	}
	pool->task_num++;
	pthread_mutex_unlock(&(pool->mutex));
	pthread_cond_signal(&(pool->cond));
}


/*
	线程函数完成任务函数
	@arg:传参
*/

void *pthread_task(void * arg)
{
	Pool *pool = (pool *)arg;
	while(1)
	{
		pthread_mutex_lock(&(pool->mutex));
		while(pool->first == NULL && pool->shutdown == 0)
		{
			pthread_cond_wait(&(pool->cond),&(pool->mutex));
		}
		if(pool->shutdown)
		{
			pthread_mutex_unlock(&(pool->mutex));
			pthread_exit(NULL);
		}
		Task *t = pool->first;
		pool->first = pool->first->next;
		pool->task_num--;
		pthread_mutex_unlock(&(pool->mutex));
		t->do_task(t->arg);
		t->next = NULL;
		free(t);
		t = NULL;
	}
	return NULL;
}

/*
	销毁一个线程池
	@要销毁的线程池的结构体的地址
*/

void pool_destory(Pool * pool)
{
	if(pool == NULL)
	{
		return;
	}
	pool->shutdown = 1;
	for(int i = 0 ; i < pool->pthread_num ; i++)
	{
		pthread_cond_signal(&(pool->cond));
	}
	pthread_mutex_destroy(&(pool->mutex));
	pthread_cond_destroy(&(pool->cond));
	free(pool);
	pool = NULL;
}

/*
	客户端连接成功后的操作函数
	@arg:主线程传入的参数
*/

void tcp(void *arg)
{
	int *acceptsocketfd = (int *)arg;
	int ret;
	while(1)
	{
		char buf[512] = {0};
		ret = read(acceptsocketfd,buf,512);
		if(ret <= 0)
		{
			break;
		}
		printf("%s\n",buf);
		ret = write(acceptsocketfd,"successful",11);
		if(ret != 11)
		{
			perror("send failed\n");
		}
	}
}