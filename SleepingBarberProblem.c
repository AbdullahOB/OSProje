#include <unistd.h>          
#include <stdio.h>           
#include <stdlib.h>          
#include <pthread.h>         
#include <semaphore.h>       
#define BEKLEME_KOLTUK 5        
#define SAC_KESIMI_SURESI 1           
#define BERBER_SAYISI 2           
#define MUSTERI_SAYISI 30          
sem_t musteri;                 
sem_t berberler;                   
sem_t mutex;                     
int bosKoltukSayisi = BEKLEME_KOLTUK;
int seatPocket[BEKLEME_KOLTUK];
int sitHereNext = 0;       
int serveMeNext = 0;           
static int count = 0;             
void berberThread(void *tmp);
void customerThread(void *tmp);      
void wait();        
int main()
{   
    pthread_t berber[BERBER_SAYISI],customer[MUSTERI_SAYISI];
    int i,status=0;
    sem_init(&musteri,0,0);
    sem_init(&berberler,0,0);
    sem_init(&mutex,0,1);   
    printf("!!Berber Dukkani Acildi!!\n");
    for(i=0;i<BERBER_SAYISI;i++)                    
    {   
       status=pthread_create(&berber[i],NULL,(void *)berberThread,(void*)&i);
       sleep(1);
       if(status!=0)
          perror("!! Berber Yok !!\n");
    }
    
    for(i=0;i<MUSTERI_SAYISI;i++)                     
    {   
       status=pthread_create(&customer[i],NULL,(void *)customerThread,(void*)&i);
       wait();                   
       if(status!=0)
           perror("!!Musteri Yok!!\n");
    }   
    for(i=0;i<MUSTERI_SAYISI;i++)        
        pthread_join(customer[i],NULL);
    printf("!!berber dukkani kapandi!!\n");
    exit(EXIT_SUCCESS);  
}
void customerThread(void *tmp)  
{   
    int mySeat, B;
    sem_wait(&mutex);  
    count++;           
    printf("musteri-%d[Id:%d] dukkana girdi. ",count,pthread_self());
    if(bosKoltukSayisi > 0) 
    {
        --bosKoltukSayisi;           
        printf("musteri-%d bekleme koltuga oturdu.\n",count);
        sitHereNext = (++sitHereNext) % BEKLEME_KOLTUK;  
        mySeat = sitHereNext;
        seatPocket[mySeat] = count;
        sem_post(&mutex);                  
        sem_post(&berberler);                
        sem_wait(&musteri);              
        sem_wait(&mutex);                  
          B = seatPocket[mySeat];   
          bosKoltukSayisi++;             
        sem_post(&mutex);                       
                
    } 
    else 
    {
       sem_post(&mutex);  
       printf("musteri-%d dukkandan cikti cunku bos bekleme koltugu yoktur.\n",count);
    }
    pthread_exit(0);
}

void berberThread(void *tmp)        
{   
    int index = *(int *)(tmp);      
    int myNext, C;
    printf("berber-%d[Id:%d] dukkana girdi. ",index,pthread_self());
    while(1)            
    {   
        printf("berber-%d uyuyor.\n",index);
        sem_wait(&berberler);          
        sem_wait(&mutex);            
          serveMeNext = (++serveMeNext) % BEKLEME_KOLTUK;  
          myNext = serveMeNext;
          C = seatPocket[myNext];                  
          seatPocket[myNext] = pthread_self();     
        sem_post(&mutex);
        sem_post(&musteri);         
                
        printf("berber-%d uyandi ve musteri-%d sac kesimi basladi.\n",index,C);
        sleep(SAC_KESIMI_SURESI);
        printf("berber-%d sac kesimi bitirdi. ",index);
    }
}
void wait()     
{
     int x = rand() % (250000 - 50000 + 1) + 50000; 
     srand(time(NULL));
     usleep(x);
}
