#include<stdio.h>
#include<unistd.h>
#include<pthread.h>
using namespace std;

typedef void* (*func)(void*);

struct Task{
	Task* next;
	void* parm;
	func  myfun;
	Task(func f,void* p):next(NULL),parm(p),myfun(f){}
	Task(){}
};

struct ThreadPool{
	int PthreadNum;
	int maxTaskNum;
	int curTaskNum;
	Task* head;
	Task* end;
	pthread_mutex_t mutex;
	pthread_cond_t notEmpty;
	pthread_cond_t notFull;
	pthread_cond_t IsEmpty;
	pthread_t *p_t;
	bool TaskClose;
	bool PoolClose;
	ThreadPool(int pnum,int maxTnum){
		PthreadNum=pnum;
		maxTaskNum=maxTnum;
		curTaskNum=0;
		head=NULL;
		end=NULL;
		TaskClose=false;
		PoolClose=false;
		p_t=new pthread_t[pnum];
		pthread_mutex_init (&mutex,NULL);
		pthread_cond_init (&notEmpty,NULL);
		pthread_cond_init (&notFull,NULL);
		pthread_cond_init(&IsEmpty,NULL);
	}
	void AddTask(func f,void* p);
	~ThreadPool(){
		pthread_mutex_destroy(&mutex);
		pthread_cond_destroy(&notEmpty);
		pthread_cond_destroy(&notFull);
		pthread_cond_destroy(&IsEmpty);
		delete[] p_t;
		p_t=NULL;
	}
};


void* PthreadFunc(void* p){
	ThreadPool *pool=(ThreadPool*)p;
	while(true){
		pthread_mutex_lock(&pool->mutex);
		
		while(pool->curTaskNum==0&&(!pool->PoolClose)){
			printf("i will sleep\n");
			pthread_cond_wait(&pool->notEmpty,&pool->mutex);
			printf("i get the cpu\n");
		}
	//	printf("i get the cpu\n");
		if(pool->PoolClose){
			pthread_mutex_unlock(&pool->mutex);
			pthread_exit(NULL);
		}
		pool->curTaskNum--;
		Task* t=pool->head;
		pool->head=pool->head->next;
		if(pool->curTaskNum==0){
			pool->head=NULL;
			pool->end=NULL;
			pthread_cond_signal(&pool->IsEmpty);
		}
		else if(pool->curTaskNum==pool->maxTaskNum-1){
			pthread_cond_signal(&pool->notFull);
		}
		pthread_mutex_unlock(&pool->mutex);
		
		t->myfun(t->parm);
		delete t;

	}
}

ThreadPool* init(int pnum,int maxTnum){
	ThreadPool *pool=new ThreadPool(pnum,maxTnum);
	for(int i=0;i<pool->PthreadNum;i++){
		if(pthread_create(&((pool->p_t)[i]),NULL,PthreadFunc,(void*)pool)){
			printf("create pthread error\n");
		}
	}
	return pool;
}


int count=1;

void ThreadPool::AddTask(func f,void* parm){
	pthread_mutex_lock(&mutex);
	if(TaskClose){
		printf("the Task queue is closed\n");
		pthread_mutex_unlock(&mutex);
		return;
	}
	if(curTaskNum==maxTaskNum){
		printf("the queue is full\n");
		pthread_cond_wait(&notFull,&mutex);
	}
	Task* t=new Task(f,parm);
	t->next=head;
	head=t;
	curTaskNum++;
	if(curTaskNum==1){
		pthread_cond_broadcast(&notEmpty);
	//	printf("addTask sleep....\n");
	//	sleep(3);
	}
	pthread_mutex_unlock(&mutex);
}
void* samp(void* parm){
	int* p=(int*)parm;
	printf("Task num:%d\n",*p);
//	(*p)++;
}

void DestroyThread(ThreadPool* pool){
	pthread_mutex_lock(&pool->mutex);
	pool->TaskClose=true;
	while(pool->curTaskNum){
		pthread_cond_wait(&pool->IsEmpty,&pool->mutex);
	}
	pool->PoolClose=true;
	pthread_mutex_unlock(&pool->mutex);
	pthread_cond_broadcast(&pool->notEmpty);
	pthread_cond_broadcast(&pool->notFull);
	while(pool->head){
		Task* t=pool->head;
		pool->head=pool->head->next;
		delete t;
	}
	for(int i=0;i<pool->PthreadNum;i++){
		pthread_join(pool->p_t[i],NULL);
	}
	delete pool;
	pool=NULL;
	return;
}
int main(){
	ThreadPool* pool=init(2,6);
	sleep(2);
	int a[20];
	for(int i=0;i<20;i++){
		a[i]=i;
		pool->AddTask(samp,a+i);
		if(i==10) DestroyThread(pool);
	}
	//DestroyThread(pool);
	sleep(5);

	return 0;
}
