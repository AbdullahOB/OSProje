#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#define N 5
#define dusunuyor 2
#define yemekIstiyor 1
#define yemekYiyor 0
#define Sol (filozofNo + 4) % N
#define Sag (filozofNo + 1) % N
int durum[N];
int filozof[N] = { 0, 1, 2, 3, 4 };
sem_t mutex;
sem_t S[N];
void filozofTest(int filozofNo)
{
	if (durum[filozofNo] == yemekIstiyor
		&& durum[Sol] != yemekYiyor
		&& durum[Sag] != yemekYiyor) {
		durum[filozofNo] = yemekYiyor;
		sleep(2);
		printf("filozof %d Cubuklar aliyor (%d ve %d)\n",
					filozofNo + 1, Sol + 1, filozofNo + 1);
		printf("filozof %d Yemek yiyor\n", filozofNo + 1);
		sem_post(&S[filozofNo]);
	}
}
void cubukAl(int filozofNo)
{
	sem_wait(&mutex);
	durum[filozofNo] = yemekIstiyor;
	printf("filozof %d Yemek Istiyor\n", filozofNo + 1);
	filozofTest(filozofNo);
	sem_post(&mutex);
	sem_wait(&S[filozofNo]);
	sleep(1);
}


void cubukBirak(int filozofNo)
{
	sem_wait(&mutex);
	durum[filozofNo] = dusunuyor;
	printf("filozof %d cubuklari birakiyor (%d ve %d) \n",
		filozofNo + 1, Sol + 1, filozofNo + 1);
	printf("filozof %d dusunuyor\n", filozofNo + 1);
	filozofTest(Sol);
	filozofTest(Sag);
	sem_post(&mutex);
}
void* filozof_fun(void* num)
{
	while (1) {
		int* i = num;
		sleep(1);
		cubukAl(*i);
		sleep(0);
		cubukBirak(*i);
	}
}
int main()
{
	int i;
	pthread_t thread_id[N];
	sem_init(&mutex, 0, 1);
	for (i = 0; i < N; i++)
		sem_init(&S[i], 0, 0);
	for (i = 0; i < N; i++) {
		pthread_create(&thread_id[i], NULL,
					filozof_fun, &filozof[i]);
		printf("filozof %d  dusunuyor\n", i + 1);
	}
	for (i = 0; i < N; i++)
		pthread_join(thread_id[i], NULL);
}
