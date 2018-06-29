#include<iostream>
#include<stdio.h>
#include<pthread.h>
using namespace std;

void* myfun(void*){
	printf("this is pthread func\n");
}

int main(){
	pthread_t p_t;
	pthread_create(&p_t,NULL,myfun,NULL);

	pthread_join(p_t,NULL);

	return 0;
}
