#include <stdio.h>
#include <pthread.h>
#define MAXN 2
pthread_cond_t cond[MAXN];
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void * function(void * args)
{
	pthread_mutex_lock(&mutex);
	printf("Waiting for a signal\n");
	pthread_cond_wait(&cond[0], &mutex);
	printf("Got signal!\n");
	pthread_mutex_unlock(&mutex);
}

void * wake(void * args)
{
	pthread_mutex_lock(&mutex);
	pthread_cond_signal(&cond[0]);
	pthread_cond_signal(&cond[0]);
	printf("Done signaling\n");
	pthread_mutex_unlock(&mutex);
}

int main()
{

	pthread_t thread;
	int b = 0;
	pthread_create(&thread, NULL, function, &b);
	pthread_t thread1;
	int a = 1;
	pthread_create(&thread1, NULL, function, &a);
	pthread_t thread2;

	pthread_create(&thread2, NULL, wake, NULL);
	pthread_join(thread, NULL);
	pthread_join(thread1, NULL);
	pthread_join(thread2, NULL);
	printf("Done.!\n");
	return 0;
}